#!/usr/bin/python3

import math

cluster = [
        ('entry_node_count', 'uint16_t', 16),
        ('first_entry_node_index', 'uint32_t', 32),
        ('exit_node_count', 'uint16_t', 16),
        ('first_exit_node_index', 'uint32_t', 32)]

entry_node = [
        ('entry_edge_front_count', 'uint16_t', 16),
        ('first_entry_edge_front_index', 'uint32_t', 32),
        ('entry_edge_reverse_count', 'uint16_t', 16),
        ('first_entry_edge_reverse_index', 'uint32_t', 32),
        ('exit_edge_reverse_index', 'uint32_t', 32),
        ('cluster_index', 'uint32_t', 32)]

exit_node = [
        ('entry_edge_front_count', 'uint16_t', 16),
        ('first_entry_edge_front_index', 'uint32_t', 32),
        ('exit_edge_front_count', 'uint16_t', 16),
        ('first_exit_edge_front_index', 'uint32_t', 32),
        ('exit_edge_reverse_index', 'uint32_t', 32),
        ('cluster_index', 'uint32_t', 32)]

entry_edge_front = [('entry_node_index', 'uint32_t', 32)]
entry_edge_reverse = [('entry_node_index', 'uint32_t', 32)]
exit_edge_front = [('exit_node_index', 'uint32_t', 32)]
exit_edge_reverse = [('exit_node_index', 'uint32_t', 32)]

recursive_circuit = [('cluster', 'clusters'),
                     ('entry_node', 'entry_nodes'),
                     ('exit_node', 'exit_nodes'),
                     ('entry_edge_front', 'entry_edges_front'),
                     ('entry_edge_reverse', 'entry_edges_reverse'),
                     ('exit_edge_front', 'exit_edges_front'),
                     ('exit_edge_reverse', 'exit_edges_reverse')]

def convertToPascal(i):
    return "".join(x for x in i.replace("_", " ").title() if not x.isspace())

def genObjTypeSignature(obj_i):
    obj = globals()[obj_i]
    signature = f"{convertToPascal(i)}<"
    total_bits = 0
    for j in range(len(obj)):
        obj_field = obj[j]
        obj_field_type = obj_field[1]
        obj_field_bit_count = obj_field[2]
        total_bits += obj_field_bit_count
        if j == len(obj) - 1:
            signature += f"{obj_field_type}"
        else:
            signature += f"{obj_field_type}, "
    signature += f">"
    total_bytes = math.ceil(total_bits / 8.0)
    return signature, total_bytes

with open("tmp.cpp", 'w') as f:
    f.write(f"#ifndef __RECURSIVE_CIRCUIT02_HPP__\n")
    f.write(f"#define __RECURSIVE_CIRCUIT02_HPP__\n\n")
    f.write(f"#include \"standard_defs/standard_defs.hpp\"\n\n")
    f.write(f"namespace RecursiveCircuit01 {{\n\n")
    for i, i_arr in recursive_circuit:
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
            assert(int(obj_field_bit_count) <= 32)
            f.write(f"assert({obj_field_name} < (1llu << {obj_field_bit_count}));\n")
            f.write(f"_{obj_field_name} = {obj_field_name};\n\n")
        f.write(f"}};\n\n")
        f.write(f"}};\n")
        signature, total_size_in_bytes = genObjTypeSignature(i)
        f.write(f"static_assert(sizeof({signature}) == {total_size_in_bytes});\n\n") 
    f.write(f"}};\n\n")
    f.write(f"#endif // __RECURSIVE_CIRCUIT02_HPP__\n")
    f.write("\n\n\n")

    f.write(f"namespace RecursiveCircuit01 {{\n")
    for i, i_arr in recursive_circuit:
        signature, total_size_in_bytes = genObjTypeSignature(i)
        f.write(f"{signature} *{i_arr} = nullptr;\n")
    f.write("\n")
    for i, i_arr in recursive_circuit:
        signature, total_size_in_bytes = genObjTypeSignature(i)
        f.write(f"void allocate_{i_arr}(size_t {i}_count) {{\n")
        f.write(f"  {i_arr} = static_cast<{signature} *>(")
        f.write(f"malloc(sizeof({signature}) * {i}_count));")
        f.write("}\n\n")
    f.write(f"}};\n\n")
