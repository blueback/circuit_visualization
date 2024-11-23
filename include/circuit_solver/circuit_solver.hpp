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

class CircuitNodeAnimKeyFrame : public CircuitAnimKeyFrame {
private:
  float _max_radius;
  Vector2 _center;

public:
  CircuitNodeAnimKeyFrame(void) = delete;
  CircuitNodeAnimKeyFrame(const float start_time, const float end_time,
                          const float max_radius, const Vector2 center)
      : CircuitAnimKeyFrame(start_time, end_time), _max_radius(max_radius),
        _center(center) {}

  float getMaxRadius(void) const { return _max_radius; }
  float getCurrentRadius(const float time) const {
    const float clamped_time = Clamp(time, getStartTime(), getEndTime());

    const float norm_time =
        Normalize(clamped_time, getStartTime(), getEndTime());

    float norm_sin_interpolated_time =
        (sinf(PI * norm_time - PI * 0.5) + 1) * 0.5;

    float sin_interpolated_radius =
        Lerp(0, getMaxRadius(), norm_sin_interpolated_time);

    return sin_interpolated_radius;
  }
  Vector2 getCenter(void) const { return _center; }
};

class CircuitEdgeAnimKeyFrame : public CircuitAnimKeyFrame {
private:
  Vector2 _start_point;
  Vector2 _end_point;
  std::vector<Vector2> _middle_points;
  std::vector<Vector2> _control_points;
  float _segment_interval_time;
  std::vector<Vector2> _frame_points;

public:
  CircuitEdgeAnimKeyFrame(void) = delete;
  CircuitEdgeAnimKeyFrame(const CircuitEdgeAnimKeyFrame &) = delete;
  CircuitEdgeAnimKeyFrame(const CircuitEdgeAnimKeyFrame &&) = delete;
  const CircuitEdgeAnimKeyFrame &
  operator=(const CircuitEdgeAnimKeyFrame &) = delete;
  const CircuitEdgeAnimKeyFrame &
  operator=(const CircuitEdgeAnimKeyFrame &&) = delete;
  CircuitEdgeAnimKeyFrame(const float start_time, const float end_time,
                          const Vector2 start_point, const Vector2 end_point,
                          const Vector2 start_control_point)
      : CircuitAnimKeyFrame(start_time, end_time), _start_point(start_point),
        _end_point(end_point) {

    assert(start_time < end_time);
    _segment_interval_time = (end_time - start_time);
    _control_points.push_back(start_control_point);
  }

  void addMiddlePoint(const Vector2 point, const Vector2 control_point) {
    _middle_points.push_back(point);
    const size_t segment_count = _middle_points.size() + 1;
    _segment_interval_time = (getEndTime() - getStartTime()) / segment_count;
    _control_points.push_back(control_point);
  }

  size_t getSegment(const float time) const {
    const float clamped_time = Clamp(time, getStartTime(), getEndTime());

    const size_t segment =
        (clamped_time - getStartTime()) / _segment_interval_time;

    if (clamped_time - getStartTime() == _segment_interval_time) {
      return segment - 1;
    }
    return segment;
  }

  inline float getSegmentStartTime(const size_t segment) const {
    return getStartTime() + (segment * _segment_interval_time);
  }

  inline float getSegmentEndTime(const size_t segment) const {
    return getStartTime() + ((segment + 1) * _segment_interval_time);
  }

  inline Vector2 getSegmentStartPoint(const size_t segment) const {
    assert(segment < _middle_points.size() + 1);
    if (segment == 0) {
      return _start_point;
    } else {
      return _middle_points[segment - 1];
    }
  }

  inline Vector2 getSegmentEndPoint(const size_t segment) const {
    assert(segment < _middle_points.size() + 1);
    if (segment == _middle_points.size()) {
      return _end_point;
    } else {
      return _middle_points[segment];
    }
  }

  inline Vector2 getSegmentControlPoint(const size_t segment) const {
    assert(segment < _middle_points.size() + 1);
    assert(segment < _control_points.size());
    return _control_points[segment];
  }

  Vector2 getCurrentPoint(const float time) const {
    const float clamped_time = Clamp(time, getStartTime(), getEndTime());

    const size_t segment = getSegment(clamped_time);

    const float segment_start_time = getSegmentStartTime(segment);
    const float segment_end_time = getSegmentEndTime(segment);
    const Vector2 segment_start_point = getSegmentStartPoint(segment);
    const Vector2 segment_end_point = getSegmentEndPoint(segment);
    const Vector2 segment_control_point = getSegmentControlPoint(segment);

    const float segment_clamped_time =
        Clamp(time, segment_start_time, segment_end_time);

    const float segment_norm_time =
        Normalize(segment_clamped_time, segment_start_time, segment_end_time);

    const Vector2 curr_point =
        GetSplinePointBezierQuad(segment_start_point, segment_control_point,
                                 segment_end_point, segment_norm_time);
    return curr_point;
  }

  void updateFramePoints(const float time) {
    _frame_points.push_back(getCurrentPoint(time));
  }

  void getFramePoints(Vector2 **points, size_t *count) {
    *points = &_frame_points[0];
    *count = _frame_points.size();
  }
};

class CircuitAnimator {
private:
  static constexpr float KEY_FRAME_TIME = 1.0f;
  static constexpr float KEY_FRAME_OVERLAP_TIME = 0.2f;
  static constexpr float MAX_NODE_RADIUS = 20.0f;

  const Circuit &_circuit;

  std::vector<CircuitNodeAnimKeyFrame> _node_animation_frames;
  std::vector<CircuitEdgeAnimKeyFrame *> _edge_animation_frames;
  std::vector<uint32_t> _node_anim_frame_indices;

public:
  CircuitAnimator(void) = delete;

  CircuitAnimator(const Circuit &circuit) : _circuit(circuit) {
    _node_anim_frame_indices.resize(_circuit.getNodeCount(), 0);
  }

  void traverseCircuitLevelized(
      std::function<IteratorStatus(const uint32_t, const uint32_t)> fn,
      std::function<IteratorStatus(const uint32_t, const uint32_t,
                                   const uint32_t)>
          fe) {
    std::vector<uint32_t> node_index_stk1;
    std::vector<uint32_t> const_node_index_stk1;
    std::vector<uint32_t> visited_fanin_counts;
    uint32_t layer(0);
    visited_fanin_counts.resize(_circuit.getNodeCount(), 0);

    _circuit.forEachNode([&](const CircuitNode &node) {
      if (node.getType() == InputNodeType) {
        node_index_stk1.push_back(node.getIndex());
      } else if (node.getType() == ConstantType) {
        const_node_index_stk1.push_back(node.getIndex());
      }
      return IterationContinue;
    });

    std::vector<uint32_t> node_index_stk2;
    for (auto index : node_index_stk1) {
      const IteratorStatus status = fn(index, layer);

      if (status == IterationBreak) {
        return;
      }

      _circuit.getNode(index).forEachFanout([&](const uint32_t sink_index) {
        visited_fanin_counts[sink_index]++;

        assert(visited_fanin_counts[sink_index] <=
               _circuit.getNode(sink_index).getFaninCount());
        if (visited_fanin_counts[sink_index] ==
            _circuit.getNode(sink_index).getFaninCount()) {

          node_index_stk2.push_back(sink_index);
        }

        return IterationContinue;
      });
    }

    layer++;

    for (auto index : const_node_index_stk1) {
      const IteratorStatus status = fn(index, layer);

      if (status == IterationBreak) {
        return;
      }

      _circuit.getNode(index).forEachFanout([&](const uint32_t sink_index) {
        visited_fanin_counts[sink_index]++;

        assert(visited_fanin_counts[sink_index] <=
               _circuit.getNode(sink_index).getFaninCount());
        if (visited_fanin_counts[sink_index] ==
            _circuit.getNode(sink_index).getFaninCount()) {

          node_index_stk2.push_back(sink_index);
        }

        return IterationContinue;
      });
    }

    node_index_stk1.swap(node_index_stk2);
    node_index_stk2.clear();

    while (!node_index_stk1.empty()) {
      layer++;
      for (auto index : node_index_stk1) {
        IteratorStatus status = IterationContinue;

        status = fn(index, layer);

        if (status == IterationBreak) {
          return;
        }

        _circuit.getNode(index).forEachFanin([&](const uint32_t source_index) {
          status = fe(source_index, index, layer);
          return status;
        });

        if (status == IterationBreak) {
          return;
        }

        _circuit.getNode(index).forEachFanout([&](const uint32_t sink_index) {
          visited_fanin_counts[sink_index]++;

          assert(visited_fanin_counts[sink_index] <=
                 _circuit.getNode(sink_index).getFaninCount());
          if (visited_fanin_counts[sink_index] ==
              _circuit.getNode(sink_index).getFaninCount()) {

            node_index_stk2.push_back(sink_index);
          }

          return IterationContinue;
        });
      }

      node_index_stk1.swap(node_index_stk2);
      node_index_stk2.clear();
    }
  }

  void finalizeLayout(void) {
    float curr_time(0.0f);
    Vector2 curr_node_center = {.x = 50.0f, .y = 50.0f};
    uint32_t curr_layer = 0;
    traverseCircuitLevelized(
        [&](const uint32_t index, const uint32_t layer) {
          if (layer == curr_layer) {
            curr_node_center += {.x = 100.0f, .y = 0.0f};
          } else {
            assert(layer == curr_layer + 1);
            curr_layer++;
            curr_node_center += {.x = 0.0f, .y = 100.0f};
            curr_node_center.x = 100.0f;
          }

          _node_animation_frames.push_back(
              CircuitNodeAnimKeyFrame(curr_time, curr_time + KEY_FRAME_TIME,
                                      MAX_NODE_RADIUS, curr_node_center));

          printf("NODE_LAYOUT: index = %u, start_time = %f, end_time = %f\n",
                 index, curr_time, curr_time + KEY_FRAME_TIME);

          _node_anim_frame_indices[index] = _node_animation_frames.size() - 1;

          curr_time += KEY_FRAME_TIME - KEY_FRAME_OVERLAP_TIME;

          return IterationContinue;
        },
        [&](const uint32_t source_index, const uint32_t sink_index,
            const uint32_t sink_layer) {
          const Vector2 start_point =
              _node_animation_frames[_node_anim_frame_indices[source_index]]
                  .getCenter();

          const Vector2 end_point =
              _node_animation_frames[_node_anim_frame_indices[sink_index]]
                  .getCenter();

          const Vector2 start_control_point =
              Vector2Lerp(start_point, end_point, 0.5f);

          _edge_animation_frames.push_back(new CircuitEdgeAnimKeyFrame(
              curr_time, curr_time + KEY_FRAME_TIME, start_point, end_point,
              start_control_point));

          printf("EDGE_LAYOUT: source_index = %u, sink_index = %u, start_time "
                 "= %f, end_time = %f\n",
                 source_index, sink_index, curr_time,
                 curr_time + KEY_FRAME_TIME);

          curr_time += KEY_FRAME_TIME - KEY_FRAME_OVERLAP_TIME;

          return IterationContinue;
        });
  }

  void updateCircuitAnimation(const float time) {
    for (auto node_anim_frame : _node_animation_frames) {
      const float radius = node_anim_frame.getCurrentRadius(time);
      const Vector2 center = node_anim_frame.getCenter();
      DrawCircleV(center, radius, RED);
    }

    for (size_t i = 0; i < _edge_animation_frames.size(); i++) {
      _edge_animation_frames[i]->updateFramePoints(time);
      Vector2 *points;
      size_t count;
      _edge_animation_frames[i]->getFramePoints(&points, &count);
      DrawSplineLinear(points, count, 3.0f, YELLOW);
    }
  }
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

  void drawCircuit(CircuitAnimator &circuit_animator, const float time);

public:
  void solve(void);
};
