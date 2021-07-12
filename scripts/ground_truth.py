#! /usr/bin/env python
# encoding: utf-8

import numpy as np
import sys
import trimesh
import networkx as nx


mesh = trimesh.load_mesh(sys.argv[1])

graph = trimesh.graph.vertex_adjacency_graph(mesh)

geodesics = nx.shortest_path_length(graph, source=0, weight='weight')

N = len(mesh.vertices)

ground_truth = np.zeros((N))
period = 2*np.pi / 0.3 * 2

for i in range(N):
    ground_truth[i] = 2 * np.sin(geodesics.get(i) * period + 0.3)

np.savetxt("rsc/data/ground_truth.csv", ground_truth)

mesh.show()
