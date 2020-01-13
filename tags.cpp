#include <Omega_h_file.hpp>
#include <Omega_h_library.hpp>
#include <Omega_h_mesh.hpp>

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
  auto nvert = mesh.nverts(); 

  double gravity[nvert] = {0.0};
  //double gravity[nvert][dim] = {0.0};
  mesh.add_tag<double>(0, "gravity", 1, gravity, 0);
  Omega_h::binary::write("/users/joshia5/omegah_examples/new_mesh/tag.osh", &mesh);
 //mesh.set_tag<double>(0, "gravity", gravity);
 // mesh.remove_tag(0, "gravity");

  return 0;
}
