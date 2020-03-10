#include <Omega_h_file.hpp>
#include <Omega_h_build.hpp>
#include <Omega_h_mark.hpp>
using namespace Omega_h;

int main(int argc, char** argv) {
  auto lib = Library(&argc, &argv);
  if(argc!=2) {
    fprintf(stderr, "Usage: %s <input mesh>\n", argv[0]);
    return 0;
  }
  const auto inmesh = argv[1];
/*  auto mesh = build_box(
		        lib.world(), OMEGA_H_HYPERCUBE, 1.0, 1.0, 1.0, 8, 8, 3);
  binary::write("/users/joshia5/omegah_examples_serial/meshes/hypercube.osh", &mesh);
  mesh = binary::read("/users/joshia5/omegah_examples_serial/meshes/hypercube.osh", lib.world());
  vtk::write_vtu("/users/joshia5/new_mesh/hypercube.vtu", &mesh);
*/
  Mesh mesh(&lib);
  binary::read(inmesh, lib.world(), &mesh);
  const auto dim = mesh.dim();

  //Test common API calls

  //Adjacency queries
  auto c2v = mesh.ask_down(3,0);
  assert(c2v.ab2b.size() == 8*mesh.nelems());
  auto v2c = mesh.ask_up(0,3);
  auto e2f = mesh.ask_up(1,2);
  assert(e2f.ab2b.size() == 4*mesh.nfaces());

  //Tags
  auto nvert = mesh.nverts();
  mesh.add_tag<Real>(0, "gravity", 1);
  Write<Real> gravityArray(nvert, 9.81, "gravityArray");
  Read<Real> gravityArray_r(gravityArray);
  mesh.set_tag<Real>(0, "gravity", gravityArray_r);
  binary::write("./tag.osh", &mesh);
  mesh.remove_tag(0, "gravity");

  //Interpolation
  auto vert_coords = mesh.coords();

  //Classification
  int model_ent_id = 1, model_ent_dim = 2;
  auto vertsOnModelEnt = mark_by_class(&mesh, 0, model_ent_dim, model_ent_id);
  return 0;
}
