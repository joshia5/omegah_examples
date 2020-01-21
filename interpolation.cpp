#include <Omega_h_file.hpp>
#include <Omega_h_library.hpp>
#include <Omega_h_mesh.hpp>
#include <initializer_list>
using namespace std;

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

  //read cellid and point coords from user and pass to GPU
  int numPts;
  cout << " Enter no. of points to test " << endl;
  cin >> numPts;
  int cell_ID[numPts];
  double point[numPts*dim];
  for (int i=0; i<numPts; ++i) {
    cout << " Enter element ID and coordinates for point " << i+1 << endl;
    if (dim == 3) {
      cin >> cell_ID[i] >> point[3*i] >> point[3*i+1] >> point[3*i+2];
    }
    else {
      cin >> cell_ID[i] >> point[2*i] >> point[2*i+1];
    }      
  }

  //figure out how to pass these values to initializer_list for making it a Read object
  //std::initializer_list<Omega_h::LO> ids = {cell_ID};
  //Omega_h::Read<Omega_h::LO>(ids,"elmIds");
  /*For example:
    Few(std::initializer_list<T> l) {
    Int i = 0;
    for (auto it = l.begin(); it != l.end(); ++it) {
      new (data() + (i++)) T(*it);
    }
  }*/

  //get ids of verts of that cell (how to copy multiple values at a time?)
  auto cell2vert = mesh.ask_elem_verts();
  int cell_verts_id[(numPts*4)*dim];
  for (int i=0; i<dim; ++i) {
    Kokkos::deep_copy(cell_verts_id[(numPts-1)+i],Kokkos::subview(cell2vert.view(),4*cell_ID[numPts-1]+i));
  }
 // Kokkos::deep_copy(cell_verts_id[1],Kokkos::subview(cell2vert.view(),4*cell_ID[0]+1));
  //Kokkos::deep_copy(cell_verts_id[2],Kokkos::subview(cell2vert.view(),4*cell_ID[0]+2));
  //Kokkos::deep_copy(cell_verts_id[3],Kokkos::subview(cell2vert.view(),4*cell_ID[0]+3));

  //get coordinates of the vertices {a,b,c,d}
  auto vert_coords = mesh.coords();
  double cell_verts_coords[4*dim];
  Kokkos::deep_copy(cell_verts_coords[0],Kokkos::subview(vert_coords.view(),cell_verts_id[0]));//what about x,y,z?
  Kokkos::deep_copy(cell_verts_coords[1],Kokkos::subview(vert_coords.view(),cell_verts_id[1]));
  Kokkos::deep_copy(cell_verts_coords[2],Kokkos::subview(vert_coords.view(),cell_verts_id[2]));
  Kokkos::deep_copy(cell_verts_coords[3],Kokkos::subview(vert_coords.view(),cell_verts_id[3]));
  //routine to calculate b.centric coords for tet. and also inturn calculate is in or out of cell "https://people.sc.fsu.edu/~jburkardt/presentations/cg_lab_barycentric_tetrahedrons.pdf"
  // for each face get normal and calc b.centric coord for that face as (dot((p-b).n)/dot((a-b).n))
  // example of normal auto normal = normalize(cross((b - a), (c - a))) in r3d_test.cpp;
  //look at dot and cross in vector.hpp
  return 0;
}
