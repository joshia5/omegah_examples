#include <Omega_h_file.hpp>
#include <Omega_h_library.hpp>
#include <Omega_h_mesh.hpp>
#include <Omega_h_for.hpp>
using namespace std;
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

  int numPts = 1;
  /*int cell_ID[numPts];
  double point[numPts*dim];
  for (int i=0; i<numPts; ++i) {
    cout << " Enter element ID and coordinates for point " << i+1 << endl;
    cin >> cell_ID[i] >> point[3*i] >> point[3*i+1] >> point[3*i+2];
  }*/

  Read <LO> elmIds(numPts, 1, "elmIds");
  Read <Real> p(numPts*3, 0.0, "p");
  auto cell2vert = mesh.ask_elem_verts();
  auto vert_coords = mesh.coords();
  int verts_per_tet = 4;
  Write <Real> cell_verts_coords(dim*verts_per_tet, 0.0, "cell_verts_coords");
  Write <LO> cell_verts_id(verts_per_tet, 1, "cell_verts_id");
  Write <Real> a(dim, 0.0, "a");
  Write <Real> b(dim, 0.0, "b");
  Write <Real> c(dim, 0.0, "c");
  Write <Real> d(dim, 0.0, "d");
  Write <Real> bc(dim, 0.0, "bc");
  Write <Real> bd(dim, 0.0, "bd");
  Write <Real> cd(dim, 0.0, "cd");
  Write <Real> ca(dim, 0.0, "ca");
  Write <Real> da(dim, 0.0, "da");
  Write <Real> ab(dim, 0.0, "ab");
  Write <Real> p_a(dim, 0.0, "p_a");
  Write <Real> p_b(dim, 0.0, "p_b");
  Write <Real> p_c(dim, 0.0, "p_c");
  Write <Real> p_d(dim, 0.0, "p_d");

  auto f = OMEGA_H_LAMBDA(LO i) {
    for (int j=0; j<verts_per_tet; ++j) {
      cell_verts_id[j] = cell2vert[4*(elmIds[i]-1)+j];
      for (int k=0; k<dim; ++k) {
        cell_verts_coords[j+k] = vert_coords[cell_verts_id[j]+k];
      }
    }
    for (int k=0; k<dim; ++k) {
      a[k] = cell_verts_coords[k];
      b[k] = cell_verts_coords[k+3];
      c[k] = cell_verts_coords[k+6];
      d[k] = cell_verts_coords[k+9];
      bc[k] = b[k] - c[k];
      bd[k] = b[k] - d[k];
      cd[k] = c[k] - d[k];
      ca[k] = c[k] - a[k];
      da[k] = d[k] - a[k];
      ab[k] = a[k] - b[k];
      p_a[k] = p[k] - a[k];
      p_b[k] = p[k] - b[k];
      p_c[k] = p[k] - c[k];
      p_d[k] = p[k] - d[k];
    }
    auto bc_v = vector_3(bc[0], bc[1], bc[2]);
    auto bd_v = vector_3(bd[0], bd[1], bd[2]);
    auto cd_v = vector_3(cd[0], cd[1], cd[2]);
    auto ca_v = vector_3(ca[0], ca[1], ca[2]);
    auto da_v = vector_3(da[0], da[1], da[2]);
    auto ab_v = vector_3(ab[0], ab[1], ab[2]);

    auto p_a_v = vector_3(p_a[0], p_a[1], p_a[2]);
    auto p_b_v = vector_3(p_b[0], p_b[1], p_b[2]);
    auto p_c_v = vector_3(p_c[0], p_c[1], p_c[2]);
    auto p_d_v = vector_3(p_d[0], p_d[1], p_d[2]);

    auto normal_a = normalize(cross(bc_v,bd_v));
    auto normal_b = normalize(cross(cd_v,ca_v));
    auto normal_c = normalize(cross(da_v,-bd_v));
    auto normal_d = normalize(cross(ab_v,-ca_v));

    auto xi_a = (p_b_v*normal_a)/((bc_v)*normal_a);
    auto xi_b = (p_c_v*normal_b)/((cd_v)*normal_b);
    auto xi_c = (p_d_v*normal_c)/((da_v)*normal_c);
    auto xi_d = (p_a_v*normal_d)/((ab_v)*normal_d);
  };
  parallel_for(numPts, f, "barycentric_coords");

  //source for calculation "https://people.sc.fsu.edu/~jburkardt/presentations/cg_lab_barycentric_tetrahedrons.pdf"
  return 0;
}
