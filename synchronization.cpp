#include <Omega_h_file.hpp>
#include <Omega_h_library.hpp>
#include <Omega_h_mesh.hpp>
#include <Omega_h_for.hpp>
#include "Omega_h_dist.hpp"
using namespace Omega_h;

void print_owners(Remotes owners, int rank) {
  printf("\n");
  auto ranks = owners.ranks;
  auto idxs = owners.idxs;
  auto ranks_w = Write<LO> (ranks.size());
  auto idxs_w = Write<LO> (idxs.size());
  auto r2w = OMEGA_H_LAMBDA(LO i) {
    ranks_w[i] = ranks[i];
    idxs_w[i] = idxs[i];
  };
  parallel_for(idxs.size(), r2w);
  auto ranks_host = HostWrite<LO>(ranks_w);
  auto idxs_host = HostWrite<LO>(idxs_w);
  printf("On rank %d\n", rank);
  for (int i=0; i<idxs_host.size(); ++i) {
    printf("owner of %d, is on rank %d, with LId %d\n", i, ranks_host[i], idxs_host[i]);
  };
  printf("\n");
  printf("\n");
  return;
}

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

  mesh.add_tag<Real>(0, "gravity", 1);
  if (!rank) {
    Write<Real> gravityArray(mesh.nverts(), 9.81, "gravityArray");
    Read<Real> gravityArray_r(gravityArray);
    mesh.set_tag<Real>(0, "gravity", gravityArray_r);
  }
  else {
    Write<Real> gravityArray(mesh.nverts(), 0.0, "gravityArray");
/*
set tag values for serial test case
    auto set_vals = OMEGA_H_LAMBDA(LO i) {
      if ((i==0)||(i==1)||(i==4)) gravityArray[i] = 1;
    };
    parallel_for(gravityArray.size(), set_vals);
*/
    Read<Real> gravityArray_r(gravityArray);
    mesh.set_tag<Real>(0, "gravity", gravityArray_r);
  }

  vtk::write_parallel("/users/joshia5/new_mesh/before_synchronization_2p.vtk", &mesh, false);
  auto owners_old = mesh.ask_owners(0);
  print_owners(owners_old, rank);
  auto dist = mesh.ask_dist(0);
  printf("roots=%d, items=%d, srcs=%d, dests=%d\n", dist.nroots(), dist.nitems(), dist.nsrcs(), dist.ndests());

  if (rank == 1) {
/*
    auto vert_owners = mesh.ask_owners(0);
    auto idxs = vert_owners.idxs;
    auto ranks = vert_owners.ranks;
    auto idxs_w = Write<LO> (idxs.size());
    auto ranks_w = Write<LO> (idxs.size());
    auto set_ownerVals = OMEGA_H_LAMBDA(LO i) {
      if ((i==0)||(i==1)||(i==3)) {
        idxs_w[0] = 1;
        ranks_w[0] = 0;
        idxs_w[1] = 0;
        ranks_w[1] = 0;
        idxs_w[3] = 2;
        ranks_w[3] = 0;
      }
      else {
        idxs_w[i] = idxs[i];
        ranks_w[i] = ranks[i];
      }
    };
    parallel_for(idxs.size(), set_ownerVals, "set_ownerVals");
*/
    mesh.set_owners(0, Remotes(LOs({0,0,0,0,0,0}), LOs({1,0,3,2,4,5})));
    //mesh.set_owners(0, Remotes(read(ranks_w), read(idxs_w)));
  //dist.set_roots2items(read(idxs_w));//does update nroots. cant determine if input is correct or not
  //dist.set_dest_idxs(read(idxs_w), mesh.nverts());//has no effect on nroots
  //sync dosent work in both cases
  }

  mesh.sync_tag(0, "gravity");
  auto new_owners = mesh.ask_owners(0);
  printf("new owners\n");
  print_owners(new_owners, rank);
  printf("new roots=%d, items=%d, srcs=%d, dests=%d\n", dist.nroots(), dist.nitems(), dist.nsrcs(), dist.ndests());
  vtk::write_parallel("/users/joshia5/new_mesh/synchronization_2p_matched.vtk", &mesh, false);
  
  return 0;
}
