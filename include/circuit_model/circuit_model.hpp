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
  std::map<uint32_t, uint32_t> _fanout_nodes;
  std::map<uint32_t, uint32_t> _fanin_nodes;

  uint32_t _fanout_count;
  uint32_t _fanin_count;

public:
  CircuitNode(void) = delete;
  CircuitNode(const uint32_t index, const CircuitNodeType type,
              const uint32_t value)
      : _index(index), _type(type), _value(value), _fanout_count(0),
        _fanin_count(0) {}

  inline uint32_t getIndex(void) const { return _index; }
  inline CircuitNodeType getType(void) const { return _type; }
  inline uint32_t getValue(void) const { return _value; }

  inline void addFanout(const uint32_t sink) {
    if (!_fanout_nodes.contains(sink)) {
      _fanout_nodes[sink] = 1;
    } else {
      _fanout_nodes[sink]++;
    }
    _fanout_count++;
  }

  inline void addFanin(const uint32_t source) {
    if (!_fanin_nodes.contains(source)) {
      _fanin_nodes[source] = 1;
    } else {
      _fanin_nodes[source]++;
    }
    _fanin_count++;
  }

  inline void forEachFanout(
      std::function<IteratorStatus(const uint32_t sink, const uint32_t count)>
          f) const {
    uint32_t count(0);
    for (auto fanout : _fanout_nodes) {
      const IteratorStatus status = f(fanout.first, fanout.second);
      count += fanout.second;
      if (status == IterationBreak) {
        return;
      }
    }
    assert(count == _fanout_count);
  }

  inline void forEachFanin(
      std::function<IteratorStatus(const uint32_t source, const uint32_t count)>
          f) const {
    uint32_t count(0);
    for (auto fanin : _fanin_nodes) {
      const IteratorStatus status = f(fanin.first, fanin.second);
      count += fanin.second;
      if (status == IterationBreak) {
        return;
      }
    }
    assert(count == _fanin_count);
  }

  inline uint32_t getFanoutCount(void) const { return _fanout_count; }
  inline uint32_t getFaninCount(void) const { return _fanin_count; }
};

typedef std::vector<CircuitNode> CircuitNodes;

class CircuitModel {
private:
  CircuitNodes circuit_nodes;

public:
  inline uint32_t addNode(const CircuitNodeType type, const uint32_t value) {
    const uint32_t index = circuit_nodes.size();
    circuit_nodes.push_back(CircuitNode(index, type, value));
    return index;
  }

  inline void addEdge(const uint32_t source, const uint32_t sink) {
    circuit_nodes[source].addFanout(sink);
    circuit_nodes[sink].addFanin(source);
  }

  inline uint32_t getNodeCount(void) const { return circuit_nodes.size(); }

  inline const CircuitNode &getNode(const uint32_t index) const {
    assert(circuit_nodes[index].getIndex() == index);
    return circuit_nodes[index];
  }

  inline void
  forEachNode(std::function<IteratorStatus(const CircuitNode &)> f) const {
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
