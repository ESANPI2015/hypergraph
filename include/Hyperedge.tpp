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
    return T::promote(Hyperedge::create(_traversal(f,g,dir), label));
}

template <typename ResultFilter, typename TraversalFilter> Hyperedge::Hyperedges Hyperedge::_traversal(
    ResultFilter f,
    TraversalFilter g,
    const Hyperedge::TraversalDirection dir
)
{
    Hyperedges result;
    Hyperedges visited;
    std::queue< unsigned > edges;

    edges.push(this->id());

    // Run through queue of unknown edges
    while (!edges.empty())
    {
        auto edge = Hyperedge::find(edges.front());
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
            auto unknown = Hyperedge::find(unknownId);
            if (g(unknown, edge)) // We need the pair of hyperedge -> hyperedge
            {
                // edge matches filter func
                edges.push(unknown->id());
            }
        }
    }

    return result;
}

