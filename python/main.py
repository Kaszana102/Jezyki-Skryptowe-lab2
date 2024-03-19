import pybind11

from  simple_graphs import *
from graphs import *
graf  = AdjacencyMatrix("DQc")
a=1

print("number_of_vertices: " + str(graf.number_of_vertices() == 5))
print("vertices: " + str(graf.vertices() == {0,1,2,3,4}))
print("vertex_degree: " + str(graf.vertex_degree(4) == 2))
print("vertex_degree: " + str(graf.vertex_degree(5) == 0))
print("vertex_neighbors: " + str(graf.vertex_neighbors(4) == {0,3}))
graf.add_vertex(15)
print("add_vertex: " + (str(True) if (graf.vertices() == {0,1,2,3,4,15}) else ('returned: ' + str(graf.vertices()))))
graf.delete_vertex(15)
print("vertices: " + str(graf.vertices() == {0,1,2,3,4}))

print("number_of_edges: " + str(graf.number_of_edges() == 4) + ", counted: " + str(graf.number_of_edges()))
print("edges: " + str(graf.edges() == {(1,3),(3,4),(0,4),(0,2)}))
print("is_edge: " + str(graf.is_edge(0,2) == True))
print("is_edge: " + str(graf.is_edge(0,1) == False))
graf.delete_edge(0,2)
print("delete_edge: " + str(graf.is_edge(0,2) == False))
graf.add_edge(0,3)
print("add_edge: " + str(graf.is_edge(0,3) == True))

graf2 = Graph("DQc")
graf2.add_vertex(2)
#print(graf2.edges())

graf = AdjacencyMatrix.create_wheel(5)
a=2


