// This file holds all templated member functions
#include "Hyperedge.hpp"
#include <map>
#include <set>
#include <queue>
#include <sstream>

template <typename ResultFilter, typename TraversalFilter> unsigned Hypergraph::traversal(
    const unsigned rootId,
    ResultFilter f,
    TraversalFilter g,
    const std::string& label,
    const Hypergraph::TraversalDirection dir
)
{
    return create(_traversal(rootId,f,g,dir), label);
}

template <typename ResultFilter, typename TraversalFilter> Hypergraph::Hyperedges Hypergraph::_traversal(
    const unsigned rootId,
    ResultFilter f,
    TraversalFilter g,
    const Hypergraph::TraversalDirection dir
)
{
    Hyperedges result;
    Hyperedges visited;
    std::queue< unsigned > edges;

    edges.push(rootId);

    // Run through queue of unknown edges
    while (!edges.empty())
    {
        auto edge = get(edges.front());
        edges.pop();
        // NOTE: We do not check the pointer here! We want it to fail if there is inconsistency!!!

        // Handle search direction
        Hyperedges unknowns;
        switch (dir)
        {
            case DOWN:
                unknowns.insert(edge->_to.begin(), edge->_to.end());
                break;
            case BOTH:
                unknowns.insert(edge->_to.begin(), edge->_to.end());
            case UP:
                unknowns.insert(edge->_from.begin(), edge->_from.end());
                break;
            default:
                result.clear();
                return result;
        }

        if (visited.count(edge->id()))
            continue;

        // Visiting!!!
        visited.insert(edge->id());
        if (f(edge))
        {
            // edge matches filter func
            result.insert(edge->id());
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

