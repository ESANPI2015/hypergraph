#include "Conceptgraph.hpp"
#include "HypergraphYAML.hpp"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <fstream>
#include <iostream>
#include <cassert>

int main(void)
{
    std::cout << "*** CONCEPT GRAPH TEST ***" << std::endl;

    Conceptgraph universe;

    std::cout << "> Store empty concept graph using YAML" << std::endl;

    std::ofstream fout;
    fout.open("emptyCG.yml");
    if(fout.good()) {
        fout << YAML::StringFrom(universe) << std::endl;
    } else {
        std::cout << "FAILED\n";
    }
    fout.close();

    std::cout << "> Create concept" << std::endl;
    universe.create("3", "First concept");

    std::cout << "> All concepts" << std::endl;
    auto concepts = universe.find();
    for (auto conceptId : concepts)
    {
        std::cout << conceptId << " " << universe.access(conceptId).label() << std::endl;
    }

    std::cout << "> Create another concept and check it" << std::endl;
    universe.create("4", "Second concept");
    concepts = universe.find("Second concept");
    assert(concepts.size() > 0);

    std::cout << "> Relate the first and the second concept\n";
    universe.relate("5", Hyperedges{"3"}, Hyperedges{"4"}, "relatedTo");

    std::cout << "> Create a tree of concepts related by a common relation\n";
    universe.create("6", "Root");
    universe.create("11","I");
    universe.create("12","You");
    universe.create("13","It"); 
    universe.create("14","Huh?");
    universe.create("15", "Plural");
    universe.create("20", "We"); 
    universe.create("21", "You");
    universe.create("22", "They");
    auto hashedId = universe.create("Doh?");

    universe.relate("7",  Hyperedges{"6"}, Hyperedges{"11"}, "R");
    universe.relate("8",  Hyperedges{"6"}, Hyperedges{"12"}, "R");
    universe.relate("9",  Hyperedges{"6"}, Hyperedges{"13"}, "R");
    universe.relate("10", Hyperedges{"6"}, Hyperedges{"14"}, "A");
    universe.relate("16", Hyperedges{"15"}, Hyperedges{"20"}, "R");
    universe.relate("17", Hyperedges{"15"}, Hyperedges{"21"}, "R");
    universe.relate("18", Hyperedges{"15"}, Hyperedges{"22"}, "R");
    universe.relate("19", Hyperedges{"15"}, hashedId, "B");
    universe.relate("24", Hyperedges{"6"}, Hyperedges{"15"}, "R");

    std::cout << "> All concepts" << std::endl;
    concepts = universe.find();
    for (auto conceptId : concepts)
    {
        std::cout << conceptId << " " << universe.access(conceptId).label() << std::endl;
    }

    std::cout << "> Store concept graph using YAML" << std::endl;

    fout.open("universe.yml");
    if(fout.good()) {
        fout << YAML::StringFrom(universe) << std::endl;
    } else {
        std::cout << "FAILED\n";
    }
    fout.close();

    std::cout << "> Create new hypergraph from YAML" << std::endl;
    Hypergraph restoredGraph(YAML::LoadFile("universe.yml").as<Hypergraph>());

    std::cout << "> All edges of restored graph" << std::endl;
    auto edges = restoredGraph.findByLabel();
    for (auto edgeId : edges)
    {
        std::cout << restoredGraph.access(edgeId) << std::endl;
    }

    std::cout << "> Make it a concept graph" << std::endl;
    Conceptgraph universe2(restoredGraph);

    std::cout << "> All concepts" << std::endl;
    concepts = universe2.find();
    for (auto conceptId : concepts)
    {
        std::cout << universe2.access(conceptId) << std::endl;
        auto relations = universe2.relationsOf(Hyperedges{conceptId});
        for (auto relId : relations)
        {
            std::cout << "\t" << universe2.access(relId) << std::endl;
        }
    }

    std::cout << "> Make a traversal returning concepts connected by a certain relation\n";
    auto cf = [](const Hyperedge& c) -> bool { return true; };
    auto rf = [](const Hyperedge& c, const Hyperedge& r) -> bool {
        // For simplicity we just check the label
        if (r.label() == "R")
            return true;
        return false;
    };
    concepts = universe2.traverse(*universe2.find("Root").begin(), cf, rf);
    for (auto conceptId : concepts)
    {
        std::cout << conceptId << " " << universe2.access(conceptId).label() << std::endl;
        auto relations = universe2.relationsOf(Hyperedges{conceptId});
        for (auto relId : relations)
        {
            std::cout << "\t" << relId << " " << universe2.access(relId).label() << std::endl;
        }
    }

    std::cout << "> Create another concept graph for inexact pattern matching\n";
    Conceptgraph query;
    query.relate("A", query.create("Root"), query.create("*",""), "A");
    concepts = query.Hypergraph::findByLabel();
    for (auto conceptId : concepts)
    {
        std::cout << "\t" << query.access(conceptId) << std::endl;
    }

    fout.open("query.yml");
    if(fout.good()) {
        fout << YAML::StringFrom(query) << std::endl;
    } else {
        std::cout << "FAILED\n";
    }
    fout.close();

    std::cout << "> Try to find a match of the query graph in the data graph\n";
    std::stack< Mapping > searchSpace;
    Mapping mapping = universe2.match(query, searchSpace, Hypergraph::defaultMatchFunc);
    for (auto it : mapping)
    {
        std::cout << "\t" << query.access(it.first) << " -> " << universe2.access(it.second) << std::endl;
    }

    std::cout << "> Create another concept graph which serves as a replacement for the matched subgraph\n";
    Conceptgraph replacement;
    replacement.relate("A^-1", replacement.create("**",""), replacement.create("Root"), "A^-1");
    concepts = replacement.Hypergraph::findByLabel();
    for (auto conceptId : concepts)
    {
        std::cout << "\t" << replacement.access(conceptId) << std::endl;
    }
    fout.open("replacement.yml");
    if(fout.good()) {
        fout << YAML::StringFrom(replacement) << std::endl;
    } else {
        std::cout << "FAILED\n";
    }
    fout.close();

    std::cout << "> Define the mapping between query and replacement\n";
    Mapping repl(fromHyperedges(query.Hypergraph::findByLabel()));
    repl.erase("*");
    repl.erase("A");
    repl.insert({"*", "**"});
    repl.insert({"A", "A^-1"});
    for (auto it : repl)
    {
        std::cout << "\t" << query.access(it.first) << " -> " << replacement.access(it.second) << std::endl;
    }

    std::cout << "> Rewrite (using previous search space)\n";
    searchSpace.push(mapping);
    Hypergraph rewritten = universe2.rewrite(query, replacement, repl, searchSpace, Hypergraph::defaultMatchFunc);
    std::cout << rewritten.findByLabel() << std::endl;

    std::cout << "> All edges of rewritten graph" << std::endl;
    edges = rewritten.findByLabel();
    for (auto edgeId : edges)
    {
        std::cout << rewritten.access(edgeId) << std::endl;
    }

    std::cout << "> All relations" << std::endl;
    Conceptgraph fin(rewritten);
    Hyperedges allRels(fin.relations());
    for (UniqueId relId : allRels)
    {
        std::cout << fin.access(relId) << std::endl;
    }

    std::cout << "> All concepts" << std::endl;
    concepts = fin.find();
    for (auto conceptId : concepts)
    {
        std::cout << fin.access(conceptId) << std::endl;
        auto relations = fin.relationsOf(Hyperedges{conceptId});
        for (auto relId : relations)
        {
            std::cout << "\t" << fin.access(relId) << std::endl;
        }
    }

    std::cout << "*** TESTS DONE ***" << std::endl;

    return 0;
}
