//=============================================================================
//
//   Exercise code for the lecture "Geometric Modeling"
//   by Prof. Dr. Mario Botsch, TU Dortmund
//
//   Copyright (C) 2023 Computer Graphics Group, TU Dortmund.
//
//=============================================================================

#include "curvature.h"
#include <laplace.h>

//=============================================================================

void compute_mean_curvature(SurfaceMesh &mesh)
{
    auto curvature = mesh.vertex_property<Scalar>("v:curv");
    auto points = mesh.get_vertex_property<Point>("v:point");

    for (auto v : mesh.vertices())
    {
        curvature[v] = 0.0;

        if (mesh.is_isolated(v) || mesh.is_boundary(v))
            continue;

        /**  \todo Compute mean curvature of vertex v
         *  - mean curvature is related to Laplace of coordinate function
         *  - compare the uniform and cotangent Laplace
         **/
        

        Point sum(0,0,0);
        for (auto h : mesh.halfedges(v)) { 
            sum += (points[mesh.to_vertex(h)] - points[v])
            * cotan(mesh, mesh.edge(h));   
        }
        curvature[v] = norm(sum / area(mesh, v)) / 2;
    }

    curvature_to_texture_coordinates(mesh);
}

//-----------------------------------------------------------------------------

void compute_gauss_curvature(SurfaceMesh &mesh)
{
    auto curvature = mesh.vertex_property<Scalar>("v:curv");
    auto points = mesh.get_vertex_property<Point>("v:point");

    for (auto v : mesh.vertices())
    {
        curvature[v] = 0.0;

        if (mesh.is_isolated(v) || mesh.is_boundary(v))
            continue;

        /**  \todo Compute Gauss curvature of vertex v
         *  - acos() returns angle in radians
         *  - input of acos() has to be in [0,1]!
        **/

       Scalar sum(0);
        for (auto h1 : mesh.halfedges(v)) { 
            auto h2 = mesh.prev_halfedge(h1);
            auto a = points[mesh.to_vertex(h1)] - points[mesh.from_vertex(h1)];
            auto b = points[mesh.from_vertex(h2)] - points[mesh.to_vertex(h2)];
            sum += acos(dot(a, b) / (norm(a) * norm(b)));
        }
        curvature[v] = (2 * M_PI - sum) / area(mesh, v);
    }

    curvature_to_texture_coordinates(mesh);
}

//-----------------------------------------------------------------------------

void curvature_to_texture_coordinates(SurfaceMesh &mesh)
{
    auto curvature = mesh.get_vertex_property<Scalar>("v:curv");
    assert(curvature);

    // sort curvature values
    std::vector<Scalar> values;
    values.reserve(mesh.n_vertices());
    for (auto v : mesh.vertices())
        if (!mesh.is_isolated(v) && !mesh.is_boundary(v))
            values.push_back(curvature[v]);
    std::sort(values.begin(), values.end());
    unsigned int n = values.size() - 1;

    // clamp upper/lower 5%
    unsigned int i = n / 20;
    Scalar kmin = values[i];
    Scalar kmax = values[n - 1 - i];
    std::cout << "Curvate range: [" << kmin << ", " << kmax << "]\n";

    // generate 1D texture coordinates
    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    for (auto v : mesh.vertices())
    {
        tex[v] = TexCoord((curvature[v] - kmin) / (kmax - kmin), 0.0);
    }
}

//=============================================================================
