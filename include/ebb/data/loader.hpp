#ifndef LOADER_HPP
#define LOADER_HPP


namespace Ebb {
class Node;

namespace Data {
Node *instantiate_node(int type, Ebb::Node *parent);
};
};

#endif