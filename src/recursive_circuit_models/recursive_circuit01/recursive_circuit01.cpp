#include "recursive_circuit_models/recursive_circuit01/recursive_circuit01.hpp"

namespace RecursiveCircuit01 {
Cluster<uint16_t, uint32_t, uint16_t, uint32_t> *clusters = nullptr;
EntryNode<uint16_t, uint32_t, uint16_t, uint32_t, uint32_t, uint32_t>
    *entry_nodes = nullptr;
ExitNode<uint16_t, uint32_t, uint16_t, uint32_t, uint32_t, uint32_t>
    *exit_nodes = nullptr;
EntryEdgeFront<uint32_t> *entry_edges_front = nullptr;
EntryEdgeReverse<uint32_t> *entry_edges_reverse = nullptr;
ExitEdgeFront<uint32_t> *exit_edges_front = nullptr;
ExitEdgeReverse<uint32_t> *exit_edges_reverse = nullptr;

void allocate_clusters(size_t cluster_count) {
  clusters = static_cast<Cluster<uint16_t, uint32_t, uint16_t, uint32_t> *>(
      malloc(sizeof(Cluster<uint16_t, uint32_t, uint16_t, uint32_t>) *
             cluster_count));
}

void allocate_entry_nodes(size_t entry_node_count) {
  entry_nodes = static_cast<
      EntryNode<uint16_t, uint32_t, uint16_t, uint32_t, uint32_t, uint32_t> *>(
      malloc(sizeof(EntryNode<uint16_t, uint32_t, uint16_t, uint32_t, uint32_t,
                              uint32_t>) *
             entry_node_count));
}

void allocate_exit_nodes(size_t exit_node_count) {
  exit_nodes = static_cast<
      ExitNode<uint16_t, uint32_t, uint16_t, uint32_t, uint32_t, uint32_t> *>(
      malloc(sizeof(ExitNode<uint16_t, uint32_t, uint16_t, uint32_t, uint32_t,
                             uint32_t>) *
             exit_node_count));
}

void allocate_entry_edges_front(size_t entry_edge_front_count) {
  entry_edges_front = static_cast<EntryEdgeFront<uint32_t> *>(
      malloc(sizeof(EntryEdgeFront<uint32_t>) * entry_edge_front_count));
}

void allocate_entry_edges_reverse(size_t entry_edge_reverse_count) {
  entry_edges_reverse = static_cast<EntryEdgeReverse<uint32_t> *>(
      malloc(sizeof(EntryEdgeReverse<uint32_t>) * entry_edge_reverse_count));
}

void allocate_exit_edges_front(size_t exit_edge_front_count) {
  exit_edges_front = static_cast<ExitEdgeFront<uint32_t> *>(
      malloc(sizeof(ExitEdgeFront<uint32_t>) * exit_edge_front_count));
}

void allocate_exit_edges_reverse(size_t exit_edge_reverse_count) {
  exit_edges_reverse = static_cast<ExitEdgeReverse<uint32_t> *>(
      malloc(sizeof(ExitEdgeReverse<uint32_t>) * exit_edge_reverse_count));
}

}; // namespace RecursiveCircuit01
