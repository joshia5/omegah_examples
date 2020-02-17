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
  const auto inmesh = argv[1];
  Mesh mesh(&lib);
  binary::read(inmesh, lib.world(), &mesh);
  const auto rank = lib.world()->rank();
  const auto comm = lib.world();

  MPI_Barrier(MPI_COMM_WORLD);  
  //mesh::nghost_layers, set_parting(omega_h_ghosted)
  mesh.set_parting(OMEGA_H_GHOSTED, 1, 1);
  MPI_Barrier(MPI_COMM_WORLD);

  // create 3d partitioned mesh?
  return 0;
}
