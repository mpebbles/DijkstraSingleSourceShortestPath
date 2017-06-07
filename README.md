# DijkstraSingleSourceShortestPathC
Program takes in a file with the first line being the number of vertices in the graph, and the following lines being in the form '(int) source_vertex     (int) destination_vertex    (double) weight'.

Usage: ./program <source-vertex> <input-file>
input-file can be stdin, denoted as '-'.

Output:
  For each vertex in graph -
       status - in the tree (discovered) -'t'
       cost - smallest cost to get to vertex from source-vertex (oo denotes no route)
       parent - parent vertex
       
