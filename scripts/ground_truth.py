#!/usr/bin/env python
# encoding: utf-8
#
#    This file is part of graphics-lib.
#
#    Copyright (c) 2020, 2021, 2022 Bernardo Fichera <bernardo.fichera@gmail.com>
#
#    Permission is hereby granted, free of charge, to any person obtaining a copy
#    of this software and associated documentation files (the "Software"), to deal
#    in the Software without restriction, including without limitation the rights
#    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#    copies of the Software, and to permit persons to whom the Software is
#    furnished to do so, subject to the following conditions:
#
#    The above copyright notice and this permission notice shall be included in all
#    copies or substantial portions of the Software.
#
#    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#    SOFTWARE.

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
