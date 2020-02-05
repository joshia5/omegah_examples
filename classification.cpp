#include <Omega_h_file.hpp>
#include <Omega_h_library.hpp>
#include <Omega_h_mesh.hpp>
#include <Omega_h_for.hpp>
#include <Omega_h_mark.hpp>
using namespace Omega_h;

Read<LO> rev_classify_cells(Mesh mesh, int model_ent_dim, int model_ent_id) {

  auto vertsOnModelEnt = mark_by_class(&mesh, 0, model_ent_dim, model_ent_id);
  auto vert_vert2elem = mesh.ask_up(0,3).a2ab;
  auto elem_vert2elem = mesh.ask_up(0,3).ab2b;
  Write<LO> cellOnModelEnt(mesh.nelems(), 0, "cellOnModelEnt");
  
  auto classifyCells = OMEGA_H_LAMBDA(LO i) {
    if (vertsOnModelEnt[i] > 0) {
      for (int j = vert_vert2elem[i]; j < vert_vert2elem[i+1]; ++j) {
        auto cell_id = elem_vert2elem[j];
	cellOnModelEnt[cell_id] = 1;
      }
    }
  };
  parallel_for(mesh.nverts(), classifyCells, "classifyCells");
  Read<LO> cellOnModelEnt_r(cellOnModelEnt);
  return cellOnModelEnt_r;
}

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

  int model_ent_id = 91;
  int model_ent_dim = 3;
  Read<LO> cellOnModelEnt = rev_classify_cells(mesh, model_ent_dim, model_ent_id);
  mesh.add_tag<LO>(dim, "cellsOnEnt", 1, cellOnModelEnt);
  binary::write("./rev_class.osh", &mesh);

  model_ent_id = 1;
  model_ent_dim = 2;
  Read<LO> cellOnModelEnt2 = rev_classify_cells(mesh, model_ent_dim, model_ent_id);
  mesh.add_tag<LO>(dim, "cellsOnEnt", 1, cellOnModelEnt2);
  binary::write("./rev_class2.osh", &mesh);

  return 0;
}
