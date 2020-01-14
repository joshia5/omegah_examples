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
  mesh.add_tag<Omega_h::Real>(0, "gravity", 1);
  Omega_h::Write<Omega_h::Real> gravityArray(nvert, 9.81, "gravityArray");
  Omega_h::Read<Omega_h::Real> gravityArray_r(gravityArray);
  mesh.set_tag<Omega_h::Real>(0, "gravity", gravityArray_r);
  Omega_h::binary::write("./tag.osh", &mesh);
  mesh.remove_tag(0, "gravity");
  Omega_h::binary::write("./no_tag.osh", &mesh);

  return 0;
}
