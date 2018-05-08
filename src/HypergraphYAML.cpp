#include "HypergraphYAML.hpp"

namespace YAML {

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

std::string StringFrom(const Hypergraph& g)
{
    Node doc(g);
    Emitter out;
    writeNode(doc, out);
    return out.c_str();
}

}
