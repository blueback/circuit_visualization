#include "circuit_animator/circuit_animator.hpp"

void CircuitAnimator::traverseCircuitLevelized(
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

void CircuitAnimator::finalizeLayout(void) {
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
        char label_codepoint;
        switch (_circuit.getNode(index).getType()) {
        case AdderType: {
          color = BLUE;
          label_codepoint = '+';
          break;
        }
        case MultiplierType: {
          color = RED;
          label_codepoint = 'x';
          break;
        }
        case ConstantType: {
          color = GREEN;
          const uint32_t val = _circuit.getNode(index).getValue();
          assert(val >= 0 && val <= 9);
          label_codepoint = '0' + val;
          break;
        }
        case InputNodeType: {
          color = YELLOW;
          label_codepoint = 'I';
          break;
        }
        case OutputNodeType: {
          color = PURPLE;
          label_codepoint = 'O';
          break;
        }
        default:
          assert(0);
          break;
        };

        _node_animation_frames.push_back(CircuitNodeAnimKeyFrame(
            curr_time, curr_time + KEY_FRAME_TIME, MAX_NODE_RADIUS,
            curr_node_center, color, label_codepoint));

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
               source_index, sink_index, curr_time, curr_time + KEY_FRAME_TIME);

        curr_time += KEY_FRAME_TIME - KEY_FRAME_OVERLAP_TIME;

        return IterationContinue;
      });
}
