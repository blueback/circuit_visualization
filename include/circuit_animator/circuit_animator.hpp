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
  Color _outer_color;
  Color _inner_color;
  char _label_codepoint;

  static constexpr Vector2 LABEL_DISPLACEMENT_RATIO = {.x = 0.25f, .y = 0.5f};

  static inline Color findOuterColor(const CircuitNodeType type) {
    switch (type) {
    case AdderType:
      return BLUE;
    case MultiplierType:
      return RED;
    case ConstantType:
      return LIME;
    case InputNodeType:
      return GOLD;
    case OutputNodeType:
      return MAROON;
    default:
      assert(0);
      return BLACK;
    };
  }

  static inline Color findInnerColor(const CircuitNodeType type) {
    switch (type) {
    case AdderType:
      return SKYBLUE;
    case MultiplierType:
      return BEIGE;
    case ConstantType:
      return GREEN;
    case InputNodeType:
      return YELLOW;
    case OutputNodeType:
      return PINK;
    default:
      assert(0);
      return BLACK;
    };
  }

  static inline char findLabelCodepoint(const CircuitNodeType type,
                                        const uint32_t value) {
    switch (type) {
    case AdderType:
      return '+';
    case MultiplierType:
      return 'x';
    case ConstantType:
      assert(value >= 0 && value <= 9);
      return '0' + value;
    case InputNodeType:
      return 'I';
    case OutputNodeType:
      return 'O';
    default:
      assert(0);
      return 'N';
    }
  }

public:
  CircuitNodeAnimKeyFrame(void) = delete;
  CircuitNodeAnimKeyFrame(const float start_time, const float end_time,
                          const float max_radius, const Vector2 center,
                          const CircuitNodeType type, const uint32_t value)
      : CircuitAnimKeyFrame(start_time, end_time), _max_radius(max_radius),
        _center(center), _outer_color(findOuterColor(type)),
        _inner_color(findInnerColor(type)),
        _label_codepoint(findLabelCodepoint(type, value)) {}

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

  inline Color getOuterColor(void) const { return _outer_color; }

  inline Color getInnerColor(void) const { return _inner_color; }

  inline char getLabelCodepoint(void) const { return _label_codepoint; }

  inline float getLabelCurrentSize(const float time) const {
    return getCurrentRadius(time);
  }

  inline Vector2 getLabelCurrentPosition(const float time) const {
    const Vector2 center = getCenter();
    const float label_size = getLabelCurrentSize(time);
    const Vector2 displacement =
        Vector2Multiply(LABEL_DISPLACEMENT_RATIO, {label_size, label_size});

    return Vector2Subtract(center, displacement);
  }
};

class CircuitEdgeAnimKeyFrame : public CircuitAnimKeyFrame {
private:
  Vector2 _start_point;
  Vector2 _end_point;
  std::vector<Vector2> _middle_points;
  std::vector<Vector2> _control_points;
  float _segment_interval_time;

  float _max_node_radius;
  float _arrow_end_time;

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
                          const Vector2 start_control_point,
                          const float max_node_radius, const float fps)
      : CircuitAnimKeyFrame(start_time, end_time), _start_point(start_point),
        _end_point(end_point), _max_node_radius(max_node_radius) {

    assert(start_time < end_time);
    _segment_interval_time = (end_time - start_time);
    _control_points.push_back(start_control_point);
    _arrow_end_time = calculateArrowEndTime(fps);
  }

  void addMiddlePoint(const Vector2 point, const Vector2 control_point,
                      const float fps) {
    _middle_points.push_back(point);
    const size_t segment_count = _middle_points.size() + 1;
    _segment_interval_time = (getEndTime() - getStartTime()) / segment_count;
    _control_points.push_back(control_point);
    _arrow_end_time = calculateArrowEndTime(fps);
  }

private:
  inline float calculateArrowEndTime(const float fps) {
    float frame_time = 0.5f / fps;

    for (float time = getStartTime(); time < getEndTime(); time += frame_time) {
      Vector2 curr_end_point = getCurrentSegmentEndPoint(time);
      const Vector2 end_point = getEndPoint();

      if (CheckCollisionPointCircle(curr_end_point, end_point,
                                    getMaxNodeRadius())) {
        return time;
      }
    }
    return getEndTime();
  }

  inline Vector2 getStartPoint(void) const { return _start_point; }
  inline Vector2 getEndPoint(void) const { return _end_point; }
  inline float getMaxNodeRadius(void) const { return _max_node_radius; }

  inline Vector2 getCurrentArrowHeadPoint(const float time) const {
    const float clamped_time = Clamp(time, getStartTime(), _arrow_end_time);
    Vector2 curr_end_point = getCurrentSegmentEndPoint(clamped_time);
    return curr_end_point;
  }

  inline Vector2 getCurrentArrowHeadControlPoint(const float time) const {
    const float clamped_time = Clamp(time, getStartTime(), _arrow_end_time);
    Vector2 end_control_point = getCurrentSegmentControlPoint(clamped_time);
    return end_control_point;
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
      return getStartPoint();
    } else {
      return _middle_points[segment - 1];
    }
  }

  inline Vector2 getSegmentEndPoint(const size_t segment) const {
    assert(segment < _middle_points.size() + 1);
    if (segment == _middle_points.size()) {
      return getEndPoint();
    } else {
      return _middle_points[segment];
    }
  }

  inline Vector2 getSegmentControlPoint(const size_t segment) const {
    assert(segment < _middle_points.size() + 1);
    assert(segment < _control_points.size());
    return _control_points[segment];
  }

  inline Vector2 getCurrentSegmentEndPoint(const float time) const {
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

    const Vector2 curr_segment_end_point =
        GetSplinePointBezierQuad(segment_start_point, segment_control_point,
                                 segment_end_point, segment_norm_time);
    return curr_segment_end_point;
  }

  inline Vector2 getCurrentSegmentControlPoint(const float time) const {
    const float clamped_time = Clamp(time, getStartTime(), getEndTime());

    const size_t segment = getSegment(clamped_time);

    const float segment_start_time = getSegmentStartTime(segment);
    const float segment_end_time = getSegmentEndTime(segment);
    const Vector2 segment_start_point = getSegmentStartPoint(segment);
    const Vector2 segment_control_point = getSegmentControlPoint(segment);

    const float segment_clamped_time =
        Clamp(time, segment_start_time, segment_end_time);

    const float segment_norm_time =
        Normalize(segment_clamped_time, segment_start_time, segment_end_time);

    const Vector2 curr_segment_control_point = Vector2Lerp(
        segment_start_point, segment_control_point, segment_norm_time);

    return curr_segment_control_point;
  }

  inline void forEachSegmentTillTime(
      const float time,
      std::function<IteratorStatus(const Vector2 segment_start_point,
                                   const Vector2 segment_end_point,
                                   const Vector2 segment_control_point)>
          f) {
    const float clamped_time = Clamp(time, getStartTime(), getEndTime());

    const size_t curr_segment = getSegment(clamped_time);

    for (size_t segment = 0; segment < curr_segment; segment++) {
      if (f(getSegmentStartPoint(segment), getSegmentEndPoint(segment),
            getSegmentControlPoint(segment)) == IterationBreak)
        return;
    }

    (void)f(getSegmentStartPoint(curr_segment), getCurrentSegmentEndPoint(time),
            getCurrentSegmentControlPoint(time));
  }

public:
  inline void
  forEachBezierQuadraticPoint(const float time,
                              std::function<void(const Vector2 point)> f) {

    if (time < getStartTime()) {
      return;
    }

    Vector2 prev_end_point = getSegmentStartPoint(0);

    forEachSegmentTillTime(time, [&](const Vector2 start_point,
                                     const Vector2 end_point,
                                     const Vector2 control_point) {
      assert(prev_end_point == start_point);
      f(start_point);
      f(control_point);
      prev_end_point = end_point;
      return IterationContinue;
    });

    f(prev_end_point);
  }

  inline bool getArrowPoints(const float time, Vector2 &v1, Vector2 &v2,
                             Vector2 &v3) const {
    if (time < getStartTime()) {
      return false;
    }

    const Vector2 arrow_head_point = getCurrentArrowHeadPoint(time);
    const Vector2 arrow_head_control_point =
        getCurrentArrowHeadControlPoint(time);

    const Vector2 dir_vector = Vector2Normalize(
        Vector2Subtract(arrow_head_control_point, arrow_head_point));
    const Vector2 v2_dir =
        Vector2Scale(Vector2Rotate(dir_vector, PI / 8.0f), 20.0f);
    const Vector2 v3_dir =
        Vector2Scale(Vector2Rotate(dir_vector, -PI / 8.0f), 20.0f);

    v1 = arrow_head_point;
    v2 = Vector2Add(arrow_head_point, v2_dir);
    v3 = Vector2Add(arrow_head_point, v3_dir);

    return true;
  }

  inline bool getCurrentHeadPoint(const float time,
                                  Vector2 &curr_head_point) const {
    if (time < getStartTime() || time > getEndTime()) {
      return false;
    }

    curr_head_point = getCurrentArrowHeadPoint(time);
    return true;
  }
};

class CircuitAnimator {
private:
  static constexpr float KEY_FRAME_TIME = 0.40f;
  static constexpr float KEY_FRAME_OVERLAP_TIME = 0.10f;
  static constexpr float MAX_NODE_RADIUS_RATIO = 30.0f / 720;
  static constexpr float EDGE_WIDTH = 4.0f;
  static constexpr Color EDGE_COLOR = BLACK;

  const CircuitModel &_circuit;
  const Vector2 _screen_resolution;
  const Color _screen_background_color;
  const float _fps;
  const Font _font;

  std::vector<CircuitNodeAnimKeyFrame> _node_animation_frames;
  std::vector<CircuitEdgeAnimKeyFrame *> _edge_animation_frames;
  std::vector<uint32_t> _node_anim_frame_indices;
  float _animation_start_time;
  float _animation_end_time;

private:
  void finalizeLayout(void);

  inline void traverseCircuitLevelized(
      std::function<IteratorStatus(const uint32_t, const uint32_t)> fn,
      std::function<IteratorStatus(const uint32_t, const uint32_t,
                                   const uint32_t, const uint32_t)>
          fe) const;

  inline uint32_t getNumberOfLayers(void) const;

  inline float getInterLayerDistance(void) const;

  inline uint32_t getLayerNodeCount(const uint32_t layer) const;

  inline float getLayerInterNodeDistance(const uint32_t layer) const;

  inline float getMaxNodeRadius(void) const;

public:
  CircuitAnimator(void) = delete;

  CircuitAnimator(const CircuitModel &circuit, const Vector2 screen_resolution,
                  const Color screen_background_color, const float fps,
                  const float start_time)
      : _circuit(circuit), _screen_resolution(screen_resolution),
        _screen_background_color(screen_background_color), _fps(fps),
        _font(LoadFont("./resources/DotGothic16-Regular.ttf")),
        _animation_start_time(start_time) {

    _node_anim_frame_indices.resize(_circuit.getNodeCount(), 0);
    _animation_end_time = 0.0f;
    finalizeLayout();
  }

  inline bool updateCircuitAnimation(const float time) const {
    for (size_t i = 0; i < _edge_animation_frames.size(); i++) {
      Vector2 curr_head_point;
      if (_edge_animation_frames[i]->getCurrentHeadPoint(time,
                                                         curr_head_point)) {
        DrawCircleGradient(curr_head_point.x, curr_head_point.y, 100.0f, WHITE,
                           Fade(_screen_background_color, 0.0f));
      }
    }

    for (size_t i = 0; i < _edge_animation_frames.size(); i++) {
      std::vector<Vector2> points;

      _edge_animation_frames[i]->forEachBezierQuadraticPoint(
          time, [&](const Vector2 point) { points.push_back(point); });

      DrawSplineBezierQuadratic(&points[0], points.size(), EDGE_WIDTH,
                                EDGE_COLOR);

      Vector2 v1, v2, v3;
      if (_edge_animation_frames[i]->getArrowPoints(time, v1, v2, v3)) {
        DrawTriangle(v1, v2, v3, EDGE_COLOR);
      }
    }

    for (auto node_anim_frame : _node_animation_frames) {
      const float radius = node_anim_frame.getCurrentRadius(time);
      const Vector2 center = node_anim_frame.getCenter();
      const Color outer_color = node_anim_frame.getOuterColor();
      const Color inner_color = node_anim_frame.getInnerColor();
      DrawCircleGradient(center.x, center.y, radius, inner_color, outer_color);
      DrawCircleLinesV(center, radius, RAYWHITE);

      const char label_codepoint = node_anim_frame.getLabelCodepoint();
      const float label_size = node_anim_frame.getLabelCurrentSize(time);
      const Vector2 label_position =
          node_anim_frame.getLabelCurrentPosition(time);
      DrawTextCodepoint(_font, label_codepoint, label_position, label_size,
                        BLACK);
    }

    if (time > _animation_end_time) {
      return false;
    }

    return true;
  }

  inline float getAnimationEndTime(void) const { return _animation_end_time; }
};

#endif // __CIRCUIT_ANIMATOR_HPP__
