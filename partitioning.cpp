#include <Omega_h_file.hpp>
#include <Omega_h_library.hpp>
#include <Omega_h_mesh.hpp>
#include <Omega_h_for.hpp>
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
  const auto dim = mesh.dim();

  mesh.add_tag<Real>(0, "metric", 1);

  if (!rank) {
    Write<Real> metricArray(mesh.nverts(), 1000000, "metricArray");
    auto metricArray_r = Reals(metricArray);
    mesh.set_tag<Real>(0, "metric", metricArray_r);
  }
  else {
    Write<Real> metricArray(mesh.nverts(), 1, "metricArray");
    auto metricArray_r = Reals(metricArray);
    mesh.set_tag<Real>(0, "metric", metricArray_r);
  }
   
  MPI_Barrier(MPI_COMM_WORLD);
  printf("before PLB, rank %d, nverts %d , nelems %d \n", rank, mesh.nverts(), mesh.nelems());
  mesh.balance(1);
  printf("after PLB, rank %d, nverts %d , nelems %d \n", rank, mesh.nverts(), mesh.nelems());
  vtk::write_parallel("/users/joshia5/new_mesh/balance.vtk", &mesh, false);

  return 0;
}
