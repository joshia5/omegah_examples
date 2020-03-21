#include <Omega_h_file.hpp>
#include <Omega_h_library.hpp>
#include <Omega_h_mesh.hpp>
#include <Omega_h_array_ops.hpp>
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

  if (!rank) {
    Write<Real> weight_w(mesh.nelems(), 0.5);
    auto weight_r = Reals(weight_w);
    mesh.add_tag<Real>(dim, "weight", 1, weight_r);
  }
  else {
    Write<Real> weight_w(mesh.nelems(), 1);
    auto weight_r = Reals(weight_w);
    mesh.add_tag<Real>(dim, "weight", 1, weight_r);
  }
   
  MPI_Barrier(MPI_COMM_WORLD);
  auto weight_array_0 = mesh.get_array<Real>(dim, "weight");
  mesh.balance(weight_array_0);
  auto weight_array_1 = mesh.get_array<Real>(dim, "weight");

  auto sum_0 = get_sum(mesh.comm(),weight_array_0);
  auto sum_1 = get_sum(mesh.comm(),weight_array_1);
  assert (sum_0 == sum_1);
  if (!rank) printf("sum before %3.3f, after %3.3f \n", sum_0, sum_1);
  return 0;
}
