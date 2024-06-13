//=============================================================================
//
//   Exercise code for the lecture "Geometric Modeling"
//   by Prof. Dr. Mario Botsch, TU Dortmund
//
//   Copyright (C) 2023 Computer Graphics Group, TU Dortmund.
//
//=============================================================================

#include "parameterization.h"
#include <laplace.h>

//=============================================================================

bool parameterize_boundary(SurfaceMesh &mesh)
{
    // get properties
    auto points = mesh.vertex_property<Point>("v:point");
    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    auto eweight = mesh.edge_property<Scalar>("e:cotan");

    // Initialize all texture coordinates to the origin.
    for (auto v : mesh.vertices())
        tex[v] = TexCoord(0, 0);

    // pre-compute cotan weight per edge
    for (auto e : mesh.edges())
        eweight[e] = cotan(mesh, e);

        /** \todo Map boundary loop to unit circle in texture domain
         *  - walk around boundary and collect boundary vertices
         *  - map each boundary vertex to point on unit circle
         *  - ratio of boundary edge lengths in 3D should be the same as in 2D
         */

    // map from unit circle [-1,1]^2 to [0,1]^2
    for (auto v : mesh.vertices())
    {
        tex[v] *= 0.5;
        tex[v] += TexCoord(0.5, 0.5);
    }

    return true;
}

//-----------------------------------------------------------------------------

void parameterize_iterative(SurfaceMesh &mesh, bool use_uniform_laplace, unsigned int n)
{
    auto points = mesh.vertex_property<Point>("v:point");
    auto tex = mesh.vertex_property<TexCoord>("v:tex");
    auto eweight = mesh.edge_property<Scalar>("e:cotan");

    /** \todo Perform n iterations of iterative harmonic parameterization
     *  - Keep boundary vertices fixed
     *  - Move interior vertices to weighted barycenter
     */
}

//-----------------------------------------------------------------------------

void parameterize_direct(SurfaceMesh &mesh)
{
    const int n = mesh.n_vertices();

    // we assume that boundary constraints are precomputed!
    auto tex = mesh.vertex_property<TexCoord>("v:tex");

    /** \todo Computer harmonic parameterization with direct solver
     *  - Keep boundary vertices fixed
     *  - Setup cotangent stiffness matrix
     *  - Setup right-hand side
     *  - Somehow move known vertices to the right-hand side
     *  - Solve system, copy solution to unknown vertices
     * 
     *  Hint: See file `laplace.h` for some utility functions
     */
}

//=============================================================================
