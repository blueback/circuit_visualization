#include "standard_defs/standard_defs.hpp"

enum BooleanNodeType : uint8_t {
  FirstBooleanNodeType = 0,
  BooleanNodeTypeAnd = FirstBooleanNodeType,
  BooleanNodeTypeOr,
  BooleanNodeTypeNot,
  BooleanNodeTypeInput,
  BooleanNodeTypeOutput,
  LastBooleanNodeType = BooleanNodeTypeOutput
};

enum ArithmeticNodeType : uint8_t {
  FirstArithmeticNodeType = 0,
  ArithmeticNodeTypeAdder = FirstArithmeticNodeType,
  ArithmeticNodeTypeSubstractor,
  ArithmeticNodeTypeMultiplier,
  ArithmeticNodeTypeConstant,
  ArithmeticNodeTypeInput,
  ArithmeticNodeTypeOutput,
  LastArithmeticNodeType = ArithmeticNodeTypeOutput
};

enum CircuitNodeType : uint8_t {
  FirstCircuitNodeType = 0,
  CircuitNodeTypeArithmetic = FirstCircuitNodeType,
  CircuitNodeTypeBoolean,
  CircuitNodeTypeCombinational,
  LastCircuitNodeType = CircuitNodeTypeCombinational
};

enum CircuitNodeShapeType : uint8_t {
  FirstCircuitNodeShapeType,
  CircuitNodeShapeTypeCircle = FirstCircuitNodeShapeType,
  CircuitNodeShapeTypeRectangle,
  CircuitNodeShapeTypeTrianle,
  CircuitNodeShapeTypeEllipse,
  LastCircuitNodeShapeType = CircuitNodeShapeTypeEllipse
};

class CircuitModel;

class AbstractCircuitNode {
public:
  CircuitNodeType _circuit_node_type;
  union {
    BooleanNodeType _boolean_node_type;
    ArithmeticNodeType _arithmetic_node_type;
  } _subtype;

public:
};

class BooleanCircuitNode : public AbstractCircuitNode {
private:
  BooleanNodeType _boolean_node_type;
  uint32_t _input_count;
  struct {
    bool _is_associative;
  } _properties;

public:
};

class CombinationalCircuitNode : public AbstractCircuitNode {
private:
  std::vector<AbstractCircuitNode> _nodes;

public:
};

class ArithmeticCircuitNode : public CombinationalCircuitNode {
private:
  std::vector<AbstractCircuitNode> _nodes;

public:
};

class CircuitNodeLayoutManager {};

class CustomCircuitNode {
private:
  std::vector<CustomInputKind> _inputs;
  std::vector<CustomOutputKink> _outputs;
  CustomLayoutManager _layout_manager;

  std::set<CustomInputKind> _fanins;
  std::set<CustomOutputKind> _fanouts;

public:
};

class CircuitText {
private:
  std::string _text;

public:
  std::string getText(void) const { return _text; };
};

class CircuitHierarchy {
private:
  std::vector<CircuitModel> _circuits;
  std::vector<CircuitHierarchy *> _circuit_hierarchies;
  CircuitText _text;

public:
};

class CustomCircuitModel {
private:
  std::vector<CustomCircuitNode> _nodes;

public:
};

class ImitationMachine {
private:
public:
};

class ComputationalFramework {
private:
  std::vector<uint32_t> _graph_node_indices;

public:
  class GraphNodeProperties {
  public:
  private:
  };

  class ConnectionProperties {
  public:
  private:
  };

  inline uint32_t addGraphNode(GraphNodeProperties properties);

  inline uint32_t connectGraphNodes(const uint32_t dag_node1_index,
                                    const uint32_t dag_node2_index,
                                    ConnectionProperties connection_properties);
};

class HalfAdder : public CombinationalCircuitNode {
private:
  CircuitModel _circuit;
  BooleanCircuitNode input1;
  BooleanCircuitNode input2;
  BooleanCircuitNode output_sum;
  BooleanCircuitNode output_carry;
  BooleanCircuitNode xor_node;
  BooleanCircuitNode and_node;

public:
  HalfAdder(void) {
    _circuit.addNode(BooleanNodeTypeInput, 0);
    addEdge(input1.getIndex(), xor_node.getIndex());
    addEdge(input2.getIndex(), xor_node.getIndex());
    addEdge(input1.getIndex(), and_node.getIndex());
    addEdge(input2.getIndex(), and_node.getIndex());
  }
};

class FullAdder : public CombinationalCircuitNode {
private:
public:
};

class NBitAdderCircuitNode : public ArithmeticCircuitNode {
private:
  CircuitModel _circuit;

public:
  NBitAdderCircuitNode(uint32_t input_width) {
    std::vector<uint32_t> input1_indices;
    std::vector<uint32_t> input2_indices;
    std::vector<uint32_t> output_indices;
    for (uint32_t i = 0; i < input_width; i++) {
      input1_indices.push_back(_circuit.addNode(BooleanNodeTypeInput, 0));
      input2_indices.push_back(_circuit.addNode(BooleanNodeTypeInput, 0));
      output_indices.push_back(_circuit.addNode(BooleanNodeTypeOutput, 0));
    }
  };
};

namespace RecursiveGraphNodeTypeSpace {
enum class RecursiveGraphNodeType { BooleanCircuit, ArithmeticCircuit };

namespace ArithmeticCircuitNodeSubtypeSpace {
enum class ArithmeticCircuitNodeSubtype {
  Adder,
  Substractor,
  Multiplier,
  Constant,
  Input,
  Output
};
};
}; // namespace RecursiveGraphNodeTypeSpace

class RecursiveGraphNode {
private:
  enum NodeType : uint32_t {
    FirstNodeType = 0,
    NodeTypeBooleanCircuit = FirstNodeType,
    NodeTypeArithmeticCircuit,
    LastNodeType = NodeTypeArithmeticCircuit
  };

  struct Fanout {
    uint32_t _sink_node_index;
    uint32_t _input_node_index;
  };

  struct Fanin {
    uint32_t _source_node_index;
    uint32_t _output_node_index;
  };

  std::vector<RecursiveGraphNode *> _graph_nodes;
  std::set<Fanout> _fanouts;
  std::set<Fanin> _fanins;

public:
  class GraphNodeProperties {
  public:
  private:
  };

  class ConnectionProperties {
  public:
  private:
  };

  inline uint32_t addGraphNode(GraphNodeProperties properties);

  inline uint32_t connectGraphNodes(const uint32_t dag_node1_index,
                                    const uint32_t dag_node2_index,
                                    ConnectionProperties connection_properties);
};

class HalfAdder : public RecursiveGraphNode {
private:
  CircuitModel _circuit;
  BooleanCircuitNode input1;
  BooleanCircuitNode input2;
  BooleanCircuitNode output_sum;
  BooleanCircuitNode output_carry;
  BooleanCircuitNode xor_node;
  BooleanCircuitNode and_node;

public:
  HalfAdder(void) {
    _circuit.addNode(BooleanNodeTypeInput, 0);
    addEdge(input1.getIndex(), xor_node.getIndex());
    addEdge(input2.getIndex(), xor_node.getIndex());
    addEdge(input1.getIndex(), and_node.getIndex());
    addEdge(input2.getIndex(), and_node.getIndex());
  }
};

class FullAdder : public CombinationalCircuitNode {
private:
public:
};

class NBitAdderCircuitNode : public ArithmeticCircuitNode {
private:
  CircuitModel _circuit;

public:
  NBitAdderCircuitNode(uint32_t input_width) {
    std::vector<uint32_t> input1_indices;
    std::vector<uint32_t> input2_indices;
    std::vector<uint32_t> output_indices;
    for (uint32_t i = 0; i < input_width; i++) {
      input1_indices.push_back(_circuit.addNode(BooleanNodeTypeInput, 0));
      input2_indices.push_back(_circuit.addNode(BooleanNodeTypeInput, 0));
      output_indices.push_back(_circuit.addNode(BooleanNodeTypeOutput, 0));
    }
  };
};
