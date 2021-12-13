from graph_creator import graph_creator
from tqdm import tqdm

NODE_NUM_INTERVAL = 20
MAX_NODE_NUM = 500
DIRECTORY = "bench_graphs/"
CSV_FILE = "in.csv"
REPETITION = 5
POPULATION = 20
NODE_NUM_FOR_EDGE_TEST = 100
EDGE_INTERVAL = 50
COLOR_INTERVAL = 20

if __name__ == "__main__":
    gc = graph_creator()
    identificator = 0

    # generate sparse graphs
    print("sparse graph test start index = {}".format(identificator))
    for node_num in tqdm(range(NODE_NUM_INTERVAL, MAX_NODE_NUM, NODE_NUM_INTERVAL)):
        file = "{}sparse_{}.dot".format(DIRECTORY, node_num)
        graph_creator.write_data(gc.generate_graph(node_num, 0, True), file)
        with open(CSV_FILE, "a") as f:
            f.write("{},{},{},{},{},{},{}\n".format(identificator, "g", file, "x", gc.min_colors_to_use + 1, 0, REPETITION))
            identificator += 1
            f.write("{},{},{},{},{},{},{}\n".format(identificator, "e", file, "x", gc.min_colors_to_use + 1, POPULATION, REPETITION))
            identificator += 1
            f.write("{},{},{},{},{},{},{}\n".format(identificator, "h", file, "x", gc.min_colors_to_use + 1, POPULATION, REPETITION))
            identificator += 1
    print("sparse graph test end index = {}".format(identificator - 1))

    # generate dense graphs
    print("dense graph test start index = {}".format(identificator))
    for node_num in tqdm(range(NODE_NUM_INTERVAL, MAX_NODE_NUM, NODE_NUM_INTERVAL)):
        file = "{}dense_{}.dot".format(DIRECTORY, node_num)
        graph_creator.write_data(gc.generate_graph(node_num, 0, False), file)
        with open(CSV_FILE, "a") as f:
            f.write("{},{},{},{},{},{},{}\n".format(identificator, "g", file, "x", gc.min_colors_to_use + 1, 0, REPETITION))
            identificator += 1
            f.write("{},{},{},{},{},{},{}\n".format(identificator, "e", file, "x", gc.min_colors_to_use + 1, POPULATION, REPETITION))
            identificator += 1
            f.write("{},{},{},{},{},{},{}\n".format(identificator, "h", file, "x", gc.min_colors_to_use + 1, POPULATION, REPETITION))
            identificator += 1
    print("dense graph test end index = {}".format(identificator - 1))

    print("constraints test start index = {}".format(identificator))
    # generate sparse graphs with constraints (sparse because dense takes too much time)
    for node_num in tqdm(range(NODE_NUM_INTERVAL, MAX_NODE_NUM, NODE_NUM_INTERVAL)):
        constraints_num = (node_num * (node_num - 1))
        graph_file = "{}graph_with_constraints_{}.dot".format(DIRECTORY, node_num)
        constraint_file = "{}constraints_{}.dot".format(DIRECTORY, node_num)
        graph_creator.write_data(gc.generate_graph(node_num, 0, True), graph_file)
        graph_creator.write_data(gc.generate_constraints(node_num, constraints_num, 0, True, 0), constraint_file)
        with open(CSV_FILE, "a") as f:
            f.write("{},{},{},{},{},{},{}\n".format(identificator, "g", graph_file, constraint_file, gc.min_colors_to_use + 1, 0, REPETITION))
            identificator += 1
            f.write("{},{},{},{},{},{},{}\n".format(identificator, "e", graph_file, constraint_file, gc.min_colors_to_use + 1, POPULATION, REPETITION))
            identificator += 1
            f.write("{},{},{},{},{},{},{}\n".format(identificator, "h", graph_file, constraint_file, gc.min_colors_to_use + 1, POPULATION, REPETITION))
            identificator += 1
    print("constraints test end index = {}".format(identificator - 1))
    
    # generate one graph with different amount of edges
    print("edge test start index = {}".format(identificator))
    min_edges = NODE_NUM_FOR_EDGE_TEST - 1
    max_edges = (NODE_NUM_FOR_EDGE_TEST * (NODE_NUM_FOR_EDGE_TEST - 1)) / 2
    for edge_num in tqdm(range(min_edges, max_edges, EDGE_INTERVAL)):
        graph_file = "{}graph_edges_{}.dot".format(DIRECTORY, edge_num)
        graph_creator.write_data(gc.generate_graph(node_num, edge_num), graph_file)
        with open(CSV_FILE, "a") as f:
            f.write("{},{},{},{},{},{},{}\n".format(identificator, "g", graph_file, "x", gc.min_colors_to_use + 1, 0, REPETITION))
            identificator += 1
            f.write("{},{},{},{},{},{},{}\n".format(identificator, "e", graph_file, "x", gc.min_colors_to_use + 1, POPULATION, REPETITION))
            identificator += 1
            f.write("{},{},{},{},{},{},{}\n".format(identificator, "h", graph_file, "x", gc.min_colors_to_use + 1, POPULATION, REPETITION))
            identificator += 1
    print("edge test end index = {}".format(identificator - 1))

    # sparse graph color test
    print("sparse color start index = {}".format(identificator))
    file = "{}sparse_color.dot".format(DIRECTORY)
    graph_creator.write_data(gc.generate_graph(MAX_NODE_NUM, 0, True), file)
    for color_num in tqdm(range(gc.min_colors_to_use + 1, MAX_NODE_NUM, COLOR_INTERVAL)):
        with open(CSV_FILE, "a") as f:
            f.write("{},{},{},{},{},{},{}\n".format(identificator, "g", file, "x", color_num, 0, REPETITION))
            identificator += 1
            f.write("{},{},{},{},{},{},{}\n".format(identificator, "e", file, "x", color_num, 0, REPETITION))
            identificator += 1
            f.write("{},{},{},{},{},{},{}\n".format(identificator, "h", file, "x", color_num, 0, REPETITION))
            identificator += 1
    print("sparse color end index = {}".format(identificator - 1))

    # dense graph color test
    print("dense color start index = {}".format(identificator))
    file = "{}dense_color.dot".format(DIRECTORY)
    graph_creator.write_data(gc.generate_graph(MAX_NODE_NUM, 0, False), file)
    for color_num in tqdm(range(gc.min_colors_to_use + 1, MAX_NODE_NUM, COLOR_INTERVAL)):
        with open(CSV_FILE, "a") as f:
            f.write("{},{},{},{},{},{},{}\n".format(identificator, "g", file, "x", color_num, 0, REPETITION))
            identificator += 1
            f.write("{},{},{},{},{},{},{}\n".format(identificator, "e", file, "x", color_num, 0, REPETITION))
            identificator += 1
            f.write("{},{},{},{},{},{},{}\n".format(identificator, "h", file, "x", color_num, 0, REPETITION))
            identificator += 1
    print("dense color end index = {}".format(identificator - 1))

    print("force collisions constraints test start index = {}".format(identificator))
    # generate sparse graphs with constraints (sparse because dense takes too much time)
    for node_num in tqdm(range(NODE_NUM_INTERVAL, MAX_NODE_NUM, NODE_NUM_INTERVAL)):
        constraints_num = (node_num * (node_num - 1))
        graph_file = "{}graph_with_forced_collisions_{}.dot".format(DIRECTORY, node_num)
        constraint_file = "{}constraints_collisions_{}.dot".format(DIRECTORY, node_num)
        graph_creator.write_data(gc.generate_graph(node_num, 0, True), graph_file)
        graph_creator.write_data(gc.generate_constraints(node_num, constraints_num, 0, True, int(constraints_num/8)), constraint_file)
        with open(CSV_FILE, "a") as f:
            f.write("{},{},{},{},{},{},{}\n".format(identificator, "g", graph_file, constraint_file, gc.min_colors_to_use + 1, 0, REPETITION))
            identificator += 1
            f.write("{},{},{},{},{},{},{}\n".format(identificator, "e", graph_file, constraint_file, gc.min_colors_to_use + 1, POPULATION, REPETITION))
            identificator += 1
            f.write("{},{},{},{},{},{},{}\n".format(identificator, "h", graph_file, constraint_file, gc.min_colors_to_use + 1, POPULATION, REPETITION))
            identificator += 1
    print("constraints test end index = {}".format(identificator - 1))
