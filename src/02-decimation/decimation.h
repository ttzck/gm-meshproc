//=============================================================================
//
//   Exercise code for the lecture "Geometric Modeling"
//   by Prof. Dr. Mario Botsch, TU Dortmund
//
//   Copyright (C) 2023 Computer Graphics Group, TU Dortmund.
//
//=============================================================================
#pragma once
//=============================================================================

#include <pmp/SurfaceMesh.h>
#include "Quadric.h"
using namespace pmp;

//=============================================================================

/// convenience function for mesh decimation
/// simply constructs a Decimator object and calls initialize() and decimate().
void decimate(SurfaceMesh& _mesh, unsigned int _target_complexity);

//=============================================================================

/// Class containing decimation functionality
class Decimater
{
public:
    /// give a mesh in the constructor
    Decimater(SurfaceMesh& m);

    /// destructor
    ~Decimater();

    /// initialize the per-vertex error quadrics
    void initialize();

    /// decimate down to `n_vertices`
    void decimate(unsigned int n_vertices);

private:
    /// is collapsing the halfedge `h` allowed?
    bool is_collapse_legal(Halfedge h);

    /// what is the priority of collapsing the halfedge `h`?
    float collapse_priority(Halfedge h);

    /// compute current quadric error
    float quadric_error() const;

private:

    /// reference to the mesh
    SurfaceMesh& mesh;

    // custom properties we need
    VertexProperty<Point> points;
    VertexProperty<Quadric> quadrics;
    HalfedgeProperty<float> priority;
};

//=============================================================================
