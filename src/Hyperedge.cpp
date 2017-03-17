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

Hyperedge::Hyperedge(Hyperedges members, const std::string& label)
{
    _label = label;
    _id = _lastId++;
    for (auto member : members)
    {
        contains(member.second);
    }
}

// DESTRUCTORS
Hyperedge::~Hyperedge()
{
    // First we detach from all our members AND supers
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

Hyperedge* Hyperedge::create(Hyperedges members, const std::string& label)
{
    Hyperedge* neu = new Hyperedge(members,label);
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
    // Deregister from supers (registered in its members)
    for (auto superIt : _supers)
    {
        auto super = superIt.second;
        if (super->_members.count(_id))
        {
            super->_members.erase(_id);
        }
    }
    // Clear supers
    _supers.clear();
}

void Hyperedge::clear()
{
    // Deregister from members (registered in its supers)
    for (auto memberIt : _members)
    {
        auto member = memberIt.second;
        if (member->_supers.count(_id))
        {
            member->_supers.erase(_id);
        }
    }
    // Clear members
    _members.clear();
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

Hyperedge* Hyperedge::create(const unsigned id, Hyperedges members, const std::string& label)
{
    Hyperedge* neu = NULL;
    if (!_created.count(id))
    {
        // Can do it
        neu = new Hyperedge(members, label);
        neu->_id = id;
        // Ensure monotonic increasing _lastId
        _lastId = (id > _lastId) ? id : _lastId;
    }
    return neu;
}

bool Hyperedge::contains(Hyperedge *member)
{
    if (_members.count(member->_id))
        return true;
        // Check if we are in the super sets of member
        // Make sure we are in the super list
        if (!member->_supers.count(_id))
        {
            member->_supers[_id] = this;
        }
        // ... and the member registered in our member list
        _members[member->_id] = member;
    
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
    return _members.size();
}

Hyperedge::Hyperedges Hyperedge::supers(const std::string& label)
{
    Hyperedges result;
    for (auto edgeIt : _supers)
    {
        auto edge = edgeIt.second;
        // Filters by label if given. It suffices that the edge label contains the given one.
        if (label.empty() || (edge->label().find(label) != std::string::npos))
            result[edge->_id] = edge;
    }
    return result;
}

Hyperedge::Hyperedges Hyperedge::members(const std::string& label)
{
    Hyperedges result;
    for (auto edgeIt : _members)
    {
        auto edge = edgeIt.second;
        // Filters by label if given. It suffices that the edge label contains the given one.
        if (label.empty() || (edge->label().find(label) != std::string::npos))
            result[edge->_id] = edge;
    }
    return result;
}


Hyperedge* Hyperedge::labelContains(const std::string& str, const Hyperedge::TraversalDirection dir)
{
    return Hyperedge::create(_traversal(
        [&](Hyperedge *x){ return (str.empty() || (x->label().find(str) != std::string::npos)) ? true : false; },
        dir),
        "labelContains(" + str + ")"
    );
}

Hyperedge* Hyperedge::labelPartOf(const std::string& str, const Hyperedge::TraversalDirection dir)
{
    return Hyperedge::create(_traversal(
        [&](Hyperedge *x){ return (str.empty() || (str.find(x->label()) != std::string::npos)) ? true : false; },
        dir),
        "labelPartOf(" + str + ")"
    );
}

Hyperedge* Hyperedge::cardinalityLessThanOrEqual(const unsigned cardinality, const Hyperedge::TraversalDirection dir)
{
    std::stringstream ss;
    ss << "cardinalityLessThanOrEqual(" << cardinality << ")";
    return Hyperedge::create(_traversal(
        [&](Hyperedge *x){ return (x->members().size() <= cardinality)? true : false; },
        dir),
        ss.str()
    );
}

Hyperedge* Hyperedge::cardinalityGreaterThan(const unsigned cardinality, const Hyperedge::TraversalDirection dir)
{
    std::stringstream ss;
    ss << "cardinalityGreaterThan(" << cardinality << ")";
    return Hyperedge::create(_traversal(
        [&](Hyperedge *x){ return (x->members().size() > cardinality)? true : false; },
        dir),
        ss.str()
    );
}

Hyperedge* Hyperedge::membersOf()
{
    return Hyperedge::create(_traversal(
        [&](Hyperedge *x){ return true; },
        DOWN),
        "membersOf(" + _label + ")"
    );
}

Hyperedge* Hyperedge::supersOf()
{
    return Hyperedge::create(_traversal(
        [&](Hyperedge *x){ return true; },
        UP),
        "supersOf(" + _label + ")"
    );
}

template <typename Func> Hyperedge* Hyperedge::traversal(Func f, const std::string& label, const Hyperedge::TraversalDirection dir)
{
    return Hyperedge::create(_traversal(f,dir), label);
}

template <typename Func> Hyperedge::Hyperedges Hyperedge::_traversal(Func f, const Hyperedge::TraversalDirection dir)
{
    Hyperedges members;
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
                unknowns.insert(edge->_members.begin(), edge->_members.end());
                break;
            case BOTH:
                unknowns.insert(edge->_members.begin(), edge->_members.end());
            case UP:
                unknowns.insert(edge->_supers.begin(), edge->_supers.end());
                break;
            default:
                members.clear();
                return members;
        }

        if (visited.count(edge))
            continue;

        // Visiting!!!
        visited.insert(edge);
        if (f(edge))
        {
            members[edge->_id] = edge;
        }

        // Inserting members and supers into queue
        for (auto unknownIt : unknowns)
        {
            auto unknown = unknownIt.second;
            edges.push(unknown);
        }
    }

    return members;
}

Hyperedge* Hyperedge::unite(const Hyperedge* other)
{
    // We will create a Hyperedge which will contain
    // x which are part of either this->members() or other->members() or both
    auto members = _members;
    members.insert(other->_members.begin(), other->_members.end());
    return Hyperedge::create(members, this->label() + "||" + other->label());
}

Hyperedge* Hyperedge::intersect(const Hyperedge* other)
{
    // We will create a Hyperedge which will contain
    // x which are part of both this->members() and other->members()
    Hyperedge* result = Hyperedge::create(this->label() + "&&" + other->label());
    for (auto mineIt : _members)
    {
        if (other->_members.count(mineIt.first))
        {
           result->contains(mineIt.second); 
        }
    }
    return result;
}

Hyperedge* Hyperedge::subtract(const Hyperedge* other)
{
    // this - other
    return NULL;
}

Hyperedge* Hyperedge::complement(Hyperedge* other)
{
    // other - this
    return other->subtract(this);
}

std::string Hyperedge::serialize(Hyperedge* root)
{
    std::stringstream result;
    std::set< Hyperedge* > visited;
    std::queue< Hyperedge* > edges;

    edges.push(root);

    // Run through queue of unknown edges
    while (!edges.empty())
    {
        auto edge = edges.front();
        edges.pop();

        if (visited.count(edge))
            continue;

        // Visiting!!!
        visited.insert(edge);
        result << edge->id() << ":" << edge->label();

        // Inserting members into queue
        for (auto unknownIt : edge->members())
        {
            auto unknown = unknownIt.second;
            result << "[" << unknown->id() << "]";
            edges.push(unknown);
        }
        result << "\n";
    }

    return result.str();
}

Hyperedge* Hyperedge::deserialize(const std::string& from)
{
    Hyperedge *root = NULL;
    Hyperedge::Hyperedges known;
    std::set< unsigned > roots;
    
    auto spos = 0;
    auto cpos = from.find(":");
    auto opos = from.find("[");
    auto npos = from.find("\n");

    // Run through string
    while (npos < from.size())
    {
        // Extract id
        auto id = std::stoul(from.substr(spos, cpos-spos));
        // Extract label
        auto label = from.substr(cpos+1, npos-cpos-1);
        if (opos < npos)
            label = from.substr(cpos+1, opos-cpos-1);

        // Go through all members, create them if necessary and update current hyperedge
        Hyperedge::Hyperedges members;
        while (opos < npos)
        {
            auto memberId = std::stoul(from.substr(opos+1, from.find("]",opos+1)-opos-1));
            // Find label in map (and create it iff not found)
            if (!known.count(memberId))
            {
                known[memberId] = Hyperedge::create(memberId,"");
            }
            members[memberId] = known[memberId];
            // Whenever something gets a member, it cannot be a root anymore
            if (roots.count(memberId))
            {
                roots.erase(memberId);
            }
            opos = from.find("[", opos+1);
        }

        // Find id in map, create or update it
        if (!known.count(id))
        {
            known[id] = Hyperedge::create(id, members, label);
            // Whenever a parent is created it might be a root
            roots.insert(id);
        } else {
            // Update already existing hyperedge
            known[id]->_label = label;
            for (auto memberIt : members)
            {
                auto member = memberIt.second;
                known[id]->contains(member);
            }
        }

        // Update positions
        spos = npos+1;
        cpos = from.find(":",spos);
        opos = from.find("[", spos);
        npos = from.find("\n",spos);
    }

    // Now we have to find the root
    if (roots.size() > 1)
        throw std::runtime_error("Multiple roots");
    if (roots.size() < 1)
        throw std::runtime_error("No root");

    root = known[*(roots.begin())];

    return root;
}
