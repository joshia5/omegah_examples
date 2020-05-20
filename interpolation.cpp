#include <Omega_h_file.hpp>
#include <Omega_h_library.hpp>
#include <Omega_h_mesh.hpp>
#include <Omega_h_for.hpp>
#include <Omega_h_shape.hpp>
using namespace Omega_h;

Read<Real> get_barycentric(Mesh mesh, Read<LO> elmIds, Read<Real> p) {
  const auto dim = mesh.dim();
  const int verts_per_tet = 4;
  auto cell2vert = mesh.ask_elem_verts();
  auto vert_coords = mesh.coords();
  Write<Real> xi_w(verts_per_tet*elmIds.size(),"xi_w");

  auto calculate_barycentric = OMEGA_H_LAMBDA(LO i) {
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

    xi_w[verts_per_tet*i+0] = ((p_v-b_v)*normal_a)/((a_v-b_v)*normal_a);
    xi_w[verts_per_tet*i+1] = ((p_v-c_v)*normal_b)/((b_v-c_v)*normal_b);
    xi_w[verts_per_tet*i+2] = ((p_v-d_v)*normal_c)/((c_v-d_v)*normal_c);
    xi_w[verts_per_tet*i+3] = ((p_v-a_v)*normal_d)/((d_v-a_v)*normal_d);

  };
  parallel_for(elmIds.size(), calculate_barycentric, "calculate_barycentric");
  Read<Real> xi(xi_w);
  return xi;
  //source for calculation "https://people.sc.fsu.edu/~jburkardt/presentations/cg_lab_barycentric_tetrahedrons.pdf"
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

  const int verts_per_tet = 4;
  auto cell2vert = mesh.ask_elem_verts();
  auto vert_coords = mesh.coords();
  const int numPts = mesh.nelems();
  Write<LO> elmIds_w(numPts, "elmIds");
  Write<Real> p_w_centroid(numPts*3, "p_w_centroid");
  Write<Real> p_w_vertex(numPts*3, "p_w_vertex");

  auto get_points = OMEGA_H_LAMBDA(LO i) {
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
      p_w_centroid[i*dim+k] = (a[k]+b[k]+c[k]+d[k])/verts_per_tet;
      p_w_vertex[i*dim+k] = a[k];
    }
  };
  parallel_for(numPts, get_points, "get_points");

  Read<LO> elmIds(elmIds_w);
  Read<Real> p_centroid(p_w_centroid);
  Read<Real> p_vertex(p_w_vertex);
  Read<Real> xi_centroid = get_barycentric(mesh, elmIds, p_centroid);
  Read<Real> xi_vertex = get_barycentric(mesh, elmIds, p_vertex);

  auto assertion = OMEGA_H_LAMBDA(LO i) {
    const int verts_per_tet = 4;
    const double error = 1e-6;
    for (int j=0; j<verts_per_tet; ++j) {
      assert((xi_centroid[verts_per_tet*i+j]-0.25) < error);
    }
    assert((xi_vertex[verts_per_tet*i+0]-1) < error);
    assert((xi_vertex[verts_per_tet*i+1]-0) < error);
    assert((xi_vertex[verts_per_tet*i+2]-0) < error);
    assert((xi_vertex[verts_per_tet*i+3]-0) < error);
  };
  parallel_for(numPts, assertion, "assertion");

  return 0;
}
