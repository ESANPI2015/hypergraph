#include <queue>
#include <set>
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>

template<typename PartitionFunc, typename MatchFunc, typename CostFunc, typename MapFunc > CommonConceptGraph CommonConceptGraph::map (PartitionFunc p, MatchFunc m, CostFunc c, MapFunc mp) const
{
    CommonConceptGraph result(*this);

    // We have to cycle through all pairs of concepts
    Hyperedges all(result.find());
    std::set< UniqueId > toBeMapped;
    std::set< UniqueId > toBeMappedTo;

    // First step: Call the partition function to divide concepts into two sets
    for (const UniqueId& a : all)
    {
        const int decision(p(result, a));
        if (decision > 0)
        {
            toBeMapped.insert(a);
        }
        else if (decision < 0)
        {
            toBeMappedTo.insert(a);
        }
    }
     

    // As long as we have mappable concepts, we try to greedily find a (sub-)optimal mapping
    auto cmp = [] (std::pair< float, std::pair< UniqueId, UniqueId > > a, std::pair< float, std::pair< UniqueId, UniqueId > > b) -> bool { return (a.first < b.first);};
    while (toBeMapped.size() > 0)
    {
        std::priority_queue< std::pair< float, std::pair< UniqueId, UniqueId > >, std::vector< std::pair< float, std::pair< UniqueId, UniqueId > > >,  decltype(cmp) > q(cmp);
        for (const UniqueId& a : toBeMapped)
        {
            for (const UniqueId& b : toBeMappedTo)
            {
                // For each pair, we have to decide if they COULD be mapped
                if (m(result, a, b))
                {
                    // If they COULD be mapped, we have to calculate costs
                    const float costs(c(result, a, b));
                    std::cout << "\t\t" << result.read(a).label() << " -> " << result.read(b).label() << ": " << costs << std::endl;

                    // Now the mapping and its associated cost has to be inserted into a priority queue
	                // NOTE: The mapping which consumes LESS ressources is at the top of the queue!
                    q.push({costs, {a,b}});
                }
            }
        }

        // Here we have a priority queue assembled with all possible mappings, where the top element should be the mapping with LOWEST cost aka the HIGHEST amount of ressources left
        // We take it and map the two concepts
        if (!q.empty())
        {
            // Gather all candidates with the same costs at the top of the priority queue
            std::vector< std::pair< float, std::pair< UniqueId, UniqueId > > > allthebest;
            std::pair< float, std::pair< UniqueId, UniqueId > > best;
            do
            {
	       best = q.top();
               allthebest.push_back(best); 
               q.pop();
            }
            while ((!q.empty()) && (q.top().first >= best.first));
            std::cout << "\tFound " << allthebest.size() << " candidates with cost " << best.first << std::endl;

            // Now we random shuffle the vector of candidates
            std::random_device rd;
            std::shuffle(allthebest.begin(), allthebest.end(), rd);

            // And select one of it
            best = allthebest.front();
            std::cout << "\t" << result.read(best.second.first).label() << " -> " << result.read(best.second.second).label() << ": " << best.first << std::endl;

            // Call the map func to map both entities (and possibly updating ressources available)
            mp(result, best.second.first, best.second.second);

            // Remove only the first entity from 
            toBeMapped.erase(best.second.first);
        } else {
            std::cout << "\tCould not map: ";
            for (const UniqueId& uid : toBeMapped)
            {
                std::cout << result.read(uid).label() << " ";
            }
            std::cout << std::endl;
	    break;
	}
    }

    return result;
}
