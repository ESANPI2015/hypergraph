#ifndef _HYPERGRAPH_YAML_HPP
#define _HYPERGRAPH_YAML_HPP

#include "Hyperedge.hpp"
#include "Hypergraph.hpp"
#include "yaml-cpp/yaml.h"
#include <algorithm>
#include <iostream>


/*
   This is an experimental encoder/decoder for hypergraph <-> YAML stuff
   However there are open questions:
   * When we load two systems of hyperedges ... how can we know if two nodes with the same id are the same or not? <- We need an EQUIVALENCE RELATION
     E.G. Given two HGs A, B ... we have to define a functor bool f(a,b) gives true if a == b and false otherwise
*/

namespace YAML {
    // Preserve ordering of keys when storing
    // Taken from https://github.com/l0b0/OpenXcom/blob/01f78550bdfb39f2a862f314d0fba32d683ca0d7/src/Engine/Options.cpp
    void writeNode(const Node& node, Emitter& emitter)
    {
        switch (node.Type())
        {
            case NodeType::Sequence:
            {
                emitter << YAML::BeginSeq;
                for (size_t i = 0; i < node.size(); i++)
                {
                    writeNode(node[i], emitter);
                }
                emitter << YAML::EndSeq;
                break;
            }
            case NodeType::Map:
            {
                emitter << YAML::BeginMap;

                // First collect all the keys
                std::vector<std::string> keys(node.size());
                int key_it = 0;
                for (YAML::const_iterator it = node.begin(); it != node.end(); ++it)
                {
                    keys[key_it++] = it->first.as<std::string>();
                }

                // Then sort them
                std::sort(keys.begin(), keys.end());

                // Then emit all the entries in sorted order.
                for(size_t i = 0; i < keys.size(); i++)
                {
                    emitter << YAML::Key;
                    emitter << keys[i];
                    emitter << YAML::Value;
                    writeNode(node[keys[i]], emitter);
                }
                emitter << YAML::EndMap;
                break;
            }
            default:
                emitter << node;
                break;
        }
    }

    template<>
        struct convert<Hyperedge> {

            static Node encode(const Hyperedge& rhs) {
                Node node;
                node["id"] = rhs.id();
                node["label"] = rhs.label();
                for (auto edgeId : rhs.pointingTo())
                {
                    node["pointingTo"].push_back(edgeId);
                }
                for (auto edgeId : rhs.pointingFrom())
                {
                    node["pointingFrom"].push_back(edgeId);
                }
                return node;
            }

            static bool decode(const Node& node, Hyperedge& rhs) {
                // TODO: Just decode it :)
                return false;
            }
        };

    template<>
        struct convert<Hypergraph> {

            static Node encode(const Hypergraph& rhs) {
                Node node;
                for (auto edgeId : rhs.find())
                {
                    node.push_back(rhs.read(edgeId));
                }   
                return node;
            }

            static bool decode(const Node& node, Hypergraph& rhs) {
                // First pass: Create nodes
                for (auto it = node.begin(); it != node.end(); it++)
                {
                    Node current = *it;
                    // Get id and label from file
                    UniqueId id = current["id"].as<UniqueId>();
                    std::string label = current["label"].as<std::string>();

                    // Create the edge
                    if (rhs.create(id, label).empty())
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
                            if (rhs.to(Hyperedges{id}, Hyperedges{otherId}).empty())
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
                            if (rhs.from(Hyperedges{otherId}, Hyperedges{id}).empty())
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

    std::string StringFrom(const Hypergraph& g)
    {
        Node doc(g);
        Emitter out;
        writeNode(doc, out);
        return out.c_str();
    }
}

#endif
