#include "raylib.h"
#include "raymath.h"
#include "standard_defs/standard_includes.hpp"

enum CircuitNodeType : uint8_t {
  FirstCircuitNodeType = 0,
  AdderType = FirstCircuitNodeType,
  MultiplierType,
  ConstantType,
  InputNodeType,
  OutputNodeType,
  LastCircuitNodeType = OutputNodeType
};

enum IteratorStatus : uint8_t { IterationBreak, IterationContinue };

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

  void forEachFanout(std::function<IteratorStatus(const uint32_t)> f) {
    for (auto fanout : fanout_nodes) {
      f(fanout);
    }
  }

  void forEachFanin(std::function<IteratorStatus(const uint32_t)> f) {
    for (auto fanin : fanin_nodes) {
      f(fanin);
    }
  }
};

typedef std::vector<CircuitNode> CircuitNodes;

class Circuit {
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
};

namespace IntegerFactorization {
class RegularAPCircuit : public Circuit {
public:
  void createCircuit(const uint32_t degree);
};
}; // namespace IntegerFactorization

class CircuitAnimKeyFrame {
private:
  float _start_time;
  float _end_time;

public:
  CircuitAnimKeyFrame(void) = delete;
  CircuitAnimKeyFrame(const float start_time, const float end_time)
      : _start_time(start_time), _end_time(end_time) {}

  float getStartTime(void) const { return _start_time; }

  float getEndTime(void) const { return _end_time; }
};

class CircuitNodeAnimKeyFrame : CircuitAnimKeyFrame {
private:
  float _max_radius;

public:
  CircuitNodeAnimKeyFrame(void) = delete;
  CircuitNodeAnimKeyFrame(const float start_time, const float end_time,
                          const float max_radius)
      : CircuitAnimKeyFrame(start_time, end_time), _max_radius(max_radius) {}

  float getMaxRadius(void) const { return _max_radius; }

  float getCurrentRadius(const float time) {
    if (time < getStartTime()) {
      return 0.0f;
    } else if (time > getEndTime()) {
      return _max_radius;
    } else {
      float norm_time = Normalize(time, getStartTime(), getEndTime());
      float sin_interpolate = (sinf(PI * norm_time - PI * 0.5) + 1) * 0.5;
      return Lerp(getStartTime(), getEndTime(), sin_interpolate);
    }
  }
};

class CircuitEdgeAnimKeyFrame : CircuitAnimKeyFrame {
private:
  std::vector<Vector2> _middle_points;

public:
  CircuitEdgeAnimKeyFrame(void) = delete;
  CircuitEdgeAnimKeyFrame(const float start_time, const float end_time)
      : CircuitAnimKeyFrame(start_time, end_time) {}

  void addPoint(const Vector2 point) { _middle_points.push_back(point); }
};

class CircuitSolver {
private:
  static constexpr float SCREEN_WIDTH = 1920;
  static constexpr float SCREEN_HEIGHT = 1080;
  static constexpr Vector2 SCREEN_RESOLUTION = {.x = SCREEN_WIDTH,
                                                .y = SCREEN_HEIGHT};
  static constexpr float SCREEN_FPS = 60;
  static constexpr int INPUT_DEGREE = 7;
  static constexpr float NODE_RADIUS = 30.0f;

  static constexpr Rectangle SCREEN_RECT = {
      .x = 0, .y = 0, .width = SCREEN_WIDTH, .height = SCREEN_HEIGHT};

  Vector2 input_position;
  std::vector<Vector2> constant_layer_positions;
  std::vector<Vector2> adder_layer_positions;
  std::vector<Vector2> multiplier_layer_positions;

  void constantLayer(void);

  void adderLayer(void);

  void multiplierLayers(void);

  void drawCircuit(void);

public:
  void solve(void);
};
