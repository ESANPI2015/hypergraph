#include "Hyperedge.hpp"
#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <queue>
#include <stdexcept>


// STATIC MEMBER INIT
unsigned Hyperedge::_lastId = 1;
std::map<unsigned, Hyperedge*> Hyperedge::_edges;

// PRIVATE CONSTRUCTORS
Hyperedge::Hyperedge(const std::string& label)
: _label(label)
{
}

// DESTRUCTORS
Hyperedge::~Hyperedge()
{
    // First we detach from all our edges AND supers
    detach();

    // Check if we are in the _edges pool. If yes, delete
    if (_edges.count(_id))
    {
        _edges.erase(_id);
    }
}

// PUBLIC FACTORY
Hyperedge* Hyperedge::create(const std::string& label)
{
    Hyperedge* neu = new Hyperedge(label);
    // TODO: Find a better mechanism (maybe assigning random numbers until we found a free id)
    while (Hyperedge::find(_lastId)) _lastId++;
    neu->_id = _lastId;
    _edges[_lastId++] = neu;
    return neu;
}

Hyperedge* Hyperedge::create(Hyperedges edges, const std::string& label)
{
    Hyperedge* neu = Hyperedge::create(label);
    for (auto edgeId : edges)
    {
        neu->pointTo(edgeId);
    }
    return neu;
}

Hyperedge* Hyperedge::find(const unsigned id)
{
    if (_edges.count(id))
    {
        return _edges[id];
    } else {
        return NULL;
    }
}

Hyperedge* Hyperedge::create(const unsigned id, const std::string& label)
{
    Hyperedge* neu = Hyperedge::find(id);
    if (!neu)
    {
        // Create a new hyperedge
        neu = new Hyperedge(label);
        // We have to override the normal mechanism
        neu->_id = id;
        _edges[id] = neu;   // Insert at place with desired id
    } else {
        // Update hyperedge
        neu->_label = label;
    }
    return neu;
}

Hyperedge* Hyperedge::create(const unsigned id, Hyperedges edges, const std::string& label)
{
    Hyperedge* neu = Hyperedge::create(id, label);
    if (neu)
    {
        // Add (possibly new) members
        for (auto edgeId : edges)
        {
            neu->pointTo(edgeId);
        }
    }
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
    for (auto edgeId : _from)
    {
        auto edge = Hyperedge::find(edgeId);
        if (edge && edge->_to.count(_id))
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
    for (auto edgeId : _to)
    {
        auto edge = Hyperedge::find(edgeId);
        if (edge && edge->_from.count(_id))
        {
            edge->_from.erase(_id);
        }
    }
    // Clear edges
    _to.clear();
}

void Hyperedge::cleanup()
{
    // Destroy only those edges which have been created by factory (all!)
    auto localCopy = _edges;

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

    _edges.clear();
}


bool Hyperedge::pointTo(Hyperedge* other)
{
    if (other)
        return this->pointTo(other->id());
    else
        return false;
}

bool Hyperedge::pointTo(const unsigned id)
{
    // Create or find the other edge
    Hyperedge *edge = Hyperedge::find(id);
    if (!edge)
    {
        edge = Hyperedge::create(id); // Create anonymous hyperedge
    }

    // Make sure we are in the from list
    edge->_from.insert(_id);
    // ... and the edge registered in our to list
    _to.insert(edge->id());
    
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

Hyperedge* Hyperedge::pointingTo(const unsigned id)
{
    return _to.count(id) ? Hyperedge::find(id) : NULL;
}

Hyperedge* Hyperedge::pointedBy(const unsigned id)
{
    return _from.count(id) ? Hyperedge::find(id) : NULL;
}

Hyperedge::Hyperedges Hyperedge::pointedBy(const std::string& label) const
{
    Hyperedges result;
    for (auto edgeId : _from)
    {
        auto edge = Hyperedge::find(edgeId);
        // Filters by label if given. It suffices that the edge label contains the given one.
        if (label.empty() || (edge->label().find(label) != std::string::npos))
            result.insert(edge->_id);
    }
    return result;
}

Hyperedge::Hyperedges Hyperedge::pointingTo(const std::string& label) const
{
    Hyperedges result;
    for (auto edgeId : _to)
    {
        auto edge = Hyperedge::find(edgeId);
        // Filters by label if given. It suffices that the edge label contains the given one.
        if (label.empty() || (edge->label().find(label) != std::string::npos))
            result.insert(edge->_id);
    }
    return result;
}


std::ostream& operator<< (std::ostream& stream, const Hyperedge* edge)
{
    stream << edge->id() << ":";
    stream << edge->label() << "[";
    auto otherIds = edge->pointingTo();
    for (auto otherId : otherIds)
    {
        auto other = Hyperedge::find(otherId);
        stream << " " << other->id() << " ";
    }
    stream << "]";
    return stream;
}

std::string Hyperedge::serialize(Hyperedge* root)
{
    std::stringstream result;
    auto trav = root->traversal<Hyperedge>(
        [&](Hyperedge *x){result << x << "\n"; return false;},
        [](Hyperedge *x, Hyperedge *y){return true;},
        "",BOTH);
    delete trav;
    return result.str();
}
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

Hyperedge* Hyperedge::labelContains(const std::string& str)
{
    return Hyperedge::create(_traversal(
        [&](Hyperedge *x){ return (str.empty() || (x->label().find(str) != std::string::npos)) ? true : false; },
        [](Hyperedge *x, Hyperedge *y){return true;},
        BOTH),
        "labelContains(" + str + ")"
    );
}

Hyperedge* Hyperedge::labelPartOf(const std::string& str)
{
    return Hyperedge::create(_traversal(
        [&](Hyperedge *x){ return (str.empty() || (str.find(x->label()) != std::string::npos)) ? true : false; },
        [](Hyperedge *x, Hyperedge *y){return true;},
        BOTH),
        "labelPartOf(" + str + ")"
    );
}

Hyperedge* Hyperedge::cardinalityLessThanOrEqual(const unsigned cardinality)
{
    std::stringstream ss;
    ss << "cardinalityLessThanOrEqual(" << cardinality << ")";
    return Hyperedge::create(_traversal(
        [&](Hyperedge *x){ return (x->cardinality() <= cardinality)? true : false; },
        [](Hyperedge *x, Hyperedge *y){return true;},
        BOTH),
        ss.str()
    );
}

Hyperedge* Hyperedge::cardinalityGreaterThan(const unsigned cardinality)
{
    std::stringstream ss;
    ss << "cardinalityGreaterThan(" << cardinality << ")";
    return Hyperedge::create(_traversal(
        [&](Hyperedge *x){ return (x->cardinality() > cardinality)? true : false; },
        [](Hyperedge *x, Hyperedge *y){return true;},
        BOTH),
        ss.str()
    );
}

Hyperedge* Hyperedge::successors()
{
    return Hyperedge::create(_traversal(
        [&](Hyperedge *x){ return x->id() != id() ? true : false; },
        [](Hyperedge *x, Hyperedge *y){return true;},
        DOWN),
        "successors(" + _label + ")"
    );
}

Hyperedge* Hyperedge::predecessors()
{
    return Hyperedge::create(_traversal(
        [&](Hyperedge *x){ return x->id() != id() ? true : false; },
        [](Hyperedge *x, Hyperedge *y){return true;},
        UP),
        "predecessors(" + _label + ")"
    );
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
    for (auto mineId : _to)
    {
        if (other->_to.count(mineId))
        {
           result->pointTo(mineId); 
        }
    }
    return result;
}

Hyperedge* Hyperedge::subtract(const Hyperedge* other)
{
    // this - other
    // only those x which are part of this->edges() but not part of other->edges()
    Hyperedge* result = Hyperedge::create(this->label() + "/" + other->label());
    for (auto mineId : _to)
    {
        if (!other->_to.count(mineId))
        {
           result->pointTo(mineId); 
        }
    }
    return result;
}

