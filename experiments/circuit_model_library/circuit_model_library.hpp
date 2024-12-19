#ifndef __RECURSIVE_GRAPH_NODE_HPP__
#define __RECURSIVE_GRAPH_NODE_HPP__

#include "standard_defs/standard_defs.hpp"

namespace RecursiveGraph {

template <typename TYPE_EDGE_INDEX_OFFSET, typename TYPE_NODE_INDEX>
class TRY_PACKED Edge {
public:
  TYPE_EDGE_INDEX_OFFSET _left_edge_index_offset;
  TYPE_EDGE_INDEX_OFFSET _right_edge_index_offset;
  TYPE_NODE_INDEX _node_index;

  TRY_NOMAGIC(Edge)

  TRY_INLINE explicit Edge(const TYPE_EDGE_INDEX_OFFSET left_edge_index_offset,
                           const TYPE_EDGE_INDEX_OFFSET right_edge_index_offset,
                           const TYPE_NODE_INDEX node_index) {

    _left_edge_index_offset = left_edge_index_offset;
    _right_edge_index_offset = right_edge_index_offset;
    _node_index = node_index;
  }
};

_Static_assert((sizeof(Edge<uint8_t, uint8_t>) == 3));
_Static_assert((sizeof(Edge<uint8_t, uint16_t>) == 4));
_Static_assert((sizeof(Edge<uint8_t, uint32_t>) == 6));
_Static_assert((sizeof(Edge<uint8_t, uint64_t>) == 10));

_Static_assert((sizeof(Edge<uint16_t, uint8_t>) == 5));
_Static_assert((sizeof(Edge<uint16_t, uint16_t>) == 6));
_Static_assert((sizeof(Edge<uint16_t, uint32_t>) == 8));
_Static_assert((sizeof(Edge<uint16_t, uint64_t>) == 12));

_Static_assert((sizeof(Edge<uint32_t, uint8_t>) == 9));
_Static_assert((sizeof(Edge<uint32_t, uint16_t>) == 10));
_Static_assert((sizeof(Edge<uint32_t, uint32_t>) == 12));
_Static_assert((sizeof(Edge<uint32_t, uint64_t>) == 16));

_Static_assert((sizeof(Edge<uint64_t, uint8_t>) == 17));
_Static_assert((sizeof(Edge<uint64_t, uint16_t>) == 18));
_Static_assert((sizeof(Edge<uint64_t, uint32_t>) == 20));
_Static_assert((sizeof(Edge<uint64_t, uint64_t>) == 24));

template <typename TYPE_EDGE_INDEX, typename TYPE_NODE_INDEX>
class TRY_PACKED Node {
public:
  TYPE_EDGE_INDEX _root_edge_index;

  TRY_NOMAGIC(Node)
  TRY_INLINE explicit Node(const TYPE_EDGE_INDEX root_edge_index) {
    _root_edge_index = root_edge_index;
  }
  void connectGraphNodes(const TYPE_NODE_INDEX node1,
                         const TYPE_NODE_INDEX node2);
};

_Static_assert((sizeof(Node<uint8_t, uint8_t>) == 1));
_Static_assert((sizeof(Node<uint8_t, uint16_t>) == 1));
_Static_assert((sizeof(Node<uint8_t, uint32_t>) == 1));
_Static_assert((sizeof(Node<uint8_t, uint64_t>) == 1));

_Static_assert((sizeof(Node<uint16_t, uint8_t>) == 2));
_Static_assert((sizeof(Node<uint16_t, uint16_t>) == 2));
_Static_assert((sizeof(Node<uint16_t, uint32_t>) == 2));
_Static_assert((sizeof(Node<uint16_t, uint64_t>) == 2));

_Static_assert((sizeof(Node<uint32_t, uint8_t>) == 4));
_Static_assert((sizeof(Node<uint32_t, uint16_t>) == 4));
_Static_assert((sizeof(Node<uint32_t, uint32_t>) == 4));
_Static_assert((sizeof(Node<uint32_t, uint64_t>) == 4));

_Static_assert((sizeof(Node<uint64_t, uint8_t>) == 8));
_Static_assert((sizeof(Node<uint64_t, uint16_t>) == 8));
_Static_assert((sizeof(Node<uint64_t, uint32_t>) == 8));
_Static_assert((sizeof(Node<uint64_t, uint64_t>) == 8));

enum class NodeEnum {
  HALF_ADDER,
  FULL_ADDER,
  ADDER_8_BIT,
  MULTIPLIER_8_BIT,
  BIT_SPLIT_8_BIT,
  BIT_MERGE_8_BIT,
};

extern std::vector<char> _names;

extern void *nodes_array;
extern size_t curr_nodes_array_size;
extern size_t curr_nodes_array_capacity;
extern size_t curr_node_size_in_bytes;

extern void *edges_array;
extern size_t curr_edges_array_size;
extern size_t curr_edges_array_capacity;
extern size_t curr_edge_size_in_bytes;

extern std::map<NodeEnum, size_t> _node_index_map;

extern void init(void);
extern uint64_t addNode(void);
extern uint64_t addEdge(const uint64_t start_node, const uint64_t end_node);
extern uint64_t findEdge(const uint64_t start_node, const uint64_t end_node);
extern void deleteEdge(const uint64_t edge);

template <typename TYPE_EDGE_INDEX, typename TYPE_NODE_INDEX,
          TYPE_EDGE_INDEX MAX_EDGE_INDEX>
uint64_t addNodeUnderCapacity(void);

uint64_t addNodeUnderCapacity(void);

template <typename TYPE_EDGE_INDEX, typename TYPE_NODE_INDEX,
          TYPE_EDGE_INDEX MAX_EDGE_INDEX>
uint64_t addNodeAboveCapacity(void *new_nodes_array);

uint64_t addNodeAboveCapacity(void *new_nodes_array);

}; // namespace RecursiveGraph

#endif // __RECURSIVE_GRAPH_NODE_HPP__
