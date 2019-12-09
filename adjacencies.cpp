#include <Omega_h_file.hpp>
#include <Omega_h_library.hpp>
#include <Omega_h_mesh.hpp>
#include <Omega_h_overlay.hpp>
#include <iostream>
#include <string>
using namespace	std;

int main(int argc, char** argv) {
  auto lib = Omega_h::Library(&argc, &argv);
  if(argc!=2) {
    fprintf(stderr, "Usage: %s <input mesh>\n", argv[0]);
    return 0;
  }
  const auto rank = lib.world()->rank();
  const auto inmesh = argv[1];
  Omega_h::Mesh mesh(&lib);
  Omega_h::binary::read(inmesh, lib.world(), &mesh);
  const auto dim = mesh.dim();

  //-- upward adjacencies --//
  //get array of vertices to edges
  if (!rank) {
    int i;
    for (i = 0; i < mesh.nglobal_ents(1); i++) {   // for each edge
      auto verts = overlay.get_edge_nodes(i);      // get the respective
      cout << verts[0] << "," << verts[1] << endl; // vertices
    }
  }
/*    array<size_t 2> nodes;

    auto edge2node = mesh.get_adj(1,0).ab2b;
    for (size_t i = 0; i < 2; i++) {
      nodes[i] = size_t(edge2node[LO(edge * 2 + i)]);
      cout << nodes[i];
    }*/
  }
//    fprintf(stderr, "vertex to edge %f \n",
//	mesh.get_adj(1,0));
/*	std::ostream& operator<<(std::ostream& out, const Omega_h::Adj &ad)
	{
    		out << ad.getName(); // for example
		return out;
	}
*/

/*  //get array of edges to faces

  //get array of faces to regions (if 3D mesh)

  //-- downward adjacencies --//
  //get array of regions to faces (if 3D mesh)
  //get array of faces to edges

  //get array of edges to vertices
*/
  return 0;
}
