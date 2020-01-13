#include <Omega_h_file.hpp>
#include <Omega_h_library.hpp>
#include <Omega_h_mesh.hpp>
#include <iostream>
#include <array>
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

  auto n_edge = mesh.nedges();
  auto edge2verts = mesh.get_adj(1,0);
  assert(edge2verts.ab2b.size() == 2*n_edge);

  auto n_face = mesh.nfaces();
  auto face2edges = mesh.get_adj(2,1);
  assert(face2edges.ab2b.size() == 3*n_face);

  if (dim == 3) {
    auto n_cell = mesh.nelems();
    auto cell2faces = mesh.get_adj(3,2);
    assert(cell2faces.ab2b.size() == 4*n_cell);
    auto face2cell = mesh.ask_up(2,3);
    assert(face2cell.ab2b.size() == 4*n_cell);
  }

  auto vert2edges = mesh.ask_up(0,1);
  auto a2ab = vert2edges.a2ab;
  int lastVal;
  Kokkos::deep_copy(lastVal,Kokkos::subview(a2ab.view(),69104));
  assert(lastVal <= 2*n_edge);
  assert(vert2edges.ab2b.size() == 2*n_edge);

  auto edge2face = mesh.ask_up(1,2);
//  cout << "here  " << edge2face.ab2b.size() << "face " << n_face << "edge " << n_edge << endl;
  assert(edge2face.ab2b.size() == 3*n_face);
  return 0;
}
