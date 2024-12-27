#ifndef __CIRCUIT_MODEL01_HPP__
#define __CIRCUIT_MODEL01_HPP__

#include "standard_defs/standard_defs.hpp"

namespace RecursiveCircuit01 {

template <typename TYPE_ENTRY_NODE_COUNT, typename TYPE_FIRST_ENTRY_NODE_INDEX,
          typename TYPE_EXIT_NODE_COUNT, typename TYPE_FIRST_EXIT_NODE_INDEX>
class TRY_PACKED Cluster {
public:
  TYPE_ENTRY_NODE_COUNT _entry_node_count : 16;
  TYPE_FIRST_ENTRY_NODE_INDEX _first_entry_node_index : 32;
  TYPE_EXIT_NODE_COUNT _exit_node_count : 16;
  TYPE_FIRST_EXIT_NODE_INDEX _first_exit_node_index : 32;

  TRY_NOMAGIC(Cluster)

  TRY_INLINE explicit Cluster(
      const TYPE_ENTRY_NODE_COUNT entry_node_count,
      const TYPE_FIRST_ENTRY_NODE_INDEX first_entry_node_index,
      const TYPE_EXIT_NODE_COUNT exit_node_count,
      const TYPE_FIRST_EXIT_NODE_INDEX first_exit_node_index) {
    _entry_node_count = entry_node_count;
    _first_entry_node_index = first_entry_node_index;
    _exit_node_count = exit_node_count;
    _first_exit_node_index = first_exit_node_index;
  };
};
static_assert(sizeof(Cluster<uint16_t, uint32_t, uint16_t, uint32_t>) == 12);

template <typename TYPE_ENTRY_EDGE_FRONT_COUNT,
          typename TYPE_FIRST_ENTRY_EDGE_FRONT_INDEX,
          typename TYPE_ENTRY_EDGE_REVERSE_COUNT,
          typename TYPE_FIRST_ENTRY_EDGE_REVERSE_INDEX,
          typename TYPE_EXIT_EDGE_REVERSE_INDEX, typename TYPE_CLUSTER_INDEX>
class TRY_PACKED EntryNode {
public:
  TYPE_ENTRY_EDGE_FRONT_COUNT _entry_edge_front_count : 16;
  TYPE_FIRST_ENTRY_EDGE_FRONT_INDEX _first_entry_edge_front_index : 32;
  TYPE_ENTRY_EDGE_REVERSE_COUNT _entry_edge_reverse_count : 16;
  TYPE_FIRST_ENTRY_EDGE_REVERSE_INDEX _first_entry_edge_reverse_index : 32;
  TYPE_EXIT_EDGE_REVERSE_INDEX _exit_edge_reverse_index : 32;
  TYPE_CLUSTER_INDEX _cluster_index : 32;

  TRY_NOMAGIC(EntryNode)

  TRY_INLINE explicit EntryNode(
      const TYPE_ENTRY_EDGE_FRONT_COUNT entry_edge_front_count,
      const TYPE_FIRST_ENTRY_EDGE_FRONT_INDEX first_entry_edge_front_index,
      const TYPE_ENTRY_EDGE_REVERSE_COUNT entry_edge_reverse_count,
      const TYPE_FIRST_ENTRY_EDGE_REVERSE_INDEX first_entry_edge_reverse_index,
      const TYPE_EXIT_EDGE_REVERSE_INDEX exit_edge_reverse_index,
      const TYPE_CLUSTER_INDEX cluster_index) {
    _entry_edge_front_count = entry_edge_front_count;
    _first_entry_edge_front_index = first_entry_edge_front_index;
    _entry_edge_reverse_count = entry_edge_reverse_count;
    _first_entry_edge_reverse_index = first_entry_edge_reverse_index;
    _exit_edge_reverse_index = exit_edge_reverse_index;
    _cluster_index = cluster_index;
  };
};
static_assert(sizeof(EntryNode<uint16_t, uint32_t, uint16_t, uint32_t, uint32_t,
                               uint32_t>) == 20);

template <typename TYPE_ENTRY_EDGE_FRONT_COUNT,
          typename TYPE_FIRST_ENTRY_EDGE_FRONT_INDEX,
          typename TYPE_EXIT_EDGE_FRONT_COUNT,
          typename TYPE_FIRST_EXIT_EDGE_FRONT_INDEX,
          typename TYPE_EXIT_EDGE_REVERSE_INDEX, typename TYPE_CLUSTER_INDEX>
class TRY_PACKED ExitNode {
public:
  TYPE_ENTRY_EDGE_FRONT_COUNT _entry_edge_front_count : 16;
  TYPE_FIRST_ENTRY_EDGE_FRONT_INDEX _first_entry_edge_front_index : 32;
  TYPE_EXIT_EDGE_FRONT_COUNT _exit_edge_front_count : 16;
  TYPE_FIRST_EXIT_EDGE_FRONT_INDEX _first_exit_edge_front_index : 32;
  TYPE_EXIT_EDGE_REVERSE_INDEX _exit_edge_reverse_index : 32;
  TYPE_CLUSTER_INDEX _cluster_index : 32;

  TRY_NOMAGIC(ExitNode)

  TRY_INLINE explicit ExitNode(
      const TYPE_ENTRY_EDGE_FRONT_COUNT entry_edge_front_count,
      const TYPE_FIRST_ENTRY_EDGE_FRONT_INDEX first_entry_edge_front_index,
      const TYPE_EXIT_EDGE_FRONT_COUNT exit_edge_front_count,
      const TYPE_FIRST_EXIT_EDGE_FRONT_INDEX first_exit_edge_front_index,
      const TYPE_EXIT_EDGE_REVERSE_INDEX exit_edge_reverse_index,
      const TYPE_CLUSTER_INDEX cluster_index) {
    _entry_edge_front_count = entry_edge_front_count;
    _first_entry_edge_front_index = first_entry_edge_front_index;
    _exit_edge_front_count = exit_edge_front_count;
    _first_exit_edge_front_index = first_exit_edge_front_index;
    _exit_edge_reverse_index = exit_edge_reverse_index;
    _cluster_index = cluster_index;
  };
};
static_assert(sizeof(ExitNode<uint16_t, uint32_t, uint16_t, uint32_t, uint32_t,
                              uint32_t>) == 20);

template <typename TYPE_ENTRY_NODE_INDEX> class TRY_PACKED EntryEdgeFront {
public:
  TYPE_ENTRY_NODE_INDEX _entry_node_index : 32;

  TRY_NOMAGIC(EntryEdgeFront)

  TRY_INLINE explicit EntryEdgeFront(
      const TYPE_ENTRY_NODE_INDEX entry_node_index) {
    _entry_node_index = entry_node_index;
  };
};
static_assert(sizeof(EntryEdgeFront<uint32_t>) == 4);

template <typename TYPE_ENTRY_NODE_INDEX> class TRY_PACKED EntryEdgeReverse {
public:
  TYPE_ENTRY_NODE_INDEX _entry_node_index : 32;

  TRY_NOMAGIC(EntryEdgeReverse)

  TRY_INLINE explicit EntryEdgeReverse(
      const TYPE_ENTRY_NODE_INDEX entry_node_index) {
    _entry_node_index = entry_node_index;
  };
};
static_assert(sizeof(EntryEdgeReverse<uint32_t>) == 4);

template <typename TYPE_EXIT_NODE_INDEX> class TRY_PACKED ExitEdgeFront {
public:
  TYPE_EXIT_NODE_INDEX _exit_node_index : 32;

  TRY_NOMAGIC(ExitEdgeFront)

  TRY_INLINE explicit ExitEdgeFront(
      const TYPE_EXIT_NODE_INDEX exit_node_index) {
    _exit_node_index = exit_node_index;
  };
};
static_assert(sizeof(ExitEdgeFront<uint32_t>) == 4);

template <typename TYPE_EXIT_NODE_INDEX> class TRY_PACKED ExitEdgeReverse {
public:
  TYPE_EXIT_NODE_INDEX _exit_node_index : 32;

  TRY_NOMAGIC(ExitEdgeReverse)

  TRY_INLINE explicit ExitEdgeReverse(
      const TYPE_EXIT_NODE_INDEX exit_node_index) {
    _exit_node_index = exit_node_index;
  };
};
static_assert(sizeof(ExitEdgeReverse<uint32_t>) == 4);

}; // namespace RecursiveCircuit01

#endif // __CIRCUIT_MODEL01_HPP__
