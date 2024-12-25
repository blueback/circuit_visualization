#!/usr/bin/python3

cluster = [
        "entry_node_count",
        "first_entry_node_index",
        "exit_node_count",
        "first_exit_node_index"]

entry_node = [
        "entry_edge_front_count",
        "first_entry_edge_front_index",
        "entry_edge_reverse_count",
        "first_entry_edge_reverse_index",
        "exit_edge_reverse_index",
        "cluster_index"]

exit_node = [
        "entry_edge_front_count",
        "first_entry_edge_front_index",
        "exit_edge_front_count",
        "first_exit_edge_front_index",
        "exit_edge_reverse_index",
        "cluster_index"]

entry_edge_front = ["entry_node_index"]
entry_edge_reverse = ["entry_node_index"]
exit_edge_front = ["exit_node_index"]
exit_edge_reverse = ["exit_node_index"]

recursive_circuit = ["cluster",
                     "entry_node",
                     "exit_node",
                     "entry_edge_front",
                     "entry_edge_reverse",
                     "exit_edge_front",
                     "exit_edge_reverse"]
def convertToPascal(i):
    return "".join(x for x in i.replace("_", " ").title() if not x.isspace())

with open("tmp.cpp", 'w') as f:
    for i in recursive_circuit:
        f.write(f"template<")
        for j in range(len(globals()[i])):
            if j == len(globals()[i]) - 1:
                f.write(f"typename TYPE_{globals()[i][j].upper()}")
            else:
                f.write(f"typename TYPE_{globals()[i][j].upper()} ,")
        f.write(f">")
        f.write(f"class TRY_PACKED {convertToPascal(i)} {{\n")
        f.write(f"public:\n")
        for j in globals()[i]:
            f.write(f"TYPE_{j.upper()} _{j};\n")
        f.write(f"\nTRY_NOMAGIC({convertToPascal(i)})\n\n")
        f.write(f"TRY_INLINE explicit {convertToPascal(i)}(")
        for j in range(len(globals()[i])):
            if j == len(globals()[i]) - 1:
                f.write(f"const TYPE_{globals()[i][j].upper()} {globals()[i][j]}) {{\n")
            else:
                f.write(f"const TYPE_{globals()[i][j].upper()} {globals()[i][j]},\n")
        for j in globals()[i]:
            f.write(f"_{j} = {j};\n")
        f.write(f"}};\n\n")
        f.write(f"}};\n")
        f.write(f"static_assert(sizeof({convertToPascal(i)}<uint32_t>) == 4);\n\n") 


