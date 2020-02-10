# omegah_examples
examples for getting started with omega_h


# build

The following instructions apply to the SCOREC RedHat 7 systems with NVIDIA
GPUs.  Specific instructions are given for Pascal GPUs (e.g., in the `blockade` and
`pachisi` systems).

## dependencies

First build the dependencies for execution with the Kokkos backend using CUDA.

Kokkos:
https://github.com/SCOREC/particle_structures/wiki/Building-and-Running-on-SCOREC-RHEL7#kokkos

Omega_h:
https://github.com/SCOREC/particle_structures/wiki/Building-and-Running-on-SCOREC-RHEL7#omega_h

## omegah_examples

Setup the environment.

```
module load cuda/10.1 gcc mpich cmake 
# use the environment variable set in the omega_h build
export CMAKE_PREFIX_PATH=$oh:$CMAKE_PREFIX_PATH
export MPICH_CXX=$kksrc/bin/nvcc_wrapper
```

Clone the repo, run cmake, then build.

```
git clone git@github.com:SCOREC/omegah_examples.git
mkdir build-ohExamples-pascal
cd !$
cmake ../omegah_examples -DCMAKE_CXX_COMPILER=mpicxx
make
```

Run `ctest` to run the examples:

```
ctest
```

# add another example

Create a new source file from an existing one:

```
cd omegah_examples
cp firstOrderAdjacency.cpp foo.cpp
```

Add the new file to the build and test system cmake files:

`testing.cmake` - execution commands for examples, add `mpi_test(foo_2D 1 ./foo ${CMAKE_SOURCE_DIR}/meshes/tri8.osh)`
to start with, then modify as the new example is implemented

`CMakeLists.txt` - defines compile and link commands for examples; add `example(foo foo.cpp)`

Ensure that the new example compiles and runs:

```
# setup the environment as described above
cd build-ohExamples-pascal
make
ctest
```
## Terminology
- Topological Complex - A breakdown of a Cartesian domain into topological entities
- Mesh - A topological complex whose entities have simple shape
- Entity - A topological entity of a mesh
- Vertex - A 0-dimensional entity
- Edge - A 1-dimensional entity
- Face - A 2-dimensional entity
- Region - A 3-dimensional entity
- Element - An entity not bounding another entity
- Mesh discretization - Subdivision of a continuous geometric space into discrete geometric and topological cells
- Model(CAD) - A topological complex consisting of vertices, edges, faces and regions from which a mesh is generated via mesh discretization
- Adjacency - If a point set b bounds a point set a, then there is a downward adjacency (a, b) an upward adjacency (b, a)
- Barycentric coordinate system - A coordinate system in which the location of a point of a simplex (a triangle, tetrahedron, etc.) is specified as the center of mass, or barycenter, of usually unequal masses placed of the coordinates of its vertices (from Wikipedia)
- Classification - A direct mapping from mesh entities to geometric (CAD) model entities

## Outline
We aim to develop functions or tests which will aid in mesh operations and other common related tasks, using the functionalities and API of Omega_h. Tests are developed for adjacency queries, modifying data related to mesh entities, calculation of barycentric coordinates, reverse classification and data reduction in parallel. More tests focusing on parallel execution like synchronizing data on process boundary, mesh-repartitioning and creating ghost elements are in the pipeline. More details can be found below.

## Summary
Following tests, which require the mesh file as an input, are developed:
- adjacencies - Given a mesh, query the first order downward (edge to vertex, face to edge, element to face) and upward (vertex to edge, edge to face and face to element), and second order (vertex to vertex via edge) adjacencies.

- tags - Given a mesh, create, modify, and delete data associated with mesh entities and write mesh files for visualization

- interpolation - Given a mesh, a function is written to compute barycentric coordinates. The calculations are tested for two cases, firstly for the centroid and secondly a vertex of the element, for all the elements.

- classification - For a given mesh and a particular CAD model face, find all mesh elements with at least one face classified on it.

parallel:
- reduction - Given a partitioned mesh, compute the maximum value of a tag across all processes

- tag synchronization - synchronize a vertex tag such that owners of vertices on the part boundary send their values to non-owned copies on other processes
***More to follow***

- partitioning - define weights associated with mesh elements and repartition(balance) the mesh according to those weights
***More to follow***

- ghosting - create a single layer of ghost elements with mesh vertices (in 3D) defined as the bridge entity
***More to follow***

## Details about the tests (assertion, results, new APIs, objects and concepts used) 

- adjacencies - The user input is the mesh file. By using 'get_adj', we can get the downward adjacencies and 'ask_up' for upward adjacencies.  'ask_star' is used to query second order vertex to vertex adjacency. Furthermore in this test, the function 'dim()' is used to get dimensions (2D/3D), 'nglobal_ents(arg)' returns the global number of entities of 'arg' dimension, similar to 'nverts()', 'nfaces()', etc. For testing the queries, assertions are used to compare the size of the list array returned by the adjacency query, to the expected size. For instance, in a edge to vertex downward adjacency query, since each edge has two vertices the size of the list array which stores the vertices for each edge should be twice the total number of edges. When we query the upward adjacency vertex to edge, the size of the list array will also be twice the number of edges because of the reverse of graph from edge to vertex. That is, each edge will be counted twice from each vertex. Similar argument is made for the assertion implemented for the second order vertex to vertex adjacency query.

- tags - User input is the mesh file. 'add_tag' is used to create a new variable or tag which is associated with each of the specified mesh entity. The values of this tag are specified using 'set_tag' and the information about the tags can be deleted by 'remove_tag'. 'Read' and 'Write' array structure of Omega_h is used for setting the values for the tag. The mesh file is written to visualize the results by using 'binary::write'. This writes a .osh file which needs to be converted to .vtk for visualization.

- interpolation - This test is implemented for a 3D mesh only. The 'get_barycentric' function is called from the main function which returns barycentric coordinates, given the mesh, the element ID with respect to which the calculations are to be performed, and the point coordinates for which barycentric coordinates are to be calculated. This function makes use of 'coords()' to return the coordinates of all vertices, 'parallel_for' looping structure to access and work on the data existing on GPU memory, 'vector_3' to convert values to three dimensional vector, 'get_triangle_normal' to calculate normal vectors of all four faces of the tetrahedron and normalize them with 'normalize', as well as some more vector operators. This reference is used for method of calculation "https://people.sc.fsu.edu/~jburkardt/presentations/cg_lab_barycentric_tetrahedrons.pdf". The 'ask_elem_verts' returns vertex ids for all elements. The calculations in the function are structured such that the face normals point outside the tetrahedron. For this, the vertex ordering information is used from here "https://github.com/SCOREC/pumi-pic/wiki/Omega_h-Simplex". As we have implemented the test for centroid and vertex of the element, assertions on the return value of the function are used to compare with expected values.

- classification - This test can also be called as reverse classification. A function is written which returns a flag representing whether or not a cell of a given mesh is classified on the model entity having certain ID and dimension. The 'mark_by_class' returns all vertices classified by a particular model entity and the corresponding elements are flagged using upward adjacency.

- reduction - Each process will load one part of the mesh. The 'get_max' will return either local or global max of input argumentarray supplied. The performance of this code should be tested for tags involved with global values like vertex coordinates. An assertion involving local parameters like mesh entity IDs might not work.

- synchronization - 'sync_array' is being studied
***More to follow***
- partitioning - ***More to follow***
- ghosting - ***More to follow***
