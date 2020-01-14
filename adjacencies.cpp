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

  auto n_face = mesh.nfaces();
  auto face2edges = mesh.get_adj(2,1);
  assert(face2edges.ab2b.size() == 3*n_face);

  if (dim == 3) {
    auto n_cell = mesh.nelems();
    auto cell2faces = mesh.get_adj(3,2);
    assert(cell2faces.ab2b.size() == 4*n_cell);
    auto face2cell = mesh.ask_up(2,3);
    assert(face2cell.ab2b.size() == 4*n_cell);
    auto a2ab = face2cell.a2ab;
    int lastVal;
    int lastIndex = a2ab.size() - 1;
    Kokkos::deep_copy(lastVal,Kokkos::subview(a2ab.view(),lastIndex));
    cout << " lastValue in index array f2c " << lastVal << " ncell " << 4*n_cell << " last index " << lastIndex << endl;
    assert(lastVal <= 4*n_cell);
  }

  auto vert2edges = mesh.ask_up(0,1);
  auto a2ab = vert2edges.a2ab;
  int lastVal;
  int lastIndex = a2ab.size() - 1;
  Kokkos::deep_copy(lastVal,Kokkos::subview(a2ab.view(),lastIndex));
  cout << " lastValue in index array v2e " << lastVal << " nedge " << 2*n_edge << " last index " << lastIndex << endl;
  assert(lastVal <= 2*n_edge);
  assert(vert2edges.ab2b.size() == 2*n_edge);

  auto edge2face = mesh.ask_up(1,2);
  a2ab = edge2face.a2ab;
  lastIndex = a2ab.size() - 1;
  Kokkos::deep_copy(lastVal,Kokkos::subview(a2ab.view(),lastIndex));
  cout << " lastValue in index array e2f " << lastVal << " nface " << 3*n_face << " last index " << lastIndex << endl;
  assert(lastVal <= 3*n_face);
  assert(edge2face.ab2b.size() == 3*n_face);
  return 0;
}
