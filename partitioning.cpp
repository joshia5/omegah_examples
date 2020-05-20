#include <Omega_h_file.hpp>
#include <Omega_h_library.hpp>
#include <Omega_h_mesh.hpp>
#include <Omega_h_array_ops.hpp>
using namespace Omega_h;

Real get_imbalance(Mesh mesh, Read<Real> weight_array) {
  auto sum = get_sum(weight_array);
  Write<Real> sum_w(1,sum);
  auto sum_r = Reals(sum_w);
  auto max_sum = get_max(mesh.comm(), sum_r);
  auto avg_sum = get_sum(mesh.comm(), weight_array)/4;
  Real imbalance = max_sum/avg_sum;
  return imbalance;
}

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
  const auto size = lib.world()->size();
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
   
  auto weight_array_0 = mesh.get_array<Real>(dim, "weight");
  mesh.balance(weight_array_0);
  auto weight_array_1 = mesh.get_array<Real>(dim, "weight");
  auto imbalance_0 = get_imbalance(mesh, weight_array_0);
  auto imbalance_1 = get_imbalance(mesh, weight_array_1);
  if (!rank) {
    printf("Weight imbalance before & after balance call %f, %f \n"
		    , imbalance_0, imbalance_1);
  }
  return 0;
}
