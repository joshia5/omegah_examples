#include <Omega_h_file.hpp>
#include <Omega_h_library.hpp>
#include <Omega_h_mesh.hpp>
#include <Omega_h_for.hpp>
#include <Omega_h_shape.hpp>
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

  const int numPts = mesh.nelems();
  const int verts_per_tet = 4;
  auto cell2vert = mesh.ask_elem_verts();
  auto vert_coords = mesh.coords();
  Write <LO> elmIds_w(numPts, "elmIds");
  Write <Real> p_w(numPts*3, "p");

  auto f_in = OMEGA_H_LAMBDA(LO i) {
    Real a[3], b[3], c[3], d[3]; 
    Real cell_verts_coords[verts_per_tet*3];
    elmIds_w[i] = i;
    for (int j=0; j<verts_per_tet; ++j) {
      const LO cell_vert_id = cell2vert[verts_per_tet*elmIds_w[i]+j];
      for (int k=0; k<dim; ++k) {
        cell_verts_coords[dim*j+k] = vert_coords[cell_vert_id*dim+k];
      }
    }
    for (int k=0; k<dim; ++k) {
      a[k] = cell_verts_coords[k];
      b[k] = cell_verts_coords[k+3];
      c[k] = cell_verts_coords[k+6];
      d[k] = cell_verts_coords[k+9];
//      p_w[i*dim+k] = (a[k]+b[k]+c[k]+d[k])/verts_per_tet;
      p_w[i*dim+k] = a[k];
    }
  };
  parallel_for(numPts, f_in, "barycentric_coords");
  Read <LO> elmIds(elmIds_w);
  Read <Real> p(p_w);

  auto f = OMEGA_H_LAMBDA(LO i) {
    Real a[3], b[3], c[3], d[3]; 
    Real cell_verts_coords[verts_per_tet*3];
    for (int j=0; j<verts_per_tet; ++j) {
      const LO cell_vert_id = cell2vert[verts_per_tet*elmIds[i]+j];
      for (int k=0; k<dim; ++k) {
        cell_verts_coords[dim*j+k] = vert_coords[cell_vert_id*dim+k];
      }
    }
    for (int k=0; k<dim; ++k) {
      a[k] = cell_verts_coords[k];
      b[k] = cell_verts_coords[k+3];
      c[k] = cell_verts_coords[k+6];
      d[k] = cell_verts_coords[k+9];
    }

    auto a_v = vector_3(a[0], a[1], a[2]);
    auto b_v = vector_3(b[0], b[1], b[2]);
    auto c_v = vector_3(c[0], c[1], c[2]);
    auto d_v = vector_3(d[0], d[1], d[2]);
    auto p_v = vector_3(p[i*dim+0], p[i*dim+1], p[i*dim+2]);

    auto normal_a = normalize(get_triangle_normal(b_v,c_v,d_v));
    auto normal_b = -normalize(get_triangle_normal(a_v,c_v,d_v));
    auto normal_c = normalize(get_triangle_normal(a_v,b_v,d_v));
    auto normal_d = -normalize(get_triangle_normal(a_v,b_v,c_v));

    auto xi_a = ((p_v-b_v)*normal_a)/((a_v-b_v)*normal_a);
    auto xi_b = ((p_v-c_v)*normal_b)/((b_v-c_v)*normal_b);
    auto xi_c = ((p_v-d_v)*normal_c)/((c_v-d_v)*normal_c);
    auto xi_d = ((p_v-a_v)*normal_d)/((d_v-a_v)*normal_d);

    printf("For cell index %d Barycentric coordinates are %f %f %f %f \n", i, xi_a, xi_b, xi_c, xi_d);
  };
  parallel_for(numPts, f, "barycentric_coords");

  //source for calculation "https://people.sc.fsu.edu/~jburkardt/presentations/cg_lab_barycentric_tetrahedrons.pdf"
  return 0;
}
