#include <Omega_h_file.hpp>
#include <Omega_h_library.hpp>
#include <Omega_h_mesh.hpp>
#include <Omega_h_adapt.hpp>
#include <Omega_h_for.hpp>

using namespace Omega_h;
/*Reals logistic_function(Reals x, Real x0, Real L, Real k) {
  Write<Real> out(x.size());
  auto f = OMEGA_H_LAMBDA(LO i) {
    out[i] = L / (1 + std::exp(-k * (x[i] - x0)));
  };
  parallel_for(x.size(), f);
  return out;
}*/
int main(int argc, char** argv) {
  auto lib = Library(&argc, &argv);
  if(argc!=2) {
    fprintf(stderr, "Usage: %s <input mesh>\n", argv[0]);
    return 0;
  }
  const auto inmesh = argv[1];
  Mesh mesh(&lib);
  binary::read(inmesh, lib.world(), &mesh);

/*
  auto coords = mesh.coords();
  auto sol = logistic_function(coords, 0.5, 1.0, 20.0);
  mesh.add_tag(0, "solution", 1, sol);
  MetricInput input;
  input.sources.push_back(MetricSource{OMEGA_H_VARIATION, 1.0, "solution"});
  input.should_limit_lengths = true;
  input.max_length = 1.0;
  input.should_limit_gradation = true;
  input.max_gradation_rate = 1.0;
  input.should_limit_element_count = true;
  input.max_element_count = 100;
  input.min_element_count = 50;
  generate_metric_tag(&mesh, input);
*/
  mesh.add_tag<Real>(0, "metric", 1);
  Write<Real> gravityArray(mesh.nverts(), 9.81, "gravityArray");
  Read<Real> gravityArray_r(gravityArray);
  mesh.set_tag<Real>(0, "metric", gravityArray_r);
  mesh.balance(0);

  //add_tag "metric" and it partitions acc. to those values
  //define weights or tag, etc. i.e. generate metric, generte metric tag, add metric tag
  //find api to repartition mesh asper weights i.e. mesh::balance, mesh::set_parting
  return 0;
}
