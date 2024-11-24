#ifndef __CIRCUIT_MODEL_HPP__
#define __CIRCUIT_MODEL_HPP__

#include "standard_defs/standard_defs.hpp"

enum CircuitNodeType : uint8_t {
  FirstCircuitNodeType = 0,
  AdderType = FirstCircuitNodeType,
  MultiplierType,
  ConstantType,
  InputNodeType,
  OutputNodeType,
  LastCircuitNodeType = OutputNodeType
};

class CircuitNode {
private:
  uint32_t _index;
  CircuitNodeType _type;
  uint32_t _value;
  std::set<uint32_t> fanout_nodes;
  std::set<uint32_t> fanin_nodes;

public:
  CircuitNode(void) = delete;
  CircuitNode(const uint32_t index, const CircuitNodeType type,
              const uint32_t value)
      : _index(index), _type(type), _value(value) {}

  uint32_t getIndex(void) const { return _index; }
  CircuitNodeType getType(void) const { return _type; }
  uint32_t getValue(void) const { return _value; }

  void addFanout(const uint32_t sink) {
    assert(!fanout_nodes.contains(sink));
    fanout_nodes.insert(sink);
  }

  void addFanin(const uint32_t sink) {
    assert(!fanin_nodes.contains(sink));
    fanin_nodes.insert(sink);
  }

  void forEachFanout(std::function<IteratorStatus(const uint32_t)> f) const {
    for (auto fanout : fanout_nodes) {
      const IteratorStatus status = f(fanout);
      if (status == IterationBreak) {
        break;
      }
    }
  }

  void forEachFanin(std::function<IteratorStatus(const uint32_t)> f) const {
    for (auto fanin : fanin_nodes) {
      const IteratorStatus status = f(fanin);
      if (status == IterationBreak) {
        break;
      }
    }
  }

  uint32_t getFanoutCount(void) const { return fanout_nodes.size(); }
  uint32_t getFaninCount(void) const { return fanin_nodes.size(); }
};

typedef std::vector<CircuitNode> CircuitNodes;

class CircuitModel {
private:
  CircuitNodes circuit_nodes;

public:
  uint32_t addNode(const CircuitNodeType type, const uint32_t value) {
    const uint32_t index = circuit_nodes.size();
    circuit_nodes.push_back(CircuitNode(index, type, value));
    return index;
  }

  void addEdge(const uint32_t source, const uint32_t sink) {
    circuit_nodes[source].addFanout(sink);
    circuit_nodes[sink].addFanin(source);
  }

  uint32_t getNodeCount(void) const { return circuit_nodes.size(); }

  const CircuitNode &getNode(const uint32_t index) const {
    assert(circuit_nodes[index].getIndex() == index);
    return circuit_nodes[index];
  }

  void forEachNode(std::function<IteratorStatus(const CircuitNode &)> f) const {
    for (uint32_t i = 0; i < circuit_nodes.size(); i++) {
      assert(i == circuit_nodes[i].getIndex());
      const IteratorStatus status = f(circuit_nodes[i]);
      if (status == IterationBreak) {
        break;
      }
    }
  }
};

#endif // __CIRCUIT_MODEL_HPP__
