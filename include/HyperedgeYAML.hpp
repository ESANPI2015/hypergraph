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
                for (auto edgeIt : rhs->pointingTo())
                {
                    node["pointingTo"].push_back(edgeIt.second->id());
                }
                return node;
            }

            static bool decode(const Node& node, Hyperedge*& rhs) {
                unsigned id = node["id"].as<unsigned>();
                std::string label = node["label"].as<std::string>();
                // Check if such an edge already exists
                // if not, create it with the correct id!!! If not possible, return false!
                if (!(rhs = Hyperedge::find(id)) && !(rhs = Hyperedge::create(id, label)))
                {
                    return false;
                }
                // Check id
                if (rhs->id() != id)
                {
                    return false;
                }
                // Find the edges we are pointing to
                std::vector<unsigned> ids = node["pointingTo"].as< std::vector<unsigned> >();
                for (auto id : ids)
                {
                    auto other = Hyperedge::find(id);
                    if (!other)
                        return false;
                    rhs->pointTo(other);
                }
                return true;
            }
        };


}

#endif
