#ifndef __CIRCUIT_SOLVER_HPP__
#define __CIRCUIT_SOLVER_HPP__

#include "standard_defs/standard_defs.hpp"
#include "circuit_model/circuit_model.hpp"

namespace IntegerFactorization {
class RegularAPCircuit : public CircuitModel {
public:
  void createCircuit(const uint32_t degree);
};
class Opt01Circuit : public CircuitModel {
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
public:
  static constexpr size_t LABEL_LEN = 21;

private:
  float _max_radius;
  Vector2 _center;
  Color _color;
  char _label[LABEL_LEN];

public:
  CircuitNodeAnimKeyFrame(void) = delete;
  CircuitNodeAnimKeyFrame(const float start_time, const float end_time,
                          const float max_radius, const Vector2 center,
                          const Color color, const char *label)
      : CircuitAnimKeyFrame(start_time, end_time), _max_radius(max_radius),
        _center(center), _color(color) {

    assert(strlen(label) < LABEL_LEN);
    sprintf(_label, "%s", label);
  }

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

  Color getColor(void) const { return _color; }

  const char *getLabel(void) const { return _label; }
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
  static constexpr float KEY_FRAME_TIME = 0.02f;
  static constexpr float KEY_FRAME_OVERLAP_TIME = 0.009f;
  static constexpr float MAX_NODE_RADIUS = 20.0f;

  const CircuitModel &_circuit;

  std::vector<CircuitNodeAnimKeyFrame> _node_animation_frames;
  std::vector<CircuitEdgeAnimKeyFrame *> _edge_animation_frames;
  std::vector<uint32_t> _node_anim_frame_indices;

public:
  CircuitAnimator(void) = delete;

  CircuitAnimator(const CircuitModel &circuit) : _circuit(circuit) {
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

          Color color = RED;
          char label[CircuitNodeAnimKeyFrame::LABEL_LEN];
          switch (_circuit.getNode(index).getType()) {
          case AdderType:
            color = BLUE;
            sprintf(label, "+");
            break;
          case MultiplierType:
            color = RED;
            sprintf(label, "x");
            break;
          case ConstantType:
            color = GREEN;
            sprintf(label, "%d", _circuit.getNode(index).getValue());
            break;
          case InputNodeType:
            color = YELLOW;
            sprintf(label, "I");
            break;
          case OutputNodeType:
            color = PURPLE;
            sprintf(label, "O");
            break;
          default:
            assert(0);
            break;
          };

          _node_animation_frames.push_back(CircuitNodeAnimKeyFrame(
              curr_time, curr_time + KEY_FRAME_TIME, MAX_NODE_RADIUS,
              curr_node_center, color, label));

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
    for (size_t i = 0; i < _edge_animation_frames.size(); i++) {
      _edge_animation_frames[i]->updateFramePoints(time);
      Vector2 *points;
      size_t count;
      _edge_animation_frames[i]->getFramePoints(&points, &count);
      DrawSplineLinear(points, count, 2.0f, YELLOW);
    }

    for (auto node_anim_frame : _node_animation_frames) {
      const float radius = node_anim_frame.getCurrentRadius(time);
      const Vector2 center = node_anim_frame.getCenter();
      const Color color = node_anim_frame.getColor();
      const char *label = node_anim_frame.getLabel();
      DrawCircleV(center, radius, color);
      if (radius == node_anim_frame.getMaxRadius()) {
        DrawText(label, center.x - 8, center.y - 14, 30.0f, YELLOW);
      }
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
  static constexpr Rectangle SCREEN_RECT = {
      .x = 0, .y = 0, .width = SCREEN_WIDTH, .height = SCREEN_HEIGHT};

  void drawCircuit(CircuitAnimator &circuit_animator, const float time);

public:
  void solve(void);
};

#endif // __CIRCUIT_SOLVER_HPP__
