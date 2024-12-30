#!/usr/bin/python3

import math

def convertToPascal(i):
    return "".join(x for x in i.replace("_", " ").title() if not x.isspace())

def getObjSign(obj_i):
    obj = globals()[obj_i]
    signature = f"{convertToPascal(obj_i)}<"
    for j in range(len(obj)):
        obj_field = obj[j]
        obj_field_type = obj_field[1]
        if j == len(obj) - 1:
            signature += f"{obj_field_type}"
        else:
            signature += f"{obj_field_type}, "
    signature += f">"
    return signature

def getObjSizeInBits(obj_i):
    obj = globals()[obj_i]
    total_bits = 0
    for j in range(len(obj)):
        obj_field = obj[j]
        obj_field_bit_count = obj_field[2]
        total_bits += obj_field_bit_count
    return total_bits

def getObjSizeInBytes(obj_i):
    obj = globals()[obj_i]
    total_bits = 0
    for j in range(len(obj)):
        obj_field = obj[j]
        obj_field_bit_count = obj_field[2]
        total_bits += obj_field_bit_count
    total_bytes = math.ceil(total_bits / 8.0)
    return total_bytes

circle_info = [
        ('center', 'Vector2', 2*4*8, False),
        ('radius', 'float', 4*8, False)]

interval_info = [
        ('start_time', 'float', 4*8, False),
        ('end_time', 'float', 4*8, False)]

node_info = [
        ('entry_edge_front_count', 'uint16_t', 16),
        ('first_entry_edge_front_index', 'uint32_t', 32),
        ('entry_edge_reverse_count', 'uint16_t', 16),
        ('first_entry_edge_reverse_index', 'uint32_t', 32),
        ('exit_edge_front_count', 'uint16_t', 16),
        ('first_exit_edge_front_index', 'uint32_t', 32),
        ('exit_edge_reverse_count', 'uint16_t', 16),
        ('first_exit_edge_reverse_index', 'uint32_t', 32),
        ('cluster_index', 'uint32_t', 32)]

cluster = [
        ('entry_node_count', 'uint16_t', 16),
        ('first_entry_node_index', 'uint32_t', 32),
        ('exit_node_count', 'uint16_t', 16),
        ('first_exit_node_index', 'uint32_t', 32)]

cluster_extra1 = [
        ('parent_cluster_index', 'uint32_t', 32),
        ('children_cluster_count', 'uint8_t', 8),
        ('first_child_cluster_index', 'uint32_t', 32)]

cluster_extra2 = [
        ('cluster_keyframe_count', 'uint8_t', 8),
        ('first_cluster_keyframe_index', 'uint32_t', 32)]

cluster_extra3 = [
        ('label_size', 'uint8_t', 8),
        ('label_offset', 'uint32_t', 32)]

cluster_keyframe = [
        ('start_territory', 'Rectangle', 4*4*8, False),
        ('end_territory', 'Rectangle', 4*4*8, False),
        ('interval', getObjSign('interval_info'), getObjSizeInBits('interval_info'), False)]

entry_node = [
        ('node_info', getObjSign('node_info'), getObjSizeInBits('node_info'), False)]

entry_node_extra1 = [
        ('entry_node_keyframe_count', 'uint8_t', 8),
        ('first_entry_node_keyframe_index', 'uint32_t', 32)]

entry_node_extra2 = [
        ('label_size', 'uint8_t', 8),
        ('label_offset', 'uint32_t', 32)]

entry_node_keyframe = [
        ('start_territory', getObjSign('circle_info'), getObjSizeInBits('circle_info'), False),
        ('end_territory', getObjSign('circle_info'), getObjSizeInBits('circle_info'), False),
        ('interval', getObjSign('interval_info'), getObjSizeInBits('interval_info'), False)]

exit_node = [
        ('node_info', getObjSign('node_info'), getObjSizeInBits('node_info'), False)]

exit_node_extra1 = [
        ('exit_node_keyframe_count', 'uint8_t', 8),
        ('first_exit_node_keyframe_index', 'uint32_t', 32)]

exit_node_extra2 = [
        ('label_size', 'uint8_t', 8),
        ('label_offset', 'uint32_t', 32)]

exit_node_keyframe = [
        ('start_territory', getObjSign('circle_info'), getObjSizeInBits('circle_info'), False),
        ('end_territory', getObjSign('circle_info'), getObjSizeInBits('circle_info'), False),
        ('interval', getObjSign('interval_info'), getObjSizeInBits('interval_info'), False)]


entry_edge_front = [('entry_node_index', 'uint32_t', 32)]
entry_edge_reverse = [('entry_node_index', 'uint32_t', 32)]
exit_edge_front = [('exit_node_index', 'uint32_t', 32)]
exit_edge_reverse = [('exit_node_index', 'uint32_t', 32)]

utility_structs = [
        'circle_info',
        'interval_info',
        'node_info']

recursive_circuit = [('cluster', 'clusters'),
                     ('cluster_extra1', 'cluster_extra1_arr'),
                     ('cluster_extra2', 'cluster_extra2_arr'),
                     ('cluster_extra3', 'cluster_extra3_arr'),
                     ('cluster_keyframe', 'cluster_keyframes'),
                     ('entry_node', 'entry_nodes'),
                     ('entry_node_extra1', 'entry_node_extra1_arr'),
                     ('entry_node_extra2', 'entry_node_extra2_arr'),
                     ('entry_node_keyframe', 'entry_node_keyframes'),
                     ('exit_node', 'exit_nodes'),
                     ('exit_node_extra1', 'exit_node_extra1_arr'),
                     ('exit_node_extra2', 'exit_node_extra2_arr'),
                     ('exit_node_keyframe', 'exit_node_keyframes'),
                     ('entry_edge_front', 'entry_edges_front'),
                     ('entry_edge_reverse', 'entry_edges_reverse'),
                     ('exit_edge_front', 'exit_edges_front'),
                     ('exit_edge_reverse', 'exit_edges_reverse')]

def generateClassStruct(i, f):
    obj = globals()[i]
    f.write(f"template<")
    for j in range(len(obj)):
        obj_field = obj[j]
        obj_field_name = obj_field[0]
        if j == len(obj) - 1:
            f.write(f"typename TYPE_{obj_field_name.upper()}")
        else:
            f.write(f"typename TYPE_{obj_field_name.upper()} ,")
    f.write(f">")
    f.write(f"class TRY_PACKED {convertToPascal(i)} {{\n")
    f.write(f"public:\n")
    for j in range(len(obj)):
        obj_field = obj[j]
        obj_field_name = obj_field[0]
        obj_field_bit_count = obj_field[2]
        if len(obj_field) == 4:
            obj_field_need_bit_field = obj_field[3]
            if obj_field_need_bit_field:
                f.write(f"TYPE_{obj_field_name.upper()} _{obj_field_name} : {obj_field_bit_count};\n")
            else:
                f.write(f"TYPE_{obj_field_name.upper()} _{obj_field_name};\n")
        else:
            f.write(f"TYPE_{obj_field_name.upper()} _{obj_field_name} : {obj_field_bit_count};\n")
    f.write(f"\nTRY_NOMAGIC({convertToPascal(i)})\n\n")
    f.write(f"TRY_INLINE explicit {convertToPascal(i)}(")
    for j in range(len(obj)):
        obj_field = obj[j]
        obj_field_name = obj_field[0]
        if j == len(obj) - 1:
            f.write(f"const TYPE_{obj_field_name.upper()} {obj_field_name}) {{\n")
        else:
            f.write(f"const TYPE_{obj_field_name.upper()} {obj_field_name},\n")
    for j in range(len(obj)):
        obj_field = obj[j]
        obj_field_name = obj_field[0]
        obj_field_bit_count = obj_field[2]
        #assert(int(obj_field_bit_count) <= 32)
        if len(obj_field) == 4:
            obj_field_need_bit_field = obj_field[3]
            if obj_field_need_bit_field:
                f.write(f"assert({obj_field_name} < (1llu << {obj_field_bit_count}));\n")
        else:
            f.write(f"assert({obj_field_name} < (1llu << {obj_field_bit_count}));\n")
        f.write(f"_{obj_field_name} = {obj_field_name};\n\n")
    f.write(f"}};\n\n")
    f.write(f"}};\n")
    signature = getObjSign(i)
    total_size_in_bytes = getObjSizeInBytes(i)
    f.write(f"static_assert(sizeof({signature}) == {total_size_in_bytes});\n\n") 
    pass

with open("tmp.cpp", 'w') as f:
    f.write(f"#ifndef __RECURSIVE_CIRCUIT02_HPP__\n")
    f.write(f"#define __RECURSIVE_CIRCUIT02_HPP__\n\n")
    f.write(f"#include \"standard_defs/standard_defs.hpp\"\n\n")
    f.write(f"namespace RecursiveCircuit01 {{\n\n")
    for i in utility_structs:
        generateClassStruct(i, f)
    for i, i_arr in recursive_circuit:
        generateClassStruct(i, f)
    f.write(f"}};\n\n")
    f.write(f"#endif // __RECURSIVE_CIRCUIT02_HPP__\n")
    f.write("\n\n\n")

    f.write(f"namespace RecursiveCircuit01 {{\n")
    for i, i_arr in recursive_circuit:
        signature = getObjSign(i)
        f.write(f"{signature} *{i_arr} = nullptr;\n")
    f.write("\n")
    for i, i_arr in recursive_circuit:
        signature = getObjSign(i)
        f.write(f"void allocate_{i_arr}(size_t {i}_count) {{\n")
        f.write(f"  {i_arr} = static_cast<{signature} *>(")
        f.write(f"malloc(sizeof({signature}) * {i}_count));")
        f.write("}\n\n")
    f.write(f"}};\n\n")
