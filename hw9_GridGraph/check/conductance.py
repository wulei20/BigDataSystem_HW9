import mmap
import time
import struct

def read_edge_list(file_path):
    with open(file_path, 'rb') as f:
        mm = mmap.mmap(f.fileno(), 0, access=mmap.ACCESS_READ)
        graph = {}
        i = 0
        start = time.time()
        while True:
            i += 1
            if i % 10000000 == 0:
                print("Read {} lines, {}s".format(i, time.time() - start))
            line = mm.read(8)
            if not line:
                break
            src, dst = struct.unpack('ii', line)
            if src not in graph:
                graph[src] = []
            graph[src].append(dst)
        return graph

def conductance(graph):
    red_num = 0
    black_num = 0
    cross_num = 0
    for node in graph:
        src_type = node % 2
        for neighbor in graph[node]:
            dst_type = neighbor % 2
            if src_type != dst_type:
                cross_num += 1
            if src_type == 0:
                red_num += 1
            else:
                black_num += 1
    print("Red nodes: {}, black nodes: {}, cross edges: {}".format(red_num, black_num, cross_num))
    return cross_num / min(red_num, black_num)

def read_weighted_edge_list(file_path):
    with open(file_path, 'rb') as f:
        mm = mmap.mmap(f.fileno(), 0, access=mmap.ACCESS_READ)
        graph = {}
        i = 0
        start = time.time()
        while True:
            i += 1
            if i % 10000000 == 0:
                print("Read {} lines, {}s".format(i, time.time() - start))
            line = mm.read(12)
            if not line:
                break
            src, dst, weight = struct.unpack('iif', line)
            if src not in graph:
                graph[src] = []
            graph[src].append((dst, weight))
        return graph
    
def conductance_weighted(graph):
    red_num = 0
    black_num = 0
    cross_num = 0
    red_weight = 0
    black_weight = 0
    cross_weight = 0
    for node in graph:
        src_type = node % 2
        for neighbor, weight in graph[node]:
            dst_type = neighbor % 2
            if src_type != dst_type:
                cross_num += 1
                cross_weight += weight
            if src_type == 0:
                red_num += 1
                red_weight += weight
            else:
                black_num += 1
                black_weight += weight
    print("Red nodes: {}, black nodes: {}, cross edges: {}".format(red_num, black_num, cross_num))
    print("Red weight: {}, black weight: {}, cross weight: {}".format(red_weight, black_weight, cross_weight))
    return cross_num / min(red_num, black_num), cross_weight / min(red_weight, black_weight)

# graph = read_edge_list('hw9_data/livejournal')
# start = time.time()
# conductance_val = conductance(graph)
# print("Time: {}s".format(time.time() - start))
# print("Conductance is {}".format(conductance_val))

# test_graph = {0: [1, 5], 1: [0, 8], 2: [0, 7], 3: [0, 6], 4: [0, 5], 5: [0, 4], 6: [0, 3], 7: [0, 2], 8: [0, 1]}
# with open('graph/test', 'wb') as f:
#     for src in test_graph:
#         f.write(struct.pack('iif', src, test_graph[src][0], test_graph[src][1]))
graph = read_weighted_edge_list('graph/livejournal_weighted')
start = time.time()
conductance_val, conductance_weighted_val = conductance_weighted(graph)
print("Time: {}s".format(time.time() - start))
print("Conductance is {}, Conductance weighted is {}".format(conductance_val, conductance_weighted_val))
# with open('hw9_GridGraph/output.txt', 'r') as f:
#     lines = f.readlines()
#     total_weight = 0
#     for line in lines:
#         src, dst, weight = line.strip().split()
#         total_weight += float(weight)
#     print("Total weight is {}".format(total_weight))