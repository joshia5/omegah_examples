#include <Omega_h_file.hpp>
#include <Omega_h_library.hpp>
#include <Omega_h_mesh.hpp>
#include <Omega_h_array_ops.hpp>
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
  const auto comm = lib.world();
  LO max_input = 45;

  Write<LO> for_max(mesh.nfaces(), 0, "for_max");
  if (!rank) {
    auto set_max_val = OMEGA_H_LAMBDA(LO i) {
      if(!i) for_max[i] = max_input;
    };
    parallel_for(1, set_max_val, "set_max_val");
  }
  Read<LO> for_max_r(for_max);
  auto max_val = get_max(comm, for_max_r);
  assert (max_val == max_input);

  return 0;
}
