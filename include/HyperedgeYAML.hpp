#ifndef _HYPEREDGE_YAML_HPP
#define _HYPEREDGE_YAML_HPP

#include "Hyperedge.hpp"
#include "Hypergraph.hpp"
#include "yaml-cpp/yaml.h"
#include <iostream>


/*
   This is an experimental encoder/decoder for hypergraph <-> YAML stuff
   However there are open questions:
   * When we load two systems of hyperedges ... how can we know if two nodes with the same id are the same or not? <- We need an EQUIVALENCE RELATION
     E.G. Given two HGs A, B ... we have to define a functor bool f(a,b) gives true if a == b and false otherwise
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
                for (auto edgeId : rhs->pointingFrom())
                {
                    node["pointingFrom"].push_back(edgeId);
                }
                return node;
            }

            static bool decode(const Node& node, Hyperedge*& rhs) {
                // TODO: It is not possible to decode a Hyperedge without a Hypergraph context
                return false;
            }
        };

    template<>
        struct convert<Hypergraph*> {

            static Node encode(Hypergraph* rhs) {
                Node node;
                for (auto edgeId : rhs->find())
                {
                    auto edge = rhs->get(edgeId);
                    if (edge)
                        node.push_back(edge);
                }   
                return node;
            }

            static bool decode(const Node& node, Hypergraph*& rhs) {
                // Initially, create a new graph :)
                if (!(rhs = new Hypergraph()))
                    return false;

                // First pass: Create nodes
                for (auto it = node.begin(); it != node.end(); it++)
                {
                    Node current = *it;
                    // Get id and label from file
                    UniqueId id = current["id"].as<UniqueId>();
                    std::string label = current["label"].as<std::string>();

                    // Create the edge
                    if (rhs->create(id, label).empty())
                    {
                        // In case a node with the same id exists, we cannot do anything and also not create a new one!!!
                        // This is because the underlying assumption is that of UNIQUE IDs (even between load & stores!)
                        std::cout << "YAML::decode(Hypergraph): " << id << " already used\n";
                        return false;
                    }
                }

                // Second pass: Wire nodes
                for (auto it = node.begin(); it != node.end(); it++)
                {
                    Node current = *it;
                    // Get id and label from file
                    UniqueId id = current["id"].as<UniqueId>();
                    std::string label = current["label"].as<std::string>();

                    // Find the edges we are pointing to
                    if (current["pointingTo"])
                    {
                        // Point from id to otherIds
                        std::vector<UniqueId> otherIds = current["pointingTo"].as< std::vector<UniqueId> >();
                        for (auto otherId : otherIds)
                        {
                            if (rhs->to(Hyperedges{id}, Hyperedges{otherId}).empty())
                            {
                                std::cout << "YAML::decode(Hypergraph): " << id << " -> " << otherId << " failed\n";
                                return false;
                            }
                        }
                    }

                    // Find the edges we are pointing from
                    if (current["pointingFrom"])
                    {
                        // Point from id to otherIds
                        std::vector<UniqueId> otherIds = current["pointingFrom"].as< std::vector<UniqueId> >();
                        for (auto otherId : otherIds)
                        {
                            if (rhs->from(Hyperedges{otherId}, Hyperedges{id}).empty())
                            {
                                std::cout << "YAML::decode(Hypergraph): " << id << " <- " << otherId << " failed\n";
                                return false;
                            }
                        }
                    }
                }
                return true;
            }
        };
}

#endif
