# omegah_examples
examples for getting started with omega_h


# build

The following instructions apply to the SCOREC RedHat 7 systems with NVIDIA
GPUs.  Specific instructions are given for Pascal GPUs (e.g., in the `blockade` and
`pachisi` systems).

## dependencies

First build the dependencies for execution with the Kokkos backend using CUDA.

Kokkos:
https://github.com/SCOREC/pumi-pic/wiki/Building-and-Running-on-SCOREC-RHEL7#kokkos

Omega_h:
https://github.com/SCOREC/pumi-pic/wiki/Building-and-Running-on-SCOREC-RHEL7#omega_h

Alternatively, for debugging purposes you can build Omega_h in serial without kokkos:

https://github.com/SCOREC/pumi-pic/wiki/Building-and-Running-on-SCOREC-RHEL7#building-omega_h-in-serial-for-debugging

Note, you'll still need to set `export oh=/path/to/omega_h/install` accordingly for the `omegah_examples` build.

## omegah_examples - cuda build

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

## omegah_examples - serial build

Setup the environment.

```
module load gcc cmake 
# use the environment variable set in the omega_h build
export CMAKE_PREFIX_PATH=$oh:$CMAKE_PREFIX_PATH
```

Clone the repo, run cmake, then build.

```
git clone git@github.com:SCOREC/omegah_examples.git
mkdir build-ohExamples-serial
cd !$
cmake ../omegah_examples -DCMAKE_CXX_COMPILER=g++
make
```

Run `ctest` to run the examples:

```
ctest
```

Note, any example using MPI will not compile. 


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

# Example Descriptions

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

- synchronization - synchronize a vertex tag such that owners of vertices on the part boundary send their values to non-owned copies on other processes

- partitioning - define weights associated with mesh elements and repartition(balance) the mesh according to those weights

- ghosting - create a single layer of ghost elements with mesh vertices (in 3D) defined as the bridge entity

## Host vs. Device Arrays

Assuming Omega\_h was built with CUDA or Kokkos, then all `Write` and `Read`
array objects are created in device (aka GPU) memory and can only be read and modified
through device kernels (`parallel_for`, `parallel_reduce`, etc.).  The
`HostWrite` and `HostRead` objects support creating arrays in host (aka CPU)
memory.  One of the `Write` constructors in `Omega_h_array.hpp` supports
conversion from a `HostWrite`.  Similarly, `HostRead` (`HostWrite`) has a constructor for
conversion from a `Read` (`Write`). Note, there is no `Read` constructor that
supports conversion from a `HostRead`.

## Details about the tests (assertion, results, new APIs, objects and concepts used) 

- adjacencies - The user input is the mesh file. By using 'get_adj' from the host, we can get the downward adjacencies and 'ask_up' can be called from host for upward adjacencies.  'ask_star' can be called from host to query second order vertex to vertex adjacency. Furthermore in this test, the function 'dim()' can be called from host to get dimensions (2D/3D) of mesh, 'nglobal_ents(arg)' can be called from host to get the global number of entities of 'arg' dimension, similar to 'nverts()', 'nfaces()', etc. For testing the queries, assertions are used to compare the size of the list array returned by the adjacency query, to the expected size. For instance, in a edge to vertex downward adjacency query, since each edge has two vertices the size of the list array which stores the vertices for each edge should be twice the total number of edges. When we query the upward adjacency vertex to edge, the size of the list array will also be twice the number of edges because of the reverse of graph from edge to vertex since all the edges are bounded by two vertices. Similar argument is made for the assertion implemented for the second order vertex to vertex adjacency query. Each omega_h function/api/class used can be called from the host (outside a kernel) or on the device (within a kernel, e.g., in the body of a parallel_for).  Also the host functions operate on large arrays of data, typically all entities of a specified dimension stored on the local process while device functions operate on small arrays of data, typically associated with a single mesh entity

- tags - User input is the mesh file. 'Read' and 'Write' array structure of Omega_h called from host is used for setting the values for the tag. 'Write' can be called from host to declare and/or initialize arrays of integer(LO) or double(Real) type. For example the 'Write' given below creates array 'gravityArray' of type Real(double) of size 'nvert' and initializes to '9.81'. The 'Read' converts this same array to a read-only 'gravityArray_r'. 'add_tag' can be called from host to create a new variable or tag which is associated with each of the mesh entities(all vertices, all edges,etc.) associated with a dimension-specified via arguments. The values of this tag are specified by calling 'set_tag' from the host which will take an input argument of an array consisting of all the values to be assigned to all the entities for which that particular tag exists. The information about the tags can be deleted by calling 'remove_tag' from the host. The mesh file is written to visualize the results by calling 'binary::write' from the host. This writes a .osh file which needs to be converted to .vtk for visualization.

- interpolation - This test is implemented for a 3D mesh only. The 'get_barycentric' function is called from the main function which returns barycentric coordinates given the mesh, the element ID with respect to which the calculations are to be performed, and the point coordinates for which barycentric coordinates are to be calculated. This function makes use of 'coords()' which is called from host to return the coordinates of all vertices. Since all data exists on GPU memory and thus can be accessed only from the device, 'parallel_for' loop kernel is used to access and work on the data existing on GPU memory. 'vector_3' is used from device for converting values to a three dimensional vector, 'get_triangle_normal' is called from device to calculate normal vectors of all four faces of the tetrahedron and normalize them by calling 'normalize' from device, as well as some more vector operators. This reference is used for method of calculation "https://people.sc.fsu.edu/~jburkardt/presentations/cg_lab_barycentric_tetrahedrons.pdf". The 'ask_elem_verts' is called from host to return vertex ids for all elements. The calculations in the function are structured such that the face normals point outside the tetrahedron. For this, the vertex ordering information shown in figure is used to while creating face normals. As we have implemented the test for centroid and first vertex of the element as per the ordering, assertions on the return value of the function are used to compare results with expected values.

![ordering_pumi](https://user-images.githubusercontent.com/56453280/74203616-39d27a80-4c3e-11ea-885d-b0260490e184.png)

- classification - This test can also be called as reverse classification. A function is written which returns a flag representing whether or not a cell of a given mesh is classified on the model entity having certain ID and dimension. The 'mark_by_class' is called from host which returns all vertices classified by a particular model entity and the corresponding elements are flagged using upward adjacency relations.

- reduction - Each process will load one part of the mesh. The 'get_max' function is called from host which will return either global max of input argument array.

- synchronization - 'sync_tag' can be called from host to synchronize the values of tags, for the entities lying on process boundry. 'vtk::write_parallel' can be called to write mesh for parallel code.

- partitioning -  'balance' can be called from host to partition the mesh as per RIB. For predictive load balancing, mesh 'metric' tag has to be associated with vertices and then 'balance(1)' can be used.

- ghosting - 'set_parting' can be called from host with the required number layers of ghost vertices as an argument
