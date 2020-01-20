#include <Omega_h_file.hpp>
#include <Omega_h_library.hpp>
#include <Omega_h_mesh.hpp>
using namespace std;

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

  int numPts;
  cout << " Enter no. of points to test " << endl;
  cin >> numPts;
  int cell_ID[numPts];
  double point[numPts*3];
  for (int i=0; i<numPts; ++i) {
    cout << " Enter element ID and coordinates for point " << i+1 << endl;
    cin >> cell_ID[i] >> point[3*i] >> point[3*i+1] >> point[3*i+2];
  }
  std::initializer_list<Omega_h::LO> ids = cell_ID;
  Omega_h::Read<Omega_h::LO>(ids,"elmIds");
//  Omega_h::Write<Omega_h::Real> xyz(numPts*3);
//  Omega_h::Write<Omega_h::LO> elmIds(numPts);
  return 0;
}
