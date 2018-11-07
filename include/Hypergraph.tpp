// This file holds all templated member functions
#include "Hyperedge.hpp"
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <queue>
#include <sstream>
#include <climits>
#include <iostream>

/*
* This algorithm is a single pushout (SPO) graph transformation algorithm
*/
template< typename MatchFunc > Hypergraph Hypergraph::rewrite(const Hypergraph& lhs, const Hypergraph& rhs, const Mapping& partialMap, std::stack< Mapping >& searchSpace, MatchFunc mf) const
{
    Hypergraph result;
    Mapping original2new;
    Mapping replacement2new;

    // First step: Find a match of lhs in this graph
    Mapping m(match(lhs, searchSpace, mf));
    // No match? Return empty graph
    if (!m.size())
        return result;

    // Second step: recreate all HYPEREDGES which are NOT PART of the match! (G\lhs)
    Hyperedges originals(find());
    Mapping mInv(invert(m));
    for (const UniqueId& originalId : originals)
    {
        if (mInv.count(originalId))
            continue;
        // Does not exist for sure, so will always succeed
        result.create(originalId, read(originalId).label());
        original2new.insert({originalId, originalId});
    }

    // Third step: Cycle over matches
    // Here, we either delete OR preserve & alter originals
    // NOTE: Deletion here is IMPLICIT (by not creating it)
    // m : matchedId -> originalId, partialMap : matchedId -> replacementId
    Mapping original2replacement(join(m, partialMap));
    for (const auto& pair : original2replacement)
    {
	    UniqueId originalId(pair.first);
	    UniqueId replacementId(pair.second);

	    // Handle label
	    std::string label(rhs.read(replacementId).label());
	    if (label.empty())
	    {
	        label = read(originalId).label();
	    }

	    // Handle uid
        // We want to keep the original UID unless ...
        UniqueId uid(originalId);

	    // ... multiple originals shall be replaced by one. Then we have to map to the SAME UID (merge)
        Mapping::const_iterator it(replacement2new.find(replacementId));
        if (it != replacement2new.end())
        {
            // If an original already has been altered by the same replacement UID, we reuse the UID of that original
            uid = it->second;
        }

	    // ... one original shall be replaced by multiple replacements
	    it = original2new.find(originalId);
	    if (it != original2new.end())
	    {
	        // If a completely new hedge has to be created we have to generate a UID by ourselves ... The best we could do is concatenating the known UIDs?!
	        uid = originalId + replacementId;
	    }

        // Here, we know everything we need to perform the replacement
        result.create(uid, label);
        original2new.insert({originalId, uid});
        replacement2new.insert({replacementId, uid});
    }

    // Fourth step: Now all hedges in rhs which are not in result yet, have to be created
    Hyperedges replacements(rhs.find());
    Mapping rInv(invert(partialMap));
    for (const UniqueId replacementId : replacements)
    {
        if (rInv.count(replacementId))
            continue;
        // Create the new hedge. If uid is already taken, it will map to the same element
        result.create(replacementId, rhs.read(replacementId).label());
        replacement2new.insert({replacementId, replacementId});
    }


    // Fifth step: Wiring
    // A) Reconstruct wiring of original hedges
    Mapping::const_iterator srcPair;
    for (srcPair = original2new.begin(); srcPair != original2new.end(); srcPair++)
    {
        UniqueId firstIdOld(srcPair->first);
        UniqueId firstId(srcPair->second);

        // Remember that we only have to handle the pairs (first, *) once, so the next loop can start at srcPair!
        Mapping::const_iterator destPair;
        for (destPair = srcPair; destPair != original2new.end(); destPair++)
        {
            UniqueId secondIdOld(destPair->first);
            UniqueId secondId(destPair->second);

	    // If both hedges are part of the MATCH we should not reconstruct the wiring
            if (mInv.count(firstIdOld) && mInv.count(secondIdOld))
		continue;

            // Reconstruct wiring
            if (read(firstIdOld).isPointingTo(secondIdOld))
                result.get(firstId)->to(secondId);
            if (read(secondIdOld).isPointingTo(firstIdOld))
                result.get(secondId)->to(firstId);
            if (read(firstIdOld).isPointingFrom(secondIdOld))
                result.get(firstId)->from(secondId);
            if (read(secondIdOld).isPointingFrom(firstIdOld))
                result.get(secondId)->from(firstId);
        }
    }

    // B) Reconstruct wiring of the replacement graph
    for (srcPair = replacement2new.begin(); srcPair != replacement2new.end(); srcPair++)
    {
        UniqueId firstIdOld(srcPair->first);
        UniqueId firstId(srcPair->second);

        // Remember that we only have to handle the pairs (first, *) once, so the next loop can start at srcPair!
        Mapping::const_iterator destPair;
        for (destPair = srcPair; destPair != replacement2new.end(); destPair++)
        {
            UniqueId secondIdOld(destPair->first);
            UniqueId secondId(destPair->second);

            // Reconstruct wiring
            if (rhs.read(firstIdOld).isPointingTo(secondIdOld))
                result.get(firstId)->to(secondId);
            if (rhs.read(secondIdOld).isPointingTo(firstIdOld))
                result.get(secondId)->to(firstId);
            if (rhs.read(firstIdOld).isPointingFrom(secondIdOld))
                result.get(firstId)->from(secondId);
            if (rhs.read(secondIdOld).isPointingFrom(firstIdOld))
                result.get(secondId)->from(firstId);
        }
    }

    return result;
}

/*
	Possible improvements:
        * Instead of searching for possible candidates beforhand, we search only for the candidates for ONE query hedge (BB-Graph algorithm)
        * Selecting the next query hedge and its candidates should respect locality of the already mapped hedges
*/
template< typename MatchFunc > Mapping Hypergraph::match(const Hypergraph& other, std::stack< Mapping >& searchSpace, MatchFunc m) const
{
    // This algorithm is according to Ullmann
    // and has been implemented following "An In-depth Comparison of Subgraph Isomorphism Algorithms in Graph Databases"
    // First step: For each vertex in subgraph, we find other suitable candidates
    // TODO: We need this candidate filtering to be stored in the search space as well!!!! Then we could skip it.
    unsigned int minCandidates = UINT_MAX;
    unsigned int maxDegree = 0;
    int bestValue = INT_MIN;
    UniqueId startUid;
    Hyperedges otherIds(other.find());
    std::unordered_map< UniqueId, Hyperedges > candidateIds;
    for (const UniqueId& otherId : otherIds)
    {
        // Find candidates by matching func
        candidateIds[otherId] = m(*this, other.read(otherId));
	// First check: No candidates, no match
	if (!candidateIds.size())
            return Mapping();
	// Check candidate size
	if (candidateIds[otherId].size() < minCandidates)
            minCandidates = candidateIds[otherId].size();
        // Check degree
        unsigned int degree(other.read(otherId).indegree() + other.read(otherId).outdegree());
        if (degree > maxDegree)
            maxDegree = degree;
        // Check if maxDegree - minCandidates > bestValue
        int value = maxDegree - minCandidates;
        if (value < bestValue)
            continue;
        // If we are here, we found a good starting hedge
        bestValue = value;
        startUid = otherId;
    }

    // Second step: Prepopulate the searchSpace with all mappings from startUid -> candidateId in candidateIds
    // ONLY IF SEARCH SPACE IS EMPTY
    if (searchSpace.empty())
    {
        for (const UniqueId& candidateId : candidateIds[startUid])
        {
            Mapping initial;
            initial.insert({startUid,candidateId});
            searchSpace.push(initial);
        }
    }

    while (!searchSpace.empty())
    {
        // Get top of stack
        Mapping currentMapping(searchSpace.top());
        searchSpace.pop();

        // Check if we can stop the search
        if (currentMapping.size() == otherIds.size())
        {
            return currentMapping;
        }

	// Custom selection:
	// * should be a neighbour of already matched query nodes
	// * should have the minimum amount of candidates
	// * should have the maximum degree
        minCandidates = UINT_MAX;
        maxDegree = 0;
        unsigned int maxOverlap = 0;
        bestValue = INT_MIN;
	UniqueId unmappedId;
	for (const UniqueId& otherId : otherIds)
	{
            // If mapped, skip
            if (currentMapping.find(otherId) != currentMapping.end())
                continue;

	    // Check candidate size
	    if (candidateIds[otherId].size() < minCandidates)
                minCandidates = candidateIds[otherId].size();

            // Check degree
            unsigned int degree(other.read(otherId).indegree() + other.read(otherId).outdegree());
            if (degree > maxDegree)
                maxDegree = degree;

            // Check neighbourhood to already mapped hedges
	    Hyperedges neighbourhood(other.allNeighboursOf(Hyperedges{otherId}));
            unsigned overlap = 0;
            for (const UniqueId& neighbourId : neighbourhood)
            {
                if (currentMapping.find(neighbourId) != currentMapping.end())
                    overlap++;
            }
	    if (overlap > maxOverlap)
                maxOverlap = overlap;

            int value = maxDegree + maxOverlap - minCandidates;
            if (value < bestValue)
                continue;
            // If we are here, we found a good unmapped hedge
            bestValue = value;
	    unmappedId = otherId;
	}

        // Found unmapped hedge
        // NOTE: This is actually what makes this method an Ullmann algorithm
        Mapping currentMappingInv(invert(currentMapping));
        Hyperedges candidates(candidateIds[unmappedId]);
        Hyperedges unmappedNextNeighbours(other.to(Hyperedges{unmappedId}));
        Hyperedges unmappedPrevNeighbours(other.from(Hyperedges{unmappedId}));
        for (const UniqueId& candidateId : candidates)
        {
            // If we want a bijective matching, we have to make sure that candidates are not mapped multiple times!!!
            if (currentMappingInv.find(candidateId) != currentMappingInv.end())
                continue;

            // We have now the neighbourhood of the unmapped hedge and the neighbourhood of the candidate
            Hyperedges candidateNextNeighbours(to(Hyperedges{candidateId}));
            Hyperedges candidatePrevNeighbours(from(Hyperedges{candidateId}));
            // If the candidate neighbourhood is less than the unmapped neighbourhood, a future match is IMPOSSIBLE
            if (candidateNextNeighbours.size() < unmappedNextNeighbours.size())
                continue;
            if (candidatePrevNeighbours.size() < unmappedPrevNeighbours.size())
                continue;

            // NOTE: Degree has already been checked in initial candidate filtering
            // Construct the new match
            Mapping newMapping(currentMapping);
            newMapping.insert({unmappedId, candidateId});

            // Check for validity (QUICKSI style)
            // For a correct mapping we have to check if all from and to sets are correct (similar to the check in rewrite)
            bool valid = true;
            for (const auto& pair : newMapping)
            {
                Hyperedges templatePointsTo(other.to(Hyperedges{pair.first}));
                Hyperedges templatePointsFrom(other.from(Hyperedges{pair.first}));
                Hyperedges matchPointsTo(to(Hyperedges{pair.second}));
                Hyperedges matchPointsFrom(from(Hyperedges{pair.second}));
                for (const UniqueId& templateId : templatePointsTo)
                {
                    Mapping::const_iterator it(newMapping.find(templateId));
                    if (it == newMapping.end())
                        continue;
                    UniqueId matchId(it->second);
                    if (std::find(matchPointsTo.begin(), matchPointsTo.end(), matchId) == matchPointsTo.end())
                    {
                        valid = false;
                        break;
                    }
                }
                if (!valid)
                    break;
                for (const UniqueId& templateId : templatePointsFrom)
                {
                    Mapping::const_iterator it(newMapping.find(templateId));
                    if (it == newMapping.end())
                        continue;
                    UniqueId matchId(it->second);
                    if (std::find(matchPointsFrom.begin(), matchPointsFrom.end(), matchId) == matchPointsFrom.end())
                    {
                        valid = false;
                        break;
                    }
                }
                if (!valid)
                    break;
            }
            if (!valid)
                continue;

            // Insert valid mapping
            searchSpace.push(newMapping);
        }
    }
    return Mapping();
}

template <typename ResultFilter, typename TraversalFilter> Hyperedges Hypergraph::traversal(
    const UniqueId& rootId,
    ResultFilter f,
    TraversalFilter g,
    const Hypergraph::TraversalDirection dir
) const
{
    Hyperedges result;
    std::set< UniqueId > visited;
    std::queue< UniqueId > edges;

    edges.push(rootId);

    // Run through queue of unknown edges
    while (!edges.empty())
    {
        const UniqueId currentUid(edges.front());
        edges.pop();
        // NOTE: We do not check the pointer here! We want it to fail if there is inconsistency!!!

        if (visited.count(currentUid))
            continue;

        // Visiting!!!
        visited.insert(currentUid);
        if (f(read(currentUid)))
        {
            // edge matches filter func
            result.push_back(currentUid);
        }

        // Handle search direction
        Hyperedges unknowns;
        switch (dir)
        {
            case FORWARD:
                unknowns = nextNeighboursOf(Hyperedges{currentUid});
                break;
            case INVERSE:
                unknowns = prevNeighboursOf(Hyperedges{currentUid});
                break;
            case BOTH:
                unknowns = allNeighboursOf(Hyperedges{currentUid});
                break;
            default:
                result.clear();
                return result;
        }

        // Inserting unknowns into queue for further searching
        for (const UniqueId& unknownId : unknowns)
        {
            if (g(read(currentUid), read(unknownId)))
            {
                // edge matches filter func
                edges.push(unknownId);
            }
        }
    }

    return result;
}

