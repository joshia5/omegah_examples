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

  auto nvert = mesh.nverts(); 
  mesh.add_tag<Real>(0, "gravity", 1);
  Write<Real> gravityArray(nvert, 9.81, "gravityArray");
  Read<Real> gravityArray_r(gravityArray);
  mesh.set_tag<Real>(0, "gravity", gravityArray_r);
  binary::write("./tag.osh", &mesh);
  mesh.remove_tag(0, "gravity");
  binary::write("./no_tag.osh", &mesh);

  return 0;
}
