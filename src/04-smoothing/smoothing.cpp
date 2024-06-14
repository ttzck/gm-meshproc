//=============================================================================
//
//   Exercise code for the lecture "Geometric Modeling"
//   by Prof. Dr. Mario Botsch, TU Dortmund
//
//   Copyright (C) 2023 Computer Graphics Group, TU Dortmund.
//
//=============================================================================

#include "smoothing.h"
#include <laplace.h>
#include <Eigen/Dense>
#include <Eigen/Sparse>

#define SPARSE 1

//=============================================================================


void explicit_smoothing(SurfaceMesh& mesh,
                        unsigned int N,
                        bool use_uniform_laplace)
{
    if (!mesh.n_vertices())
        return;

    auto points = mesh.get_vertex_property<Point>("v:point");
    auto eweight = mesh.edge_property<Scalar>("e:cotan");
    auto laplace = mesh.vertex_property<Point>("v:laplace");

    /**  \todo Perform N iterations of explicit Laplacian smoothing
     *   - pre-compute cotan weights (keep them fixed for all N iterations)
     *   - in each iteration, first compute and store Laplace per vertex,
     *     then move each vertex in direction of its Laplace
     *   - experiment with different time-steps; when does it blow up?
     **/

    for(auto e : mesh.edges()) {
        eweight[e] = use_uniform_laplace ? 1 : cotan(mesh, e);
    }

    for(unsigned int i = 0; i < N; i++) {
        for (auto v : mesh.vertices()) {
            laplace[v] = Point(0,0,0);
            for (auto h : mesh.halfedges(v)) {
                    laplace[v] += eweight[mesh.edge(h)] * (points[mesh.to_vertex(h)] - points[v]);
            }
            laplace[v] /= area(mesh, v);
        }
        
        for (auto v : mesh.vertices()) {
            points[v] += laplace[v] * 0.00001;
        }
    }




    // clean-up custom properties
    mesh.remove_edge_property(eweight);
    mesh.remove_vertex_property(laplace);
}

//-----------------------------------------------------------------------------

void implicit_smoothing(SurfaceMesh& mesh,
                        Scalar dt)
{
    if (!mesh.n_vertices())
        return;

    auto points = mesh.get_vertex_property<Point>("v:point");
    const unsigned int n = mesh.n_vertices();

    /**  \todo Perform one iteration of implicit Laplacian smoothing with time-step dt
     *   - Number of vertices is N
     *   - Setup NxN-matrix A
     *   - Setup Nx3-matrix B
     *   - Solve A*X=B for Nx3-matrix of new vertex positions
     *   - Copy positions from X to mesh
     *   - This should work for all time-steps dt!
     *   - Let's use the cotan weights for this task only
     **/
}

//=============================================================================
