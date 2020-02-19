#include <Omega_h_file.hpp>
#include <Omega_h_library.hpp>
#include <Omega_h_mesh.hpp>
using namespace Omega_h;

int main(int argc, char** argv) {
  auto lib = Library(&argc, &argv);
  if(argc!=2) {
    fprintf(stderr, "Usage: %s <input mesh>\n", argv[0]);
    return 0;
  }
  const auto rank = lib.world()->rank();
  const auto inmesh = argv[1];
  Mesh mesh(&lib);
  binary::read(inmesh, lib.world(), &mesh);
  const auto dim = mesh.dim();

  mesh.add_tag<Real>(0, "gravity", 1);
  if (rank==2) {
    Write<Real> gravityArray(mesh.nverts(), 9.81, "gravityArray");
    Read<Real> gravityArray_r(gravityArray);
    mesh.set_tag<Real>(0, "gravity", gravityArray_r);
  }
  else {
    Write<Real> gravityArray(mesh.nverts(), 0.0, "gravityArray");
    Read<Real> gravityArray_r(gravityArray);
    mesh.set_tag<Real>(0, "gravity", gravityArray_r);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  vtk::write_parallel("/users/joshia5/omegah_examples/new_mesh/before_synchronization.vtk", &mesh, false);
  mesh.sync_tag(0, "gravity");
  vtk::write_parallel("/users/joshia5/omegah_examples/new_mesh/synchronization.vtk", &mesh, false);
  
  return 0;
}
