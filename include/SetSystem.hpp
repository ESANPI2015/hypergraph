#ifndef _SETSYSTEM_HPP
#define _SETSYSTEM_HPP

#include "Hypergraph.hpp"

class Set;
class Relation;

/*
    This class creates a system of sets based on a hypergraph.
    To define a set we need at least to relations:
    a) X -- isA --> SET
    b) x -- memberOf --> X

    The big question is:
    DO WE NEED A SET AND A RELATION CLASS?
    Because if a hyperedge A is connected by a hyperedge labelled "isA" to a hyperedge labelled "SET", we can safely assume that A isA SET :)

*/

class SetSystem : public Hypergraph
{
    public:
        typedef std::set<unsigned> Sets;

        SetSystem();                                    // will create a basic set as superclass for all created sets
        ~SetSystem();

        unsigned getSetClass();                         // returns an id which serves as representative of the set concept

        /*For derived classes based on sets*/
        unsigned getClass(const std::string& label);    // returns an id which serves as representative of a class with the given label (DERIVED CLASSES)
        // unsigned instantiateFrom(const std::string& classLabel, const std::string& label=""); // implies (new -- isA --> <classLabel>)

        /*Factory functions*/
        unsigned create(const std::string& label="");   // creates a new set
        unsigned create(Sets members, const std::string& label="");
        bool     create(const unsigned id, 
                        const std::string& label="");   // Tries to create a set with a given id ... if already taken, returns false
        void     destroy(const unsigned id);            // Will remove a set from this set system. Will cause all relations to be destroyed as well!

        /*Get access to sets*/
        bool isSet(const unsigned id);                  // Checks if id -- isA --> superclassId after transitive closure
        Set* get(const unsigned id);                    // Finds a set by id
        Sets find(const std::string& label="");         // Finds all sets with a certain label
        Sets members(const unsigned id);                // Returns the members of a set

        /*Relate sets*/
        unsigned relateTo(const unsigned idA, const unsigned idB, const std::string& relation="relatedTo"); // generic relation "A -- <label> --> B"
        // TODO: Predefined relations?
        // unsigned isA(const unsigned id, const unsigned typeId);

        /* Query directly related sets (id -- relation --> *) */
        Sets directlyRelatedTo(const unsigned id, const std::string& relation="relatedTo");

        // Merge operations:
        // NOTE: These operations are USING the memberOf relation ONLY!!!
        //       They also use only direct members
        // Unites the to sets of A and B creating a new set C
        // TODO: Implement these!
        unsigned unite(const unsigned idA, const unsigned idB);
        // Intersect the to sets of A and B creating a new set C
        unsigned intersect(const unsigned idA, const unsigned idB);
        // Create an set C which contains all sets A points to but B does not point to
        unsigned subtract(const unsigned idA, const unsigned idB);

        // Queries:
        // NOTE: These make a graph traversal and give back a new relation holding the results of the closure
        // NOTE2: Although transitive closures exist, it does not mean that they are useful.
        //        E.g. If A hasA B && B hasA C !=> A hasA C !!!!!
        // Transitive closures of ...
        unsigned relatedTo(const unsigned id, const std::string& relation="relatedTo"); // generic transitive closure of "A -- <label> --> * -- <label> --> * ..."
        // Predefined relations
        unsigned memberOf(const unsigned id);
        unsigned isA(const unsigned id);
        //unsigned isA(const unsigned id, const std::string=""); // also checks for a concrete label :)

        // Inverse transitive closures of
        unsigned relatedToInverse(const unsigned id, const std::string& relation="relatedTo", const std::string& inverse="relatedTo"); // generic inverse transitive closure of "A <label> B"
        // ... predefined relations
        unsigned setOf(const unsigned id);
        unsigned superclassOf(const unsigned id);
};

#endif
