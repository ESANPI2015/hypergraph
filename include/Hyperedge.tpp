// This file holds all templated member functions
#include <map>
#include <set>
#include <queue>
#include <sstream>

template <typename T, typename ResultFilter, typename TraversalFilter> T* Hyperedge::traversal(
    ResultFilter f,
    TraversalFilter g,
    const std::string& label,
    const Hyperedge::TraversalDirection dir
)
{
    return static_cast< T* >(Hyperedge::create(_traversal(f,g,dir), label));
}

template <typename ResultFilter, typename TraversalFilter> Hyperedge::Hyperedges Hyperedge::_traversal(
    ResultFilter f,
    TraversalFilter g,
    const Hyperedge::TraversalDirection dir
)
{
    Hyperedges result;
    std::set< Hyperedge* > visited;
    std::queue< Hyperedge* > edges;

    edges.push(this);

    // Run through queue of unknown edges
    while (!edges.empty())
    {
        auto edge = edges.front();
        edges.pop();

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

        if (visited.count(edge))
            continue;

        // Visiting!!!
        visited.insert(edge);
        if (f(edge))
        {
            // edge matches filter func
            result[edge->_id] = edge;
        }

        // Inserting edges and supers into queue
        for (auto unknownIt : unknowns)
        {
            auto unknown = unknownIt.second;
            if (g(unknown, edge)) // We need the pair of hyperedge -> hyperedge
            {
                // edge matches filter func
                edges.push(unknown);
            }
        }
    }

    return result;
}

