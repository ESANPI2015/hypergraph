#include <set>
#include <queue>

template< typename ConceptFilterFunc, typename RelationFilterFunc > Hyperedges Conceptgraph::traverse(
                    const UniqueId& rootId,
                    ConceptFilterFunc cf,
                    RelationFilterFunc rf,
                    const TraversalDirection dir) const
{
    // NOTE: We cannot use Hypergraph::traverse here, because concepts do not point to neighbouring relations (yet?)
    // That means, that we have C <-R-> C and not C-> R-> C
    Hyperedges result;
    std::set< UniqueId > visited;
    std::queue< UniqueId > toVisit;
    std::queue< Hyperedges > path;

    toVisit.push(rootId);
    path.push(Hyperedges{rootId});

    // Run through queue of unknown edges
    while (!toVisit.empty())
    {
        const UniqueId currentUid(toVisit.front());
        toVisit.pop();
        Hyperedges currentPath(path.front());
        path.pop();

        if (visited.count(currentUid))
            continue;

        // Visiting!!!
        visited.insert(currentUid);

        // Insert the hedge iff the ConceptFilterFunc says so
        if (cf(*this, currentUid, currentPath))
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
                        if (rf(*this, currentUid, relUid))
                        {
                            for (const UniqueId& otherUid : access(relUid).pointingTo())
                            {
                                currentPath.push_back(otherUid);
                                path.push(currentPath);
                                toVisit.push(otherUid);
                            }
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
                        if (rf(*this, currentUid, relUid))
                        {
                            for (const UniqueId& otherUid : access(relUid).pointingTo())
                            {
                                currentPath.push_back(otherUid);
                                path.push(currentPath);
                                toVisit.push(otherUid);
                            }
                        }
                    }
                }
            case INVERSE:
                {
                    Hyperedges relations(relationsTo(Hyperedges{currentUid}));
                    for (const UniqueId& relUid : relations)
                    {
                        // If RelationFilterFunc returns true, we push all sources of it to the toVisit queue
                        if (rf(*this, currentUid, relUid))
                        {
                            for (const UniqueId& otherUid : access(relUid).pointingFrom())
                            {
                                currentPath.push_back(otherUid);
                                path.push(currentPath);
                                toVisit.push(otherUid);
                            }
                        }
                    }
                }
                break;
        }
    }

    return result;
}
