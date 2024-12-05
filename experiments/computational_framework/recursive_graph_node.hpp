#ifndef __RECURSIVE_GRAPH_NODE_HPP__
#define __RECURSIVE_GRAPH_NODE_HPP__

#include "standard_defs/standard_defs.hpp"

class NodeIdentifier {
private:
  uint32_t _number;

public:
  // Block Magic
  NodeIdentifier(void) = delete;
  NodeIdentifier(const NodeIdentifier &) = delete;
  NodeIdentifier(NodeIdentifier &) = delete;
  NodeIdentifier(const NodeIdentifier &&) = delete;
  NodeIdentifier(NodeIdentifier &&) = delete;
  const NodeIdentifier &operator=(const NodeIdentifier &) = delete;
  const NodeIdentifier &operator=(NodeIdentifier &) = delete;
  const NodeIdentifier &operator=(const NodeIdentifier &&) = delete;
  const NodeIdentifier &operator=(NodeIdentifier &&) = delete;
  void *operator new(size_t) = delete;
  void operator delete(void *p) = delete;
  void *operator new[](size_t) = delete;
  void operator delete[](void *p) = delete;

  explicit NodeIdentifier(const uint32_t number) : _number(number) {}
  uint32_t getNumber(void) const { return _number; }
};

class RecursiveGraphNodeEntry {
private:
public:
  // Block Magic
  RecursiveGraphNodeEntry(void) = delete;
  RecursiveGraphNodeEntry(const RecursiveGraphNodeEntry &) = delete;
  RecursiveGraphNodeEntry(RecursiveGraphNodeEntry &) = delete;
  RecursiveGraphNodeEntry(const RecursiveGraphNodeEntry &&) = delete;
  RecursiveGraphNodeEntry(RecursiveGraphNodeEntry &&) = delete;
  const RecursiveGraphNodeEntry &
  operator=(const RecursiveGraphNodeEntry &) = delete;
  const RecursiveGraphNodeEntry &operator=(RecursiveGraphNodeEntry &) = delete;
  const RecursiveGraphNodeEntry &
  operator=(const RecursiveGraphNodeEntry &&) = delete;
  const RecursiveGraphNodeEntry &operator=(RecursiveGraphNodeEntry &&) = delete;
  void *operator new(size_t) = delete;
  void operator delete(void *p) = delete;
  void *operator new[](size_t) = delete;
  void operator delete[](void *p) = delete;
};

class RecursiveGraphNode {
private:
  NodeIdentifier _identifier;
  std::vector<RecursiveGraphNode *> _input_nodes;
  std::vector<RecursiveGraphNode *> _output_nodes;

  struct Connection {
    NodeIdentifier _external_node_identifier;
    NodeIdentifier _internal_subnode_identifier;
  };

  typedef std::set<Connection> AdjacencyList;

  std::set<RecursiveGraphNode *> _internal_graph_nodes;
  AdjacencyList _adj_list;

public:
  // Block Magic
  RecursiveGraphNode(void) = delete;
  RecursiveGraphNode(const RecursiveGraphNode &) = delete;
  RecursiveGraphNode(RecursiveGraphNode &) = delete;
  RecursiveGraphNode(const RecursiveGraphNode &&) = delete;
  RecursiveGraphNode(RecursiveGraphNode &&) = delete;
  const RecursiveGraphNode &operator=(const RecursiveGraphNode &) = delete;
  const RecursiveGraphNode &operator=(RecursiveGraphNode &) = delete;
  const RecursiveGraphNode &operator=(const RecursiveGraphNode &&) = delete;
  const RecursiveGraphNode &operator=(RecursiveGraphNode &&) = delete;
  void *operator new(size_t) = delete;
  void operator delete(void *p) = delete;
  void *operator new[](size_t) = delete;
  void operator delete[](void *p) = delete;

  explicit RecursiveGraphNode(const NodeIdentifier &identifier);

  void addGraphInternalNode(const NodeIdentifier &identifier);

  void connectGraphNodes(NodeIdentifier node1_identifier,
                         NodeIdentifier subnode1_identifier,
                         NodeIdentifier node2_identifier,
                         NodeIdentifier subnode2_identifier);

  RecursiveGraphNode *findGraphInternalNode(NodeIdentifier identifier);

  RecursiveGraphNode *findRecursiveGraphNode(NodeIdentifier identifier);
};

namespace RecursiveGraphNodeNameSpace {
enum class NodeEnum {
  HALF_ADDER,
  FULL_ADDER,
  ADDER_8_BIT,
  MULTIPLIER_8_BIT,
  BIT_SPLIT_8_BIT,
  BIT_MERGE_8_BIT,
};
extern std::vector<char> _names;
extern std::vector<RecursiveGraphNode> _nodes;
extern std::map<NodeEnum, size_t> _node_index_map;
}; // namespace RecursiveGraphNodeNameSpace

#endif // __RECURSIVE_GRAPH_NODE_HPP__
