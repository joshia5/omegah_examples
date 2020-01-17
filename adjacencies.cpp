#include <Omega_h_file.hpp>
#include <Omega_h_library.hpp>
#include <Omega_h_mesh.hpp>
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

  if(!rank) {
    fprintf(stderr, "mesh <v e f r> %d %d %d %d\n",
        mesh.nglobal_ents(0),
        mesh.nglobal_ents(1),
        mesh.nglobal_ents(2),
        dim == 3 ? mesh.nglobal_ents(3) : 0);
  }

  auto n_edge = mesh.nedges();
  auto edge2verts = mesh.get_adj(1,0);
  assert(edge2verts.ab2b.size() == 2*n_edge);
  // each edge associates with 2 vertices

  auto n_face = mesh.nfaces();
  auto face2edges = mesh.get_adj(2,1);
  assert(face2edges.ab2b.size() == 3*n_face);
  // each face associates with 3 edges (all tris and tets)

  if (dim == 3) {
    auto n_cell = mesh.nelems();
    auto cell2faces = mesh.get_adj(3,2);
    assert(cell2faces.ab2b.size() == 4*n_cell);
    // each cell associates with 4 faces (all tets)
    auto face2cell = mesh.ask_up(2,3);
    assert(face2cell.ab2b.size() == 4*n_cell);
    // due to reverse graph from c2f
  }

  auto edge2face = mesh.ask_up(1,2);
  assert(edge2face.ab2b.size() == 3*n_face);
  // due to reverse graph from f2e

  auto vert2edges = mesh.ask_up(0,1);
  assert(vert2edges.ab2b.size() == 2*n_edge);
  // due to reverse graph from e2v

  auto vert2vert = mesh.ask_star(0);
  assert(vert2vert.ab2b.size() == 2*n_edge);
  // counts each edge twice, once from each vertex
  return 0;
}
