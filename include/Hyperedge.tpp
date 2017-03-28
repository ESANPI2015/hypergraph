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

template <typename T> T* Hyperedge::labelContains(const std::string& str)
{
    return static_cast< T* >(Hyperedge::create(_traversal(
        [&](Hyperedge *x){ return (str.empty() || (x->label().find(str) != std::string::npos)) ? true : false; },
        [](Hyperedge *x, Hyperedge *y){return true;},
        BOTH),
        "labelContains(" + str + ")"
    ));
}

template <typename T> T* Hyperedge::labelPartOf(const std::string& str)
{
    return static_cast< T* >(Hyperedge::create(_traversal(
        [&](Hyperedge *x){ return (str.empty() || (str.find(x->label()) != std::string::npos)) ? true : false; },
        [](Hyperedge *x, Hyperedge *y){return true;},
        BOTH),
        "labelPartOf(" + str + ")"
    ));
}

template <typename T> T* Hyperedge::cardinalityLessThanOrEqual(const unsigned cardinality)
{
    std::stringstream ss;
    ss << "cardinalityLessThanOrEqual(" << cardinality << ")";
    return static_cast< T* >(Hyperedge::create(_traversal(
        [&](Hyperedge *x){ return (x->pointingTo().size() <= cardinality)? true : false; },
        [](Hyperedge *x, Hyperedge *y){return true;},
        BOTH),
        ss.str()
    ));
}

template <typename T> T* Hyperedge::cardinalityGreaterThan(const unsigned cardinality)
{
    std::stringstream ss;
    ss << "cardinalityGreaterThan(" << cardinality << ")";
    return static_cast< T* >(Hyperedge::create(_traversal(
        [&](Hyperedge *x){ return (x->pointingTo().size() > cardinality)? true : false; },
        [](Hyperedge *x, Hyperedge *y){return true;},
        BOTH),
        ss.str()
    ));
}

template <typename T> T* Hyperedge::successors()
{
    return static_cast< T* >(Hyperedge::create(_traversal(
        [&](Hyperedge *x){ return x->id() != id() ? true : false; },
        [](Hyperedge *x, Hyperedge *y){return true;},
        DOWN),
        "successors(" + _label + ")"
    ));
}

template <typename T> T* Hyperedge::predecessors()
{
    return static_cast< T* >(Hyperedge::create(_traversal(
        [&](Hyperedge *x){ return x->id() != id() ? true : false; },
        [](Hyperedge *x, Hyperedge *y){return true;},
        UP),
        "predecessors(" + _label + ")"
    ));
}

template <typename T> T* Hyperedge::unite(const T* other)
{
    // We will create a Hyperedge which will contain
    // x which are part of either this->edges() or other->edges() or both
    auto edges = _to;
    edges.insert(other->_to.begin(), other->_to.end());
    return static_cast< T* >(Hyperedge::create(edges, this->label() + "||" + other->label()));
}

template <typename T> T* Hyperedge::intersect(const T* other)
{
    // We will create a Hyperedge which will contain
    // x which are part of both this->edges() and other->edges()
    T* result = static_cast< T* >(Hyperedge::create(this->label() + "&&" + other->label()));
    for (auto mineIt : _to)
    {
        if (other->_to.count(mineIt.first))
        {
           result->pointTo(mineIt.second); 
        }
    }
    return result;
}

template <typename T> T* Hyperedge::subtract(const T* other)
{
    // this - other
    // only those x which are part of this->edges() but not part of other->edges()
    T* result = static_cast< T* >(Hyperedge::create(this->label() + "/" + other->label()));
    for (auto mineIt : _to)
    {
        if (!other->_to.count(mineIt.first))
        {
           result->pointTo(mineIt.second); 
        }
    }
    return result;
}

