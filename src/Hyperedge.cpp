#include "Hyperedge.hpp"
#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <queue>
#include <stdexcept>


// STATIC MEMBER INIT
unsigned Hyperedge::_lastId = 1;
Hyperedge::Hyperedges Hyperedge::_created;

// CONSTRUCTORS
Hyperedge::Hyperedge(const std::string& label)
: _label(label)
{
    _id = _lastId++;
}

Hyperedge::Hyperedge(Hyperedges edges, const std::string& label)
{
    _label = label;
    _id = _lastId++;
    for (auto edge : edges)
    {
        pointTo(edge.second);
    }
}

// DESTRUCTORS
Hyperedge::~Hyperedge()
{
    // First we detach from all our edges AND supers
    detach();

    // Check if we are in _created pool. If yes, delete
    if (_created.count(_id))
    {
        _created.erase(_id);
    }
}

// PUBLIC FACTORY
// TODO: We should throw if needed ...
Hyperedge* Hyperedge::create(const std::string& label)
{
    Hyperedge* neu = new Hyperedge(label);
    _created[neu->_id] = neu;
    return neu;
}

Hyperedge* Hyperedge::create(Hyperedges edges, const std::string& label)
{
    Hyperedge* neu = new Hyperedge(edges,label);
    _created[neu->_id] = neu;
    return neu;
}

void Hyperedge::detach()
{
    clear();
    seperate();
}

void Hyperedge::seperate()
{
    // Deregister from edges pointing to us (registered in its to set)
    for (auto edgeIt : _from)
    {
        auto edge = edgeIt.second;
        if (edge->_to.count(_id))
        {
            edge->_to.erase(_id);
        }
    }
    // Clear from edges poiting to us
    _from.clear();
}

void Hyperedge::clear()
{
    // Deregister from edges (registered in its from set)
    for (auto edgeIt : _to)
    {
        auto edge = edgeIt.second;
        if (edge->_from.count(_id))
        {
            edge->_from.erase(_id);
        }
    }
    // Clear edges
    _to.clear();
}

void Hyperedge::cleanup()
{
    auto localCopy = _created;

    // detach all registered hyperedges
    for (auto edgeIt : localCopy)
    {
        auto edge = edgeIt.second;
        edge->detach();
    }

    // finally destroy
    for (auto edgeIt : localCopy)
    {
        auto edge = edgeIt.second;
        delete edge;
    }
    _created.clear();
}

// PRIVATE FACTORY
Hyperedge* Hyperedge::create(const unsigned id, const std::string& label)
{
    Hyperedge* neu = NULL;
    if (!_created.count(id))
    {
        // Can do it
        neu = new Hyperedge(label);
        neu->_id = id;
        // Ensure monotonic increasing _lastId
        _lastId = (id > _lastId) ? id : _lastId;
    }
    return neu;
}

Hyperedge* Hyperedge::create(const unsigned id, Hyperedges edges, const std::string& label)
{
    Hyperedge* neu = NULL;
    if (!_created.count(id))
    {
        // Can do it
        neu = new Hyperedge(edges, label);
        neu->_id = id;
        // Ensure monotonic increasing _lastId
        _lastId = (id > _lastId) ? id : _lastId;
    }
    return neu;
}

bool Hyperedge::pointTo(Hyperedge *edge)
{
    if (_to.count(edge->_id))
        return true;
        // Check if we are in the from set of edge
        // Make sure we are in the from list
        if (!edge->_from.count(_id))
        {
            edge->_from[_id] = this;
        }
        // ... and the edge registered in our edge list
        _to[edge->_id] = edge;
    
    return true;
}

unsigned Hyperedge::id() const
{
    return _id;
}

std::string Hyperedge::label() const
{
    return _label;
}

unsigned Hyperedge::cardinality() const
{
    return _to.size();
}

Hyperedge::Hyperedges Hyperedge::pointedBy(const std::string& label)
{
    Hyperedges result;
    for (auto edgeIt : _from)
    {
        auto edge = edgeIt.second;
        // Filters by label if given. It suffices that the edge label contains the given one.
        if (label.empty() || (edge->label().find(label) != std::string::npos))
            result[edge->_id] = edge;
    }
    return result;
}

Hyperedge::Hyperedges Hyperedge::pointingTo(const std::string& label)
{
    Hyperedges result;
    for (auto edgeIt : _to)
    {
        auto edge = edgeIt.second;
        // Filters by label if given. It suffices that the edge label contains the given one.
        if (label.empty() || (edge->label().find(label) != std::string::npos))
            result[edge->_id] = edge;
    }
    return result;
}


Hyperedge* Hyperedge::labelContains(const std::string& str)
{
    return Hyperedge::create(_traversal(
        [&](Hyperedge *x){ return (str.empty() || (x->label().find(str) != std::string::npos)) ? true : false; },
        [](Hyperedge *x){return true;},
        BOTH),
        "labelContains(" + str + ")"
    );
}

Hyperedge* Hyperedge::labelPartOf(const std::string& str)
{
    return Hyperedge::create(_traversal(
        [&](Hyperedge *x){ return (str.empty() || (str.find(x->label()) != std::string::npos)) ? true : false; },
        [](Hyperedge *x){return true;},
        BOTH),
        "labelPartOf(" + str + ")"
    );
}

Hyperedge* Hyperedge::cardinalityLessThanOrEqual(const unsigned cardinality)
{
    std::stringstream ss;
    ss << "cardinalityLessThanOrEqual(" << cardinality << ")";
    return Hyperedge::create(_traversal(
        [&](Hyperedge *x){ return (x->pointingTo().size() <= cardinality)? true : false; },
        [](Hyperedge *x){return true;},
        BOTH),
        ss.str()
    );
}

Hyperedge* Hyperedge::cardinalityGreaterThan(const unsigned cardinality)
{
    std::stringstream ss;
    ss << "cardinalityGreaterThan(" << cardinality << ")";
    return Hyperedge::create(_traversal(
        [&](Hyperedge *x){ return (x->pointingTo().size() > cardinality)? true : false; },
        [](Hyperedge *x){return true;},
        BOTH),
        ss.str()
    );
}

Hyperedge* Hyperedge::successors()
{
    return Hyperedge::create(_traversal(
        [&](Hyperedge *x){ return x->id() != id() ? true : false; },
        [](Hyperedge *x){return true;},
        DOWN),
        "successors(" + _label + ")"
    );
}

Hyperedge* Hyperedge::predecessors()
{
    return Hyperedge::create(_traversal(
        [&](Hyperedge *x){ return x->id() != id() ? true : false; },
        [](Hyperedge *x){return true;},
        UP),
        "predecessors(" + _label + ")"
    );
}

template <typename ResultFilter, typename TraversalFilter> Hyperedge* Hyperedge::traversal(
    ResultFilter f,
    TraversalFilter g,
    const std::string& label,
    const Hyperedge::TraversalDirection dir
)
{
    return Hyperedge::create(_traversal(f,g,dir), label);
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
            if (g(edge))
            {
                // edge matches filter func
                edges.push(unknown);
            }
        }
    }

    return result;
}

Hyperedge* Hyperedge::unite(const Hyperedge* other)
{
    // We will create a Hyperedge which will contain
    // x which are part of either this->edges() or other->edges() or both
    auto edges = _to;
    edges.insert(other->_to.begin(), other->_to.end());
    return Hyperedge::create(edges, this->label() + "||" + other->label());
}

Hyperedge* Hyperedge::intersect(const Hyperedge* other)
{
    // We will create a Hyperedge which will contain
    // x which are part of both this->edges() and other->edges()
    Hyperedge* result = Hyperedge::create(this->label() + "&&" + other->label());
    for (auto mineIt : _to)
    {
        if (other->_to.count(mineIt.first))
        {
           result->pointTo(mineIt.second); 
        }
    }
    return result;
}

Hyperedge* Hyperedge::subtract(const Hyperedge* other)
{
    // this - other
    // only those x which are part of this->edges() but not part of other->edges()
    Hyperedge* result = Hyperedge::create(this->label() + "/" + other->label());
    for (auto mineIt : _to)
    {
        if (!other->_to.count(mineIt.first))
        {
           result->pointTo(mineIt.second); 
        }
    }
    return result;
}

std::ostream& operator<< (std::ostream& stream, const Hyperedge& edge)
{
    stream << edge.id() << ":";
    stream << edge.label() << "[";
    for (auto otherIt : edge._to)
    {
        auto other = otherIt.second;
        stream << " " << other->id() << " ";
    }
    stream << "]";
    return stream;
}

//std::string Hyperedge::serialize(Hyperedge* root)
//{
//    std::stringstream result;
//    std::set< Hyperedge* > visited;
//    std::queue< Hyperedge* > edges;
//
//    edges.push(root);
//
//    // Run through queue of unknown edges
//    while (!edges.empty())
//    {
//        auto edge = edges.front();
//        edges.pop();
//
//        if (visited.count(edge))
//            continue;
//
//        // Visiting!!!
//        visited.insert(edge);
//        result << edge->id() << ":" << edge->label();
//
//        // Inserting edges into queue
//        for (auto unknownIt : edge->edges())
//        {
//            auto unknown = unknownIt.second;
//            result << "[" << unknown->id() << "]";
//            edges.push(unknown);
//        }
//        result << "\n";
//    }
//
//    return result.str();
//}
//
//Hyperedge* Hyperedge::deserialize(const std::string& from)
//{
//    Hyperedge *root = NULL;
//    Hyperedge::Hyperedges known;
//    std::set< unsigned > roots;
//    
//    auto spos = 0;
//    auto cpos = from.find(":");
//    auto opos = from.find("[");
//    auto npos = from.find("\n");
//
//    // Run through string
//    while (npos < from.size())
//    {
//        // Extract id
//        auto id = std::stoul(from.substr(spos, cpos-spos));
//        // Extract label
//        auto label = from.substr(cpos+1, npos-cpos-1);
//        if (opos < npos)
//            label = from.substr(cpos+1, opos-cpos-1);
//
//        // Go through all edges, create them if necessary and update current hyperedge
//        Hyperedge::Hyperedges edges;
//        while (opos < npos)
//        {
//            auto edgeId = std::stoul(from.substr(opos+1, from.find("]",opos+1)-opos-1));
//            // Find label in map (and create it iff not found)
//            if (!known.count(edgeId))
//            {
//                known[edgeId] = Hyperedge::create(edgeId,"");
//            }
//            edges[edgeId] = known[edgeId];
//            // Whenever something gets a edge, it cannot be a root anymore
//            if (roots.count(edgeId))
//            {
//                roots.erase(edgeId);
//            }
//            opos = from.find("[", opos+1);
//        }
//
//        // Find id in map, create or update it
//        if (!known.count(id))
//        {
//            known[id] = Hyperedge::create(id, edges, label);
//            // Whenever a parent is created it might be a root
//            roots.insert(id);
//        } else {
//            // Update already existing hyperedge
//            known[id]->_label = label;
//            for (auto edgeIt : edges)
//            {
//                auto edge = edgeIt.second;
//                known[id]->pointTo(edge);
//            }
//        }
//
//        // Update positions
//        spos = npos+1;
//        cpos = from.find(":",spos);
//        opos = from.find("[", spos);
//        npos = from.find("\n",spos);
//    }
//
//    // Now we have to find the root
//    if (roots.size() > 1)
//        throw std::runtime_error("Multiple roots");
//    if (roots.size() < 1)
//        throw std::runtime_error("No root");
//
//    root = known[*(roots.begin())];
//
//    return root;
//}
