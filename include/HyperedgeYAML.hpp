#ifndef _HYPEREDGE_YAML_HPP
#define _HYPEREDGE_YAML_HPP

#include "Set.hpp"
#include "yaml-cpp/yaml.h"
//#include <iostream>


/*
   This is an experimental encoder/decoder for hyperedge <-> YAML stuff
   However there are open questions:
   * Currently we not only store the hyperedge we want, but also the edges it points to and so forth.
     Can we just store a hyperedge on its own?
   * We have not yet found a good way to ensure persistent ids ... when we import an hyperedge and the things it points to do not exist, what do we do?
   * When we store a system of hyperedges, how can we make sure, that it is correctly reconstructed?
*/

namespace YAML {
    template<>
        struct convert<Hyperedge*> {

            static Node encode(const Hyperedge* rhs) {
                Node node;
                node["id"] = rhs->id();
                node["label"] = rhs->label();
                for (auto edgeId : rhs->pointingTo())
                {
                    node["pointingTo"].push_back(edgeId);
                }
                return node;
            }

            static bool decode(const Node& node, Hyperedge*& rhs) {
                unsigned id = node["id"].as<unsigned>();
                std::string label = node["label"].as<std::string>();
                // Check if such an edge already exists
                // if not, create it with the correct id!!! If not possible, return false!
                //std::cout << "CREATE " << label << std::endl;
                if (!(rhs = Hyperedge::create(id, label))) // Create or get
                {
                    return false;
                }
                //std::cout << "CHECK ID\n";
                // Check id
                if (rhs->id() != id)
                {
                    return false;
                }
                //std::cout << "CHECK TO SET\n";
                // Find the edges we are pointing to
                if (!node["pointingTo"])
                {
                    // We do not point to anything so, we skip
                    return true;
                }
                //std::cout << "INSERT TO SET\n";
                std::vector<unsigned> otherIds = node["pointingTo"].as< std::vector<unsigned> >();
                for (auto otherId : otherIds)
                {
                    //std::cout << "INSERT " << otherId << std::endl;
                    rhs->pointTo(otherId);
                }
                return true;
            }
        };

    // Stores a system of hyperedges to a YAML node
    static Node store(const Hyperedge::Hyperedges& edges)
    {
        Node node;
        for (auto edgeId : edges)
        {
            node.push_back(Hyperedge::find(edgeId));
        }   
        return node;
    }

    // Creates a complete system of Hyperedges from a YAML node
    static Hyperedge::Hyperedges load(const Node& node)
    {
        Hyperedge::Hyperedges result;
        // Node must be a sequence!
        for (auto it = node.begin(); it != node.end(); it++)
        {
            Hyperedge *neu = it->as<Hyperedge*>();
            result.insert(neu->id());
        }
        return result;
    }
}

#endif
