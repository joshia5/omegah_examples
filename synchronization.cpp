#include <Omega_h_file.hpp>
#include <Omega_h_library.hpp>
#include <Omega_h_mesh.hpp>
using namespace Omega_h;
using namespace std;

int main(int argc, char** argv) {
  auto lib = Library(&argc, &argv);
  if(argc!=2) {
    fprintf(stderr, "Usage: %s <input mesh>\n", argv[0]);
    return 0;
  }
  const auto rank = lib.world()->rank();
  const auto comm = lib.world();
  const auto inmesh = argv[1];
  Mesh mesh(&lib);
  binary::read(inmesh, lib.world(), &mesh);
  const auto dim = mesh.dim();
/*
  mesh.add_tag<Real>(0, "gravity", 1);

  cout << "\n 1 rank is " << rank << endl;
  if (rank == 1) {
    Write<Real> gravityArray(mesh.nverts(), 9.81, "gravityArray");
    Read<Real> gravityArray_r(gravityArray);
    mesh.set_tag<Real>(0, "gravity", gravityArray_r);
    //mesh.sync_tag(0, "gravity");
  }
  MPI_Barrier(MPI_COMM_WORLD);
  cout << "\n 2 rank is " << rank << endl;
  //mesh.sync_tag(0, "gravity");
  cout << "\n 3 rank is " << rank << endl;
  //write parallel file
  //vtk::write_parallel("./partition.vtk", &mesh, dim);
*/
  return 0;
}
