#include <set>
#include <queue>

template< typename ConceptFilterFunc, typename RelationFilterFunc > Hyperedges Conceptgraph::traverse(
                    const UniqueId& rootId,
                    ConceptFilterFunc cf,
                    RelationFilterFunc rf,
                    const TraversalDirection dir) const
{
    Hyperedges result;
    std::set< UniqueId > visited;
    std::queue< UniqueId > toVisit;

    toVisit.push(rootId);

    // Run through queue of unknown edges
    while (!toVisit.empty())
    {
        const UniqueId currentUid(toVisit.front());
        toVisit.pop();

        if (visited.count(currentUid))
            continue;

        // Visiting!!!
        visited.insert(currentUid);

        // Insert the hedge iff the ConceptFilterFunc says so
        if (cf(read(currentUid)))
        {
            // edge matches filter
            result.push_back(currentUid);
        }

        // Get the relations of currentUid depending on dir
        switch (dir)
        {
            case FORWARD:
                {
                    Hyperedges relations(relationsFrom(Hyperedges{currentUid}));
                    for (const UniqueId& relUid : relations)
                    {
                        // If RelationFilterFunc returns true, we push all targets of it to the toVisit queue
                        if (rf(read(currentUid), read(relUid)))
                        {
                            for (const UniqueId& otherUid : read(relUid).pointingTo())
                                toVisit.push(otherUid);
                        }
                    }
                }
                break;
            case BOTH:
                {
                    Hyperedges relations(relationsFrom(Hyperedges{currentUid}));
                    for (const UniqueId& relUid : relations)
                    {
                        // If RelationFilterFunc returns true, we push all targets of it to the toVisit queue
                        if (rf(read(currentUid), read(relUid)))
                        {
                            for (const UniqueId& otherUid : read(relUid).pointingTo())
                                toVisit.push(otherUid);
                        }
                    }
                }
            case INVERSE:
                {
                    Hyperedges relations(relationsTo(Hyperedges{currentUid}));
                    for (const UniqueId& relUid : relations)
                    {
                        // If RelationFilterFunc returns true, we push all sources of it to the toVisit queue
                        if (rf(read(currentUid), read(relUid)))
                        {
                            for (const UniqueId& otherUid : read(relUid).pointingFrom())
                                toVisit.push(otherUid);
                        }
                    }
                }
                break;
        }
    }

    return result;
}
