#include <Omega_h_file.hpp>
#include <Omega_h_library.hpp>
#include <Omega_h_mesh.hpp>
#include <Omega_h_for.hpp>
using namespace Omega_h;

/*template <typename T>
Read<T> get_max_of(Read<T> my_array); 
template <typename T>
*/
int get_max_of(Read<LO> my_array) {
  int maxval[1] = {0};
  auto findMax_GPU = OMEGA_H_LAMBDA(LO i) {
    if (my_array[i] >= maxval[0]) {
	    printf(" here %d and %d \n", maxval[0], my_array[i]);
      //maxval[0] = my_array[i];
    }
  };
  parallel_for(my_array.size(), findMax_GPU, "findMax_GPU");
  return maxval[0];
}

int main(int argc, char** argv) {
  auto lib = Omega_h::Library(&argc, &argv);
  if(argc!=2) {
    fprintf(stderr, "Usage: %s <input mesh>\n", argv[0]);
    return 0;
  }
  const auto rank = lib.world()->rank();
  const auto inmesh = argv[1];
  Mesh mesh(&lib);
  binary::read(inmesh, lib.world(), &mesh);
  const auto dim = mesh.dim();

  auto my_classID = mesh.get_array<LO>(0,"class_id");
  auto maxval = get_max_of(my_classID);

  return 0;
}
