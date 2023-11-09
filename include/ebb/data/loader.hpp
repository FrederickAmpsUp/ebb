#ifndef LOADER_HPP
#define LOADER_HPP

#include <ebb/error.hpp>
namespace Ebb {
class Node;

namespace Data {
Node *instantiate_node(int type, Ebb::Node *parent);
int node_type(Node *node);
};
};

#endif