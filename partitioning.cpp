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
    Write<Real> metricArray(mesh.nverts(), 1, "metricArray");
    auto write_values = OMEGA_H_LAMBDA(LO i) {
      if (i == 0) metricArray[i] = 0.5;
    };
    parallel_for(mesh.nverts(), write_values);
    auto metricArray_r = Reals(metricArray);
    mesh.set_tag<Real>(0, "metric", metricArray_r);
  }
  else {
    Write<Real> metricArray(mesh.nverts(), 1, "metricArray");
    auto metricArray_r = Reals(metricArray);
    mesh.set_tag<Real>(0, "metric", metricArray_r);
  }
   
  MPI_Barrier(MPI_COMM_WORLD);
  //mesh.sync_tag(0,"metric");
  mesh.balance(1);
  vtk::write_parallel("/users/joshia5/new_mesh/balance.vtk", &mesh, false);

  return 0;
}
