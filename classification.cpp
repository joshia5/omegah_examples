#include <Omega_h_file.hpp>
#include <Omega_h_library.hpp>
#include <Omega_h_mesh.hpp>
#include <Omega_h_for.hpp>
#include <Omega_h_mark.hpp>
using namespace Omega_h;

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

  //get face id for model face
  //?

  //get mesh face for that model face(currently for all model faces)
  auto exposed_sides = mark_exposed_sides(&mesh);
  auto side_side2elem = mesh.ask_up(2,3).a2ab;
  auto elem_side2elem = mesh.ask_up(2,3).ab2b;
  auto f = OMEGA_H_LAMBDA(LO i) {
  //identify elements on those faces
    if (exposed_sides[i] > 0) {
      auto index = side_side2elem[i];
      auto cell_id = elem_side2elem[index];
    }
  };
  parallel_for(mesh.nfaces(),f,"f");
  //Look at following cpps assoc, mark, maybe surface
  //classify_sides_by_exposure; takes input exposed sides and returns uique identifier value in class_dim
  return 0;
}
