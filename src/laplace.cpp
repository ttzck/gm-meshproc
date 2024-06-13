//=============================================================================
//
//   Exercise code for the lecture "Geometric Modeling"
//   by Prof. Dr. Mario Botsch, TU Dortmund
//
//   Copyright (C) 2023 Computer Graphics Group, TU Dortmund.
//
//=============================================================================

#include "laplace.h"

//=============================================================================

Scalar area(const SurfaceMesh &mesh, Face f)
{
    auto fv = mesh.vertices(f);
    const Point &p0 = mesh.position(*fv);
    const Point &p1 = mesh.position(*(++fv));
    const Point &p2 = mesh.position(*(++fv));
    return 0.5f * norm(cross(p1 - p0, p2 - p0));
}

//-----------------------------------------------------------------------------

Scalar area(const SurfaceMesh &mesh, Vertex v)
{
    Scalar a = 0;
    for (auto f : mesh.faces(v))
        a += area(mesh, f);
    return a / 3.0;
}

//-----------------------------------------------------------------------------

Scalar cotan(const SurfaceMesh &mesh, Edge e)
{
    Scalar weight(0);
    auto points = mesh.get_vertex_property<Point>("v:point");

    /**  \todo Compute the cotangent weight for the edge e,
     *   defined by the two angles opposite to edge e.
     *   Hints:
     *   - in case of a boundary edge, there is only one angle to compute
     *   - acos and similar functions return angles in radians
     **/

    auto h0 = mesh.halfedge(e, 0);
    auto h1 = mesh.halfedge(e, 1);

    auto v_A = mesh.to_vertex(h0);
    auto v_B = mesh.to_vertex(h1);
    auto p_A = points[v_A];
    auto p_B = points[v_B];

    auto cot_alpha = 0.0;
    if (!mesh.is_boundary(h0))
    {
        auto v_alpha = mesh.to_vertex(mesh.next_halfedge(h0));
        auto p_alpha = points[v_alpha];
        auto x_alpha = p_A - p_alpha;
        auto y_alpha = p_B - p_alpha;
        cot_alpha = dot(x_alpha, y_alpha) / norm(cross(x_alpha, y_alpha));
    }

    auto cot_beta = 0.0;
    if (!mesh.is_boundary(h1))
    {
        auto v_beta = mesh.to_vertex(mesh.next_halfedge(h1));
        auto p_beta = points[v_beta];
        auto x_beta = p_A - p_beta;
        auto y_beta = p_B - p_beta;
        cot_beta = dot(x_beta, y_beta) / norm(cross(x_beta, y_beta));
    }

    weight = (cot_alpha + cot_beta) / 2 ;

    // let's hope this does not happen
    assert(!std::isnan(weight));
    assert(!std::isinf(weight));

    return weight;
}

//-----------------------------------------------------------------------------

void setup_mass_matrix(const SurfaceMesh &mesh, SparseMatrix &M)
{
    const unsigned int n = mesh.n_vertices();
    std::vector<Eigen::Triplet<double>> triplets;
    triplets.reserve(n);

    for (auto vi : mesh.vertices())
    {
        triplets.emplace_back(vi.idx(), vi.idx(), area(mesh, vi));
    }

    M.resize(n, n);
    M.setFromTriplets(triplets.begin(), triplets.end());
}

//-----------------------------------------------------------------------------

void setup_stiffness_matrix(const SurfaceMesh &mesh, SparseMatrix &S)
{
    const unsigned int n = mesh.n_vertices();
    std::vector<Eigen::Triplet<double>> triplets;
    triplets.reserve(7 * n);

    for (auto vi : mesh.vertices())
    {
        Scalar sum_weights = 0.0;
        for (auto h : mesh.halfedges(vi))
        {
            Vertex vj = mesh.to_vertex(h);
            Scalar w = cotan(mesh, mesh.edge(h));
            sum_weights += w;
            triplets.emplace_back(vi.idx(), vj.idx(), w);
        }
        triplets.emplace_back(vi.idx(), vi.idx(), -sum_weights);
    }

    S.resize(n, n);
    S.setFromTriplets(triplets.begin(), triplets.end());
}

//-----------------------------------------------------------------------------

void setup_selector_matrices(const SurfaceMesh &mesh,
                             VertexProperty<bool> is_selected,
                             SparseMatrix &S_in,
                             SparseMatrix &S_out)
{
    std::vector<Triplet> triplets_in, triplets_out;

    int row_in = 0, row_out = 0;
    for (auto v : mesh.vertices())
    {
        if (is_selected[v])
        {
            triplets_in.emplace_back(Triplet(row_in++, v.idx(), 1));
        }
        else
        {
            triplets_out.emplace_back(Triplet(row_out++, v.idx(), 1));
        }
    }

    S_in.resize(row_in, mesh.n_vertices());
    S_in.setFromTriplets(triplets_in.begin(), triplets_in.end());

    S_out.resize(row_out, mesh.n_vertices());
    S_out.setFromTriplets(triplets_out.begin(), triplets_out.end());
}

//=============================================================================
