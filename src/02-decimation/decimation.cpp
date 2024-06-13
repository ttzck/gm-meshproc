//=============================================================================
//
//   Exercise code for the lecture "Geometric Modeling"
//   by Prof. Dr. Mario Botsch, TU Dortmund
//
//   Copyright (C) 2023 Computer Graphics Group, TU Dortmund.
//
//=============================================================================

#include "decimation.h"
#include "Quadric.h"
#include <pmp/algorithms/SurfaceNormals.h>
#include <float.h>
using namespace pmp;

//=============================================================================

void decimate(SurfaceMesh &_mesh, unsigned int _target_complexity)
{
    Decimater deci(_mesh);
    deci.initialize();
    deci.decimate(_target_complexity);
}

//=============================================================================

Decimater::Decimater(SurfaceMesh &m)
    : mesh(m)
{
    // get properties for vertex positions
    points = mesh.vertex_property<Point>("v:point");

    // add properties for vertex quadric and halfedge priority
    quadrics = mesh.add_vertex_property<Quadric>("v:quadric");
    priority = mesh.add_halfedge_property<float>("h:prio");
}

//-----------------------------------------------------------------------------

Decimater::~Decimater()
{
    // clean-up properties we added in the constructor
    mesh.remove_vertex_property(quadrics);
    mesh.remove_halfedge_property(priority);
}

//-----------------------------------------------------------------------------

void Decimater::initialize()
{
    /** \todo initialize per-vertex quadrics
        - Vertex positions are stored in `points[v]`
        - Vertex quadrics are stored in `quadrics[v]`
        - See Quadric.h for the functions provided by the class Quadric.
        - SurfaceNormals::compute_face_normal(mesh, f) computes the normal for face f
    */

    for (auto v : mesh.vertices())
    {
        auto p = points[v];
        for (auto f : mesh.faces(v))
        {
            auto n = SurfaceNormals::compute_face_normal(mesh, f);
            quadrics[v] += Quadric(n, p);
        }
    }
}

//-----------------------------------------------------------------------------

bool Decimater::is_collapse_legal(Halfedge h)
{
    // collect two vertices and their positions
    Vertex v0 = mesh.from_vertex(h);
    Vertex v1 = mesh.to_vertex(h);
    Point p0 = points[v0];
    Point p1 = points[v1];

    // topological test
    if (!mesh.is_collapse_ok(h))
        return false;

    // boundary test
    if (mesh.is_boundary(v0) && !mesh.is_boundary(v1))
        return false;

    /** \todo test for normal flips
        - Simulate collapse and test whether any triangle flips over
        - `SurfaceNormals::compute_face_normal(mesh, f)` computes the face normal for face f`
    */

    for (auto f : mesh.faces(v0))
    {
        auto skip = false;
        for (auto v : mesh.vertices(f))
        {
            if (v == v1)
                skip = true;
        }
        if (skip)
            continue;

        auto n_prev = SurfaceNormals::compute_face_normal(mesh, f);
        points[v0] = p1;
        auto n_post = SurfaceNormals::compute_face_normal(mesh, f);
        points[v0] = p0;

        if (dot(n_prev, n_post) < 0.0)
            return false;
    }

    // collapse passed all tests -> ok
    return true;
}

//-----------------------------------------------------------------------------

float Decimater::collapse_priority(Halfedge h)
{
    // illegal collapses get infinite cost
    if (!is_collapse_legal(h))
        return FLT_MAX;

    /** \todo Compute (and return) the quadric error for the simulated collapse.
        - Get the quadric of a vertex v by calling quadrics[v]
        - See what the class Quadric provides
    */

    // collect two vertices and their positions
    Vertex v0 = mesh.from_vertex(h);
    Vertex v1 = mesh.to_vertex(h);

    auto quad = quadrics[v0] + quadrics[v1];

    return quad(points[v1]);
}

//-----------------------------------------------------------------------------

float Decimater::quadric_error() const
{
    float error(0);
    for (auto v : mesh.vertices())
    {
        error += quadrics[v](points[v]);
    }
    return error;
}

//-----------------------------------------------------------------------------

void Decimater::decimate(unsigned int _target_complexity)
{
    /** \todo Perform main decimation loop
        while not done
            get halfedge `hmin` with lowest cost
            check that mesh.is_collapse_ok(hmin) is true
            accumulate quadrics
            collapse halfedge hmin
            update priority of incident halfedges

        afterwards move each vertex to the optimal position wrt its quadric
        (note: inverting the quadric matrix might throw a numerical exception)
    */

    while (mesh.n_vertices() > _target_complexity)
    {
        auto pmin = FLT_MAX;
        Halfedge hmin;
        for (auto h : mesh.halfedges())
        {
            if (priority[h] < pmin)
            {
                hmin = h;
                pmin = priority[h];
            }
        }

        if (pmin == FLT_MAX)
            break;

        if (!mesh.is_collapse_ok(hmin))
        {
            priority[hmin] = FLT_MAX;
            continue;
        }

        Vertex v0 = mesh.from_vertex(hmin);
        Vertex v1 = mesh.to_vertex(hmin);
        quadrics[v1] += quadrics[v0];

        mesh.collapse(hmin);

        for(auto h1: mesh.halfedges(v1)) 
        {
            priority[h1] = collapse_priority(h1);
            auto h2 = mesh.opposite_halfedge(h1);
            priority[h2] = collapse_priority(h2);
        }
    }

    for (auto v : mesh.vertices()) {
        try
        {
            points[v] = quadrics[v].minimizer();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
}

//=============================================================================
