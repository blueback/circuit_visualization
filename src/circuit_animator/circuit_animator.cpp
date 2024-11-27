#include "circuit_animator/circuit_animator.hpp"

void CircuitAnimator::traverseCircuitLevelized(
    std::function<IteratorStatus(const uint32_t, const uint32_t)> fn,
    std::function<IteratorStatus(const uint32_t, const uint32_t, const uint32_t,
                                 const uint32_t)>
        fe) const {
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

    _circuit.getNode(index).forEachFanout(
        [&](const uint32_t sink_index, const uint32_t count) {
          visited_fanin_counts[sink_index] += count;

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

    _circuit.getNode(index).forEachFanout(
        [&](const uint32_t sink_index, const uint32_t count) {
          visited_fanin_counts[sink_index] += count;

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

      _circuit.getNode(index).forEachFanin(
          [&](const uint32_t source_index, const uint32_t count) {
            status = fe(source_index, index, layer, count);
            return status;
          });

      if (status == IterationBreak) {
        return;
      }

      _circuit.getNode(index).forEachFanout(
          [&](const uint32_t sink_index, const uint32_t count) {
            visited_fanin_counts[sink_index] += count;

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

uint32_t CircuitAnimator::getNumberOfLayers(void) const {
  uint32_t curr_layer = 0;

  traverseCircuitLevelized(
      [&](const uint32_t, const uint32_t layer) {
        if (layer != curr_layer) {
          assert(layer == curr_layer + 1);
          curr_layer++;
        }
        return IterationContinue;
      },
      [&](const uint32_t, const uint32_t, const uint32_t, const uint32_t) {
        return IterationContinue;
      });

  return curr_layer + 1;
}

float CircuitAnimator::getInterLayerDistance(void) const {
  const uint32_t layer_count = getNumberOfLayers();
  const float screen_height = _screen_resolution.y;
  return screen_height / (layer_count + 1.0f);
}

uint32_t CircuitAnimator::getLayerNodeCount(const uint32_t layer) const {
  uint32_t layer_node_count(0);
  traverseCircuitLevelized(
      [&](const uint32_t, const uint32_t curr_layer) {
        if (layer == curr_layer) {
          layer_node_count++;
        }
        return IterationContinue;
      },
      [&](const uint32_t, const uint32_t, const uint32_t, const uint32_t) {
        return IterationContinue;
      });
  return layer_node_count;
}

float CircuitAnimator::getLayerInterNodeDistance(const uint32_t layer) const {
  const uint32_t layer_node_count = getLayerNodeCount(layer);
  const float screen_width = _screen_resolution.x;
  return screen_width / (layer_node_count + 1);
}

void CircuitAnimator::finalizeLayout(void) {
  float curr_time(_animation_start_time);
  uint32_t curr_layer = 0;
  Vector2 curr_node_center = {.x = 0.0f, .y = getInterLayerDistance()};

  traverseCircuitLevelized(
      [&](const uint32_t index, const uint32_t layer) {
        if (layer == curr_layer) {
          curr_node_center = Vector2Add(
              curr_node_center,
              {.x = getLayerInterNodeDistance(curr_layer), .y = 0.0f});
        } else {
          assert(layer == curr_layer + 1);
          curr_layer++;
          curr_node_center = Vector2Add(
              curr_node_center, {.x = 0.0f, .y = getInterLayerDistance()});
          curr_node_center.x = getLayerInterNodeDistance(curr_layer);
        }

        _node_animation_frames.push_back(CircuitNodeAnimKeyFrame(
            curr_time, curr_time + KEY_FRAME_TIME,
            MAX_NODE_RADIUS_RATIO * _screen_resolution.y, curr_node_center,
            _circuit.getNode(index).getType(),
            _circuit.getNode(index).getValue()));

        printf("NODE_LAYOUT: index = %u, start_time = %f, end_time = %f\n",
               index, curr_time, curr_time + KEY_FRAME_TIME);

        _node_anim_frame_indices[index] = _node_animation_frames.size() - 1;

        curr_time += KEY_FRAME_TIME - KEY_FRAME_OVERLAP_TIME;

        return IterationContinue;
      },
      [&](const uint32_t source_index, const uint32_t sink_index,
          const uint32_t sink_layer, const uint32_t count) {
        const Vector2 start_point =
            _node_animation_frames[_node_anim_frame_indices[source_index]]
                .getCenter();

        const Vector2 end_point =
            _node_animation_frames[_node_anim_frame_indices[sink_index]]
                .getCenter();

        const float edge_count_inverse = 1.0f / (count + 1.0f);

        for (uint32_t i = 1; i <= count; i++) {
          const Vector2 mid_point =
              Vector2Lerp(start_point, end_point, i * edge_count_inverse);

          const Vector2 start_control_point = {.x = end_point.x,
                                               .y = mid_point.y};

          // const Vector2 mid_point =
          //     Vector2Lerp(start_point, end_point, 0.50f);

          // const Vector2 middle_point1 = {.x = start_control_point.x, .y =
          // mid_point.y};

          // const Vector2 mid_mid_point = Vector2Lerp(mid_point, end_point,
          // 0.50f);

          // const Vector2 control_point1 = {.x = end_point.x, .y =
          // mid_mid_point.y};

          _edge_animation_frames.push_back(new CircuitEdgeAnimKeyFrame(
              curr_time, curr_time + KEY_FRAME_TIME, start_point, end_point,
              start_control_point));

          //_edge_animation_frames[_edge_animation_frames.size() - 1]
          //    ->addMiddlePoint(middle_point1, control_point1);

          printf("EDGE_LAYOUT: source_index = %u, sink_index = %u, start_time "
                 "= %f, end_time = %f\n",
                 source_index, sink_index, curr_time,
                 curr_time + KEY_FRAME_TIME);

          curr_time += KEY_FRAME_TIME - KEY_FRAME_OVERLAP_TIME;
        }

        return IterationContinue;
      });

  _animation_end_time = curr_time + 2;
}
