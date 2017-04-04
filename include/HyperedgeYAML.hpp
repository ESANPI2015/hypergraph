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
                unsigned id = node[0]["id"].as<unsigned>();
                std::string label = node[0]["label"].as<std::string>();
                // Check if such an edge already exists
                if (!(rhs = Hyperedge::find(id)))
                {
                    // FIXME: Create it with the correct id!!! If not possible, return false!
                    rhs = Hyperedge::create(label);
                }
                // Check id
                if (rhs->id() != id)
                {
                    return false;
                }
                // Find the edges we are pointing to
                std::vector<unsigned> ids = node[0]["pointingTo"].as< std::vector<unsigned> >();
                for (auto id : ids)
                {
                    auto other = Hyperedge::find(id);
                    if (!other)
                        return false;
                    rhs->pointTo(other);
                }
                return true;
            }

            // FIXME: Use proper traversal to do this!!!
            //static Node _encode(Node& node, const Hyperedge* rhs) {
            //  node[rhs->id()]["label"] = rhs->label();
            //  for (auto edgeIt : rhs->pointingTo())
            //  {
            //      node[rhs->id()]["pointingTo"].push_back(edgeIt.second->id());
            //      convert<Hyperedge*>::_encode(node, edgeIt.second);
            //  }
            //  return node;
            //}
            //static Node encode(const Hyperedge* rhs) {
            //  Node node;
            //  
            //  convert<Hyperedge*>::_encode(node, rhs);
            //  
            //  return node;
            //}

            //static bool decode(const Node& node, Hyperedge*& rhs) {
                // SEQ -> MAP -> id
                //         |---> MAP -> "label"
                //                |---> "pointingTo"
                //switch (second.Type()) {
                //    case NodeType::Null: // ...
                //        std::cout << "NULL\n";
                //        break;
                //    case NodeType::Scalar: // ...
                //        std::cout << "SCALAR\n";
                //        break;
                //    case NodeType::Sequence: // ...
                //        std::cout << "SEQ\n";
                //        break;
                //    case NodeType::Map: // ...
                //        std::cout << "MAP\n";
                //        break;
                //    case NodeType::Undefined: // ...
                //        std::cout << "?\n";
                //        break;
                //}


                // We need two passes here:
                //Hyperedge *result = NULL;
                //// I: Register (id,PTR) && (id,id')
                //std::map<unsigned, Hyperedge*> edges;

                //{
                //    auto seqIt = node.begin();
                //    for (;seqIt != node.end(); seqIt++)
                //    {
                //        auto mapIt = seqIt->begin();
                //        for (;mapIt != seqIt->end(); mapIt++)
                //        {
                //            unsigned id = mapIt->first.as<unsigned>();
                //            std::string label = mapIt->second["label"].as<std::string>();
                //            edges[id] = Set::create(label);
                //            if (!result)
                //                result = edges[id];
                //        }
                //    }
                //}

                //rhs = result;

                //// II: Register (id -> id,id, ...,id) and use (id,id') to create correct pointing
                //{
                //    auto seqIt = node.begin();
                //    for (;seqIt != node.end(); seqIt++)
                //    {
                //        auto mapIt = seqIt->begin();
                //        for (;mapIt != seqIt->end(); mapIt++)
                //        {
                //            unsigned oldId = mapIt->first.as<unsigned>();
                //            if (!mapIt->second["pointingTo"])
                //                continue;
                //            std::vector<unsigned> othersOldIds = mapIt->second["pointingTo"].as< std::vector<unsigned> >();
                //            for (auto otherOldId : othersOldIds)
                //            {
                //                edges[oldId]->pointTo(edges[otherOldId]);
                //            }
                //        }
                //    }
                //}

                //return true;
            //}
        };
}

#endif
