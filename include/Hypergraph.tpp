// This file holds all templated member functions
#include "Hyperedge.hpp"
#include <map>
#include <set>
#include <queue>
#include <sstream>

template <typename ResultFilter, typename TraversalFilter> Hyperedges Hypergraph::traversal(
    const UniqueId rootId,
    ResultFilter f,
    TraversalFilter g,
    const Hypergraph::TraversalDirection dir
)
{
    Hyperedges result;
    Hyperedges visited;
    std::queue< UniqueId > edges;

    edges.push(rootId);

    // Run through queue of unknown edges
    while (!edges.empty())
    {
        auto edge = get(edges.front());
        edges.pop();
        // NOTE: We do not check the pointer here! We want it to fail if there is inconsistency!!!

        if (visited.count(edge->id()))
            continue;

        // Visiting!!!
        visited.insert(edge->id());
        if (f(edge))
        {
            // edge matches filter func
            result.insert(edge->id());
        }

        // Handle search direction
        Hyperedges unknowns;
        switch (dir)
        {
            case DOWN:
                unknowns = nextNeighboursOf(Hyperedges{edge->id()});
                break;
            case UP:
                unknowns = prevNeighboursOf(Hyperedges{edge->id()});
                break;
            case BOTH:
                unknowns = allNeighboursOf(Hyperedges{edge->id()});
                break;
            default:
                result.clear();
                return result;
        }

        // Inserting unknowns into queue for further searching
        for (auto unknownId : unknowns)
        {
            auto unknown = get(unknownId);
            if (g(edge, unknown)) // We need the pair of hyperedge -> hyperedge
            {
                // edge matches filter func
                edges.push(unknown->id());
            }
        }
    }

    return result;
}

