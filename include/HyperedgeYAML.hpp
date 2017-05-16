#ifndef _HYPEREDGE_YAML_HPP
#define _HYPEREDGE_YAML_HPP

#include "Set.hpp"
#include "yaml-cpp/yaml.h"
#include <iostream>


/*
   This is an experimental encoder/decoder for hyperedge <-> YAML stuff
   However there are open questions:
   * When we load two systems of hyperedges ... how can we know if two nodes with the same id are the same or not?
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
                
                // Check if an edge with that index already exists
                rhs = Hyperedge::find(id);
                if (rhs)
                {
                    // Existing node: Check if label and _to set are the same
                    if (label != rhs->label())
                    {
                        std::cout << "DEC: Label mismatch\n";
                        return false;
                    }
                    // Find the edges we are pointing to
                    if (!node["pointingTo"])
                    {
                        // We do not point to anything so, we skip
                        return true;
                    }
                    // Check if we are pointing to the right edges
                    std::vector<unsigned> otherIds = node["pointingTo"].as< std::vector<unsigned> >();
                    for (auto otherId : otherIds)
                    {
                        if (!rhs->pointingTo(otherId))
                        {
                            std::cout << "DEC: Edge does not point to the same nodes\n";
                            return false;
                        }
                    }
                    return true;
                } else {
                    // Non-existing node: Create and connect it
                    rhs = Hyperedge::create(id,label);
                    // Find the edges we are pointing to
                    if (!node["pointingTo"])
                    {
                        // We do not point to anything so, we skip
                        return true;
                    }
                    // Add the edges to our _to set
                    std::vector<unsigned> otherIds = node["pointingTo"].as< std::vector<unsigned> >();
                    for (auto otherId : otherIds)
                    {
                        if (!rhs->pointTo(otherId))
                        {
                            std::cout << "DEC: Could not find other edge\n";
                            return false;
                        }
                    }
                    return true;
                }
            }
        };

    // Stores a system of hyperedges to a YAML node
    static Node store(const Hyperedge::Hyperedges& edges)
    {
        Node node;
        for (auto edgeId : edges)
        {
            auto edge = Hyperedge::find(edgeId);
            if (edge)
                node.push_back(edge);
        }   
        return node;
    }

    // This will only store connected systems (no forests)
    static Node store(Hyperedge* root)
    {
        auto query = root->labelContains();
        auto node = store(query->pointingTo());
        delete query;
        return node;
    }

    // Creates a complete system of Hyperedges from a YAML node
    // ATTENTION: This will reindex all nodes!!!
    static Hyperedge::Hyperedges load(const Node& node)
    {
        Hyperedge::Hyperedges result;
        std::map<unsigned, unsigned> old2new;

        // First pass: Create nodes
        for (auto it = node.begin(); it != node.end(); it++)
        {
            Node current = *it;
            // Get id and label from file
            unsigned id = current["id"].as<unsigned>();
            std::string label = current["label"].as<std::string>();

            // Try to find that edge
            auto edge = Hyperedge::find(id);
            if (!edge)
            {
                // Does not exist, so create it :)
                edge = Hyperedge::create(id, label);
            } else {
                // Does exist, what will we do? Create a new one!
                // NOTE: We cannot check for label match only. We would have to check the pointingTo sets as well to check for equality...
                edge = Hyperedge::create(label);
            }
            old2new[id] = edge->id();

            result.insert(edge->id());
        }

        // Second pass: Wire nodes
        for (auto it = node.begin(); it != node.end(); it++)
        {
            Node current = *it;
            // Get id and label from file
            unsigned id = current["id"].as<unsigned>();
            std::string label = current["label"].as<std::string>();

            // Find the edges we are pointing to
            if (!current["pointingTo"])
            {
                // We do not point to anything so, we skip
                continue;
            }

            // Add the edges to our _to set
            auto us = Hyperedge::find(old2new[id]);
            std::vector<unsigned> otherIds = current["pointingTo"].as< std::vector<unsigned> >();
            for (auto otherId : otherIds)
            {
                us->pointTo(old2new[otherId]);
            }
        }
        return result;
    }
}

#endif
