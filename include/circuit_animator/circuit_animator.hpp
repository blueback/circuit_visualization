#ifndef __CIRCUIT_ANIMATOR_HPP__
#define __CIRCUIT_ANIMATOR_HPP__

#include "circuit_model/circuit_model.hpp"

class CircuitAnimKeyFrame {
private:
  float _start_time;
  float _end_time;

public:
  CircuitAnimKeyFrame(void) = delete;
  CircuitAnimKeyFrame(const float start_time, const float end_time)
      : _start_time(start_time), _end_time(end_time) {}

  inline float getStartTime(void) const { return _start_time; }

  inline float getEndTime(void) const { return _end_time; }
};

class CircuitNodeAnimKeyFrame : public CircuitAnimKeyFrame {
private:
  float _max_radius;
  Vector2 _center;
  Color _color;
  char _label_codepoint;

public:
  CircuitNodeAnimKeyFrame(void) = delete;
  CircuitNodeAnimKeyFrame(const float start_time, const float end_time,
                          const float max_radius, const Vector2 center,
                          const Color color, const char label_codepoint)
      : CircuitAnimKeyFrame(start_time, end_time), _max_radius(max_radius),
        _center(center), _color(color), _label_codepoint(label_codepoint) {}

  inline float getMaxRadius(void) const { return _max_radius; }

  inline float getCurrentRadius(const float time) const {
    const float clamped_time = Clamp(time, getStartTime(), getEndTime());

    const float norm_time =
        Normalize(clamped_time, getStartTime(), getEndTime());

    float norm_sin_interpolated_time =
        (sinf(PI * norm_time - PI * 0.5) + 1) * 0.5;

    float sin_interpolated_radius =
        Lerp(0, getMaxRadius(), norm_sin_interpolated_time);

    return sin_interpolated_radius;
  }

  inline Vector2 getCenter(void) const { return _center; }

  inline Color getColor(void) const { return _color; }

  inline const char getLabelCodepoint(void) const { return _label_codepoint; }
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

  inline size_t getSegment(const float time) const {
    const float clamped_time = Clamp(time, getStartTime(), getEndTime());

    const size_t segment =
        (clamped_time - getStartTime()) / _segment_interval_time;

    if (segment == _middle_points.size() + 1) {
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

  inline Vector2 getCurrentPoint(const float time) const {
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

  inline void updateFramePoints(const float time) {
    _frame_points.push_back(getCurrentPoint(time));
  }

  inline void getFramePoints(Vector2 **points, size_t *count) {
    *points = &_frame_points[0];
    *count = _frame_points.size();
  }
};

class CircuitAnimator {
private:
  static constexpr float KEY_FRAME_TIME = 0.40f;
  static constexpr float KEY_FRAME_OVERLAP_TIME = 0.10f;
  static constexpr float MAX_NODE_RADIUS_RATIO = 30.0f / 720;
  static constexpr float EDGE_WIDTH = 2.0f;
  static constexpr float LABEL_FONT_SIZE_RATIO = 30.0f / 720;
  static constexpr Vector2 LABEL_DISPLACEMENT_RATIO = {
      .x = LABEL_FONT_SIZE_RATIO / 4, .y = LABEL_FONT_SIZE_RATIO / 2};

  const CircuitModel &_circuit;
  const Vector2 _screen_resolution;
  const Font _font;

  std::vector<CircuitNodeAnimKeyFrame> _node_animation_frames;
  std::vector<CircuitEdgeAnimKeyFrame *> _edge_animation_frames;
  std::vector<uint32_t> _node_anim_frame_indices;
  float _animation_start_time;
  float _animation_end_time;

private:
  void finalizeLayout(void);

  void traverseCircuitLevelized(
      std::function<IteratorStatus(const uint32_t, const uint32_t)> fn,
      std::function<IteratorStatus(const uint32_t, const uint32_t,
                                   const uint32_t)>
          fe) const;

  uint32_t getNumberOfLayers(void) const;

  float getInterLayerDistance(void) const;

  uint32_t getLayerNodeCount(const uint32_t layer) const;

  float getLayerInterNodeDistance(const uint32_t layer) const;

public:
  CircuitAnimator(void) = delete;

  CircuitAnimator(const CircuitModel &circuit, const Vector2 screen_resolution,
                  const float start_time)
      : _circuit(circuit), _screen_resolution(screen_resolution),
        _font(LoadFont("./resources/DotGothic16-Regular.ttf")),
        _animation_start_time(start_time) {

    _node_anim_frame_indices.resize(_circuit.getNodeCount(), 0);
    _animation_end_time = 0.0f;
    finalizeLayout();
  }

  inline bool updateCircuitAnimation(const float time) {
    for (size_t i = 0; i < _edge_animation_frames.size(); i++) {
      _edge_animation_frames[i]->updateFramePoints(time);
      Vector2 *points;
      size_t count;
      _edge_animation_frames[i]->getFramePoints(&points, &count);
      DrawSplineLinear(points, count, EDGE_WIDTH, LIGHTGRAY);
    }

    for (auto node_anim_frame : _node_animation_frames) {
      const float radius = node_anim_frame.getCurrentRadius(time);
      const Vector2 center = node_anim_frame.getCenter();
      const Color color = node_anim_frame.getColor();
      DrawCircleV(center, radius, color);
      DrawCircleLinesV(center, radius, RAYWHITE);

      if (radius == node_anim_frame.getMaxRadius()) {
        const char label_codepoint = node_anim_frame.getLabelCodepoint();
        DrawTextCodepoint(
            _font, label_codepoint,
            Vector2Subtract(center, Vector2Multiply(LABEL_DISPLACEMENT_RATIO,
                                                    {_screen_resolution.y,
                                                     _screen_resolution.y})),
            LABEL_FONT_SIZE_RATIO * _screen_resolution.y, BLACK);
      }
    }

    if (time > _animation_end_time) {
      return false;
    }

    return true;
  }

  inline float getAnimationEndTime(void) const { return _animation_end_time; }
};

#endif // __CIRCUIT_ANIMATOR_HPP__
