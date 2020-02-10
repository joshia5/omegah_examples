#include <Omega_h_file.hpp>
#include <Omega_h_library.hpp>
#include <Omega_h_mesh.hpp>
#include <Omega_h_array_ops.hpp>

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

  auto faces = mesh.ask_up(1,2).ab2b;
  auto comm = lib.world();
  auto max_face = get_max(faces);
  auto max_face_comm = get_max(comm, faces);
  std::cout << "\n loc max " << max_face << "\n glo max" << max_face_comm << std::endl;
  //std::cout << "\n  rank " << rank << ", maxface " << max_face << ", nface " << mesh.nfaces() << ", gface " << mesh.nglobal_ents(2) << std::endl;
  if (rank == 0) assert(max_face+1 == mesh.nfaces());

  auto vert_coords = mesh.coords();
  auto max_coord = get_max(vert_coords);
  auto max_coord_comm = get_max(comm, vert_coords);
  std::cout << "\n rank " << rank << " coord loc max " << max_coord << "\n glo max" << max_coord_comm << std::endl;

  return 0;
}
