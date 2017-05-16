#ifndef _SET_HPP
#define _SET_HPP

#include "Relation.hpp"

// A set (system) is formed by Hyperedges and a memberOf relation
// A subsumption hierarchy is formed by an isA relation
// A compositional hierarchy is formed by an partOf relation
class Set : public Hyperedge
{
    public:
        typedef std::set<unsigned> Sets;

        ~Set();

        // Factory function
        static Set* create(const std::string& label="Set");
        static Set* create(Sets members, const std::string& label="Set");

        // Write access
        //bool relatedTo(Set *other, const std::string& relation="memberOf"); // generic relation
        // Predefined relations
        bool memberOf(Set *other); // causes a memberOf relation to be created
        bool isA(Set *other);      // causes a isA relation to be created 
        bool partOf(Set *other);   // causes a partOf relation to be created

        // Useful static member functions
        // This function will try to find a Hyperedge with the classLabel or create one!
        static Set* Superclass();
        static Set* promote(Hyperedge *edge);
        static Sets promote(Hyperedge::Hyperedges edges);

        // For derived classes!
        template <typename T> static T* create(const std::string& label=""); // WILL CALL Set::promote<T>
        template <typename T> T* promote();
        template <typename T> bool kindOf();

        // Read access
        // Only the DIRECT members of the set (for transitive sets see members() below)
        Sets members(const std::string& label="") const;

        // Queries:
        // Transitive closures of ...
        //Relation* relatedTo(const std::string& relation="memberOf"); // generic transitive closure
        // Predefined relations
        Relation* memberOf();
        Relation* isA(); //isA
        Relation* partOf();

        // Inverse transitive closures (temporary relations only)
        //Relation* relatedToInverse(const std::string& relation="memberOf"); // generic inverse transitive closure
        // Predefined inverse relations
        Relation* setOf(); // All members AND their members AND ...
        Relation* superclassOf();
        Relation* wholeOf();

        // Merge operations:
        // NOTE: These operations are USING the memberOf relation ONLY!!!
        //       They also use only direct members
        // Unite *this and other (but *this and other are NOT part of unification)
        Set* unite(const Set* other);
        // Intersect *this and other (and *this and other cannot be part of it)
        Set* intersect(const Set* other);
        // Difference between *this - other (and *this and other cannot be part of it)
        Set* subtract(const Set* other);
    private:
        /*Private constructors allow only heap based objects*/
        Set(const std::string& label="Set");
        Set(const Set&);
        Set& operator=(const Set&);

        // The classLabel is a string representing the class of sets
        static const std::string classLabel;
        static unsigned lastSuperclassId;
};

// Include template member functions
// See http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
#include "Set.tpp"

#endif
