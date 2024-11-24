import mmap
import time
import struct

def read_edge_list(file_path):
    with open(file_path, 'rb') as f:
        mm = mmap.mmap(f.fileno(), 0, access=mmap.ACCESS_READ)
        edges = set()
        i = 0
        start = time.time()
        while True:
            i += 1
            if i % 10000000 == 0:
                print("Read {} lines, {}s".format(i, time.time() - start))
            line = mm.read(8)
            if not line:
                break
            src, dst = struct.unpack('II', line)
            edges.add((src, dst))
        print("Read {} lines, {}s".format(i, time.time() - start))
    return edges

def kcore(edges, k):
    start = time.time()
    iter = 0
    degrees = {}
    for src, dst in edges:
        if src not in degrees:
            degrees[src] = 0
        if dst not in degrees:
            degrees[dst] = 0
        degrees[src] += 1
        degrees[dst] += 1
    print("Initialization finished, {}s".format(time.time() - start))
    print("Total {} nodes".format(len(degrees)))
    while True:
        print("Iteration {}, {}s".format(iter, time.time() - start))
        iter += 1
        deleted = set()
        deleted_nodes = set()
        for src, dst in edges:
            if degrees[src] < k:
                deleted.add((src, dst))
                deleted_nodes.add(src)
            if degrees[dst] < k:
                deleted.add((src, dst))
                deleted_nodes.add(dst)

        for src, dst in deleted:
            degrees[src] -= 1
            degrees[dst] -= 1
        print("Deleted {} edges".format(len(deleted)))
        print("Deleted {} nodes".format(len(deleted_nodes)))
                
        edges -= deleted
        for node in deleted_nodes:
            del degrees[node]
        additional_deleted = set()
        for node in degrees:
            if degrees[node] == 0:
                additional_deleted.add(node)
        for node in additional_deleted:
            del degrees[node]
        print("Deleted {} additional nodes".format(len(additional_deleted)))
        print("Remaining {} nodes".format(len(degrees)))
        if not deleted:
            break
    return set([src for src, _ in edges] + [dst for _, dst in edges])
        

edges = read_edge_list('hw9_data/livejournal')
kcore_graph = kcore(edges, 100)
print("K-core graph has {} nodes".format(len(kcore_graph)))