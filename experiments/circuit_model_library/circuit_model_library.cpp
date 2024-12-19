#include "circuit_model_library.hpp"
#include <cstdint>
#include <cstring>

void *operator new(std::uint64_t size) { assert(0); }
void operator delete(void *ptr) throw() { assert(0); }
void *operator new[](std::uint64_t size) { assert(0); }
void operator delete[](void *ptr) throw() { assert(0); }

void *nodes_array = nullptr;
void *edges_array = nullptr;

void RecursiveGraph::init(void) {
  curr_nodes_array_size = 0;
  curr_node_size_in_bytes = sizeof(uint8_t);
  curr_nodes_array_capacity = UINT8_MAX;

  nodes_array = aligned_malloc_multiple_of_alignment(
      1llu + curr_nodes_array_capacity, 32);

  Node<uint8_t, uint8_t> *nodes_array_ptr =
      static_cast<Node<uint8_t, uint8_t> *>(nodes_array);

  nodes_array_ptr[UINT8_MAX]._root_edge_index = UINT8_MAX;

  curr_edges_array_size = 0;
  curr_edge_size_in_bytes = sizeof(uint8_t);
  curr_edges_array_capacity = UINT8_MAX;

  edges_array = aligned_malloc_multiple_of_alignment(
      1llu + curr_edges_array_capacity, 32);

  Edge<uint8_t, uint8_t> *edges_array_ptr =
      static_cast<Edge<uint8_t, uint8_t> *>(edges_array);

  edges_array_ptr[UINT8_MAX]._left_edge_index_offset = UINT8_MAX;
  edges_array_ptr[UINT8_MAX]._right_edge_index_offset = UINT8_MAX;
  edges_array_ptr[UINT8_MAX]._node_index = UINT8_MAX;
}

template <typename TYPE_EDGE_INDEX, typename TYPE_NODE_INDEX,
          TYPE_EDGE_INDEX MAX_EDGE_INDEX>
uint64_t RecursiveGraph::addNodeUnderCapacity(void) {
  Node<TYPE_EDGE_INDEX, TYPE_NODE_INDEX> *nodes_array_ptr =
      static_cast<Node<TYPE_EDGE_INDEX, TYPE_NODE_INDEX> *>(nodes_array);

  ::new (&nodes_array_ptr[curr_nodes_array_size++])
      Node<TYPE_EDGE_INDEX, TYPE_NODE_INDEX>(MAX_EDGE_INDEX);

  return curr_nodes_array_size;
}

uint64_t RecursiveGraph::addNodeUnderCapacity(void) {
  switch (curr_edge_size_in_bytes) {
  case 1: {
    switch (curr_node_size_in_bytes) {
    case 1: {
      return addNodeUnderCapacity<uint8_t, uint8_t, UINT8_MAX>();
    }
    case 2: {
      return addNodeUnderCapacity<uint8_t, uint16_t, UINT8_MAX>();
    }
    case 4: {
      return addNodeUnderCapacity<uint8_t, uint32_t, UINT8_MAX>();
    }
    case 8: {
      return addNodeUnderCapacity<uint8_t, uint64_t, UINT8_MAX>();
    }
    default:
      assert(0);
      return 0;
    }
  }
  case 2: {
    switch (curr_node_size_in_bytes) {
    case 1: {
      return addNodeUnderCapacity<uint16_t, uint8_t, UINT16_MAX>();
    }
    case 2: {
      return addNodeUnderCapacity<uint16_t, uint16_t, UINT16_MAX>();
    }
    case 4: {
      return addNodeUnderCapacity<uint16_t, uint32_t, UINT16_MAX>();
    }
    case 8: {
      return addNodeUnderCapacity<uint16_t, uint64_t, UINT16_MAX>();
    }
    default:
      assert(0);
      return 0;
    }
  }
  case 4: {
    switch (curr_node_size_in_bytes) {
    case 1: {
      return addNodeUnderCapacity<uint32_t, uint8_t, UINT32_MAX>();
    }
    case 2: {
      return addNodeUnderCapacity<uint32_t, uint16_t, UINT32_MAX>();
    }
    case 4: {
      return addNodeUnderCapacity<uint32_t, uint32_t, UINT32_MAX>();
    }
    case 8: {
      return addNodeUnderCapacity<uint32_t, uint64_t, UINT32_MAX>();
    }
    default:
      assert(0);
      return 0;
    }
  }
  case 8: {
    switch (curr_node_size_in_bytes) {
    case 1: {
      return addNodeUnderCapacity<uint64_t, uint8_t, UINT64_MAX>();
    }
    case 2: {
      return addNodeUnderCapacity<uint64_t, uint16_t, UINT64_MAX>();
    }
    case 4: {
      return addNodeUnderCapacity<uint64_t, uint32_t, UINT64_MAX>();
    }
    case 8: {
      return addNodeUnderCapacity<uint64_t, uint64_t, UINT64_MAX>();
    }
    default:
      assert(0);
      return 0;
    }
  }
  default:
    assert(0);
    return 0;
  }
}

template <typename TYPE_EDGE_INDEX, typename TYPE_NODE_INDEX,
          TYPE_EDGE_INDEX MAX_EDGE_INDEX>
uint64_t RecursiveGraph::addNodeAboveCapacity(void *new_nodes_array) {
  Node<TYPE_EDGE_INDEX, TYPE_NODE_INDEX> *nodes_array_ptr =
      static_cast<Node<TYPE_EDGE_INDEX, TYPE_NODE_INDEX> *>(nodes_array);

  ::new (&nodes_array_ptr[curr_nodes_array_size++])
      Node<TYPE_EDGE_INDEX, TYPE_NODE_INDEX>(MAX_EDGE_INDEX);

  return curr_nodes_array_size;
}

uint64_t RecursiveGraph::addNodeAboveCapacity(void *new_nodes_array) {
  switch (curr_edge_size_in_bytes) {
  case 1: {
    switch (curr_node_size_in_bytes) {
    case 1: {
      return addNodeUnderCapacity<uint8_t, uint8_t, UINT8_MAX>();
    }
    case 2: {
      return addNodeUnderCapacity<uint8_t, uint16_t, UINT8_MAX>();
    }
    case 4: {
      return addNodeUnderCapacity<uint8_t, uint32_t, UINT8_MAX>();
    }
    case 8: {
      return addNodeUnderCapacity<uint8_t, uint64_t, UINT8_MAX>();
    }
    default:
      assert(0);
      return 0;
    }
  }
  case 2: {
    switch (curr_node_size_in_bytes) {
    case 1: {
      return addNodeUnderCapacity<uint16_t, uint8_t, UINT16_MAX>();
    }
    case 2: {
      return addNodeUnderCapacity<uint16_t, uint16_t, UINT16_MAX>();
    }
    case 4: {
      return addNodeUnderCapacity<uint16_t, uint32_t, UINT16_MAX>();
    }
    case 8: {
      return addNodeUnderCapacity<uint16_t, uint64_t, UINT16_MAX>();
    }
    default:
      assert(0);
      return 0;
    }
  }
  case 4: {
    switch (curr_node_size_in_bytes) {
    case 1: {
      return addNodeUnderCapacity<uint32_t, uint8_t, UINT32_MAX>();
    }
    case 2: {
      return addNodeUnderCapacity<uint32_t, uint16_t, UINT32_MAX>();
    }
    case 4: {
      return addNodeUnderCapacity<uint32_t, uint32_t, UINT32_MAX>();
    }
    case 8: {
      return addNodeUnderCapacity<uint32_t, uint64_t, UINT32_MAX>();
    }
    default:
      assert(0);
      return 0;
    }
  }
  case 8: {
    switch (curr_node_size_in_bytes) {
    case 1: {
      return addNodeUnderCapacity<uint64_t, uint8_t, UINT64_MAX>();
    }
    case 2: {
      return addNodeUnderCapacity<uint64_t, uint16_t, UINT64_MAX>();
    }
    case 4: {
      return addNodeUnderCapacity<uint64_t, uint32_t, UINT64_MAX>();
    }
    case 8: {
      return addNodeUnderCapacity<uint64_t, uint64_t, UINT64_MAX>();
    }
    default:
      assert(0);
      return 0;
    }
  }
  default:
    assert(0);
    return 0;
  }
}

uint64_t RecursiveGraph::addNode(void) {
  if (__builtin_expect(
          static_cast<bool>(curr_nodes_array_size < curr_nodes_array_capacity),
          1)) {
    addNodeUnderCapacity();
  } else {
    // full capacity
    const uint64_t actual_arr_size = curr_nodes_array_capacity + 1llu;
    const uint64_t popcnt =
        static_cast<uint64_t>(__builtin_popcountll(actual_arr_size));
    assert(popcnt == 1);
    const uint64_t old_index_size =
        63 - static_cast<uint64_t>(__builtin_clzll(actual_arr_size));
    const uint64_t new_index_size = old_index_size + 1llu;
    const uint64_t new_index_size_in_bytes = (new_index_size & 7llu)
                                                 ? (new_index_size >> 3) + 1
                                                 : new_index_size >> 3;

    if (__builtin_expect(static_cast<bool>(new_index_size_in_bytes <=
                                           curr_node_size_in_bytes),
                         1)) {
      const uint64_t new_arr_size = 1llu << new_index_size;
      curr_nodes_array_capacity = new_arr_size - 1llu;

      void *new_nodes_array =
          aligned_malloc_multiple_of_alignment(new_arr_size, 32);

      memcpy(new_nodes_array, nodes_array, actual_arr_size);

      Node<uint8_t, uint8_t> *nodes_array_ptr =
          static_cast<Node<uint8_t, uint8_t> *>(nodes_array);

      nodes_array[UINT8_MAX]._root_edge_index = UINT8_MAX;

      curr_nodes_array_size++;
    } else {
      // full size
    }
    uint64_t new_capacity = curr_nodes_array_capacity * 2;
  }
}

uint64_t addEdge(const uint64_t start_node, const uint64_t end_node) {
  //
}

uint64_t findEdge(const uint64_t start_node, const uint64_t end_node) {
  //
}

void deleteEdge(const uint64_t edge) {
  //
}

std::vector<char> RecursiveGraph::_names;
std::vector<Node> RecursiveGraph::_nodes;
std::map<NodeEnum, uint64_t> RecursiveGraph::_node_index_map;

Node::Node(const NodeIdentifier &identifier)
    : _identifier(identifier.getName(), identifier.getNumber()) {
  //::new (&_identifier)
  //    NodeIdentifier(identifier.getName(), identifier.getNumber());
}

void Node::addGraphInternalNode(const NodeIdentifier &name) {
  Node *node = findGraphInternalNode(name);
  assert(node == nullptr);
  node = new Node(name);
  _internal_graph_nodes.insert(node);
}

void Node::connectGraphNodes(NodeIdentifier node1_identifier,
                             NodeIdentifier subnode1_identifier,
                             NodeIdentifier node2_identifier,
                             NodeIdentifier subnode2_identifier) {
  Node *node = findGraphInternalNode(node1_identifier);
  assert(node != nullptr);
  node = findGraphInternalNode(node2_identifier);
  assert(node != nullptr);
}

Node *Node::findGraphInternalNode(NodeIdentifier name) {}

Node *Node::findNode(NodeIdentifier name) {}

static void buildHalfAdderNode(void) {
  master_node->addGraphInternalNode("Half adder");

  Node *half_adder = master_node->findGraphInternalNode("half adder");

  half_adder->addGraphInternalNode("input1");
  half_adder->addGraphInternalNode("input2");
  half_adder->addGraphInternalNode("output_sum");
  half_adder->addGraphInternalNode("output_carry");

  half_adder->connectGraphNodes("input1", "input", "xor", "input1");
  half_adder->connectGraphNodes("input2", "input", "xor", "input1");
  half_adder->connectGraphNodes("input1", "input", "and", "and");
  half_adder->connectGraphNodes("input2", "input", "and", "and");
  half_adder->connectGraphNodes("and", "output", "output_sum", "output");
  half_adder->connectGraphNodes("or", "output", "output_carry", "output");
}

void buildMaster() {
  master_node = new Node();

  buildHalfAdderNode();
}
