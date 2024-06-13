//=============================================================================
//
//   Exercise code for the lecture "Geometric Modeling"
//   by Prof. Dr. Mario Botsch, TU Dortmund
//
//   Copyright (C) 2023 Computer Graphics Group, TU Dortmund.
//
//=============================================================================

#include <Viewer.h>
#include <01-reconstruction/reconstruction.h>
#include <02-decimation/decimation.h>
#include <03-curvature/curvature.h>
#include <04-smoothing/smoothing.h>
#include <05-parameterization/parameterization.h>
#include <pmp/algorithms/SurfaceNormals.h>
#include <pmp/algorithms/decimation.h>
#include <pmp/Timer.h>
#include <imgui.h>
#include <fstream>

//=============================================================================

const std::string point_dir = POINTSET_DIRECTORY;
const std::string mesh_dir = MESH_DIRECTORY;

std::vector<std::string> pointsets = {
    "bunny.pts", "mario_hq.pts", "mario_lq.pts", "mario_laser.pts",
    "open_sphere.pts", "sphere.pts", "plane.pts", "myscan.txt" };

std::vector<std::string> meshes = {
    "bunny.off", "boar.off", "blubb.off", "spot.off",
    "max.off", "open_bunny.off", "dragon.off",
    "hemisphere.off", "sphere.off", "cylinder.off",
    "plane-regular.off", "plane-irregular.off" };

//=============================================================================

Viewer::Viewer(const char *title, int width, int height)
    : pmp::MeshViewer(title, width, height)
{
    draw_pointset_ = false;
    draw_mesh_ = true;
    run_parameterization_ = false;
    draw_uv_layout_ = false;
    parameterization_uniform_ = false;

    // setup draw modes for viewer
    clear_draw_modes();
    add_draw_mode("Smooth Shading");
    add_draw_mode("Hidden Line");
    add_draw_mode("Texture");
    set_draw_mode("Smooth Shading");

    // check which pointsets exist
    {
        std::ifstream ifs;
        auto filenames = pointsets;
        pointsets.clear();
        for (auto filename : filenames)
        {
            ifs.open(std::string(POINTSET_DIRECTORY) + filename);
            if (ifs)
                pointsets.push_back(filename);
            ifs.close();
        }
    }

    // check which meshes exist
    {
        std::ifstream ifs;
        auto filenames = meshes;
        meshes.clear();
        for (auto filename : filenames)
        {
            ifs.open(std::string(MESH_DIRECTORY) + filename);
            if (ifs)
                meshes.push_back(filename);
            ifs.close();
        }
    }
}

//-----------------------------------------------------------------------------

bool Viewer::load_data(const char *_filename)
{
    std::string filename(_filename);
    std::string::size_type dot(filename.rfind("."));
    std::string ext = filename.substr(dot + 1, filename.length() - dot - 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

    bool ok;
    mesh_.clear();

    // load as pointset
    ok = pointset_.read_data(_filename);
    if (!ok)
    {
        std::cerr << "cannot read file " << _filename << std::endl;
        return false;
    }

    // load mesh
    if (ext == "obj" || ext == "off")
    {
        try
        {
            MeshViewer::load_mesh(_filename);
        }
        catch (const std::exception &e)
        {
            std::cerr << "cannot read file " << _filename << std::endl;
            return false;
        }
    }

    // update scene center and bounds
    BoundingBox bb = pointset_.bounds();
    set_scene((vec3)bb.center(), 0.5 * bb.size());

    if (mesh_.n_vertices() != pointset_.n_vertices())
        draw_pointset_ = true;

    filename_ = filename;

    return true;
}

//-----------------------------------------------------------------------------

void Viewer::draw(const std::string &draw_mode)
{
    // draw the triangle mesh
    if (draw_mesh_)
    {
        MeshViewer::draw(draw_mode);
    }

    // draw the point set
    if (draw_pointset_)
    {
        pointset_.draw(projection_matrix_, modelview_matrix_, "Points");
    }

    // draw the UV coordinates of the mesh in the upper right corner
    if (draw_uv_layout_)
    {
        glClear(GL_DEPTH_BUFFER_BIT);

        // setup viewport
        GLint size = std::min(width(), height()) / 3;
        glViewport(width() - size - 1, height() - size - 1, size, size);

        // setup matrices
        mat4 P = ortho_matrix(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
        mat4 M = mat4::identity();

        // draw mesh once more
        mesh_.draw(P, M, "Texture Layout");

        // restore viewport
        glViewport(0, 0, width(), height());
    }
}

//-----------------------------------------------------------------------------

void Viewer::keyboard(int key, int scancode, int action,
                      int mods)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
        return;

    switch (key)
    {
        case GLFW_KEY_BACKSPACE: // reload model
        {
            load_data(filename_.c_str());
            break;
        }
        default:
        {
            MeshViewer::keyboard(key, scancode, action, mods);
            break;
        }
    }
}

//-----------------------------------------------------------------------------

void Viewer::process_imgui()
{
    if (ImGui::CollapsingHeader("Load pointset or mesh",
                                ImGuiTreeNodeFlags_DefaultOpen))
    {
        static std::string current_pointset = "- load pointset -";
        static std::string current_mesh = "- load mesh -";

        ImGui::Text("Point Cloud");
        ImGui::Indent(10);

        ImGui::PushItemWidth(150);
        if (ImGui::BeginCombo("##PC to load", current_pointset.c_str()))
        {
            ImGui::Selectable("- load pointset -", false);
            for (auto item : pointsets)
            {
                bool is_selected = (current_pointset == item);
                if (ImGui::Selectable(item.c_str(), is_selected))
                {
                    current_mesh = "- load mesh -";
                    current_pointset = item;
                    std::string fn = point_dir + current_pointset;
                    load_data(fn.c_str());
                }
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();

        // output point statistics
        ImGui::BulletText("%d points", (int)pointset_.points_.size());
        ImGui::Unindent(10);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Mesh");
        ImGui::Indent(10);

        ImGui::PushItemWidth(150);
        if (ImGui::BeginCombo(
                "##Mesh to load",
                current_mesh
                    .c_str())) // The second parameter is the label previewed before opening the combo.
        {
            ImGui::Selectable("- load mesh -", false);
            for (auto item : meshes)
            {
                bool is_selected = (current_mesh == item);
                if (ImGui::Selectable(item.c_str(), is_selected))
                {
                    current_pointset = "- load pointset -";
                    current_mesh = item;
                    std::string fn = mesh_dir + current_mesh;
                    load_data(fn.c_str());
                    draw_pointset_ = false;
                }
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();

        // output mesh statistics
        ImGui::BulletText("%d vertices", (int)mesh_.n_vertices());
        ImGui::BulletText("%d edges", (int)mesh_.n_edges());
        ImGui::BulletText("%d faces", (int)mesh_.n_faces());
        ImGui::Unindent(10);

        // control crease angle
        ImGui::PushItemWidth(100);
        ImGui::SliderFloat("Crease Angle", &crease_angle_, 0.0f, 180.0f,
                           "%.0f");
        ImGui::PopItemWidth();
        if (crease_angle_ != mesh_.crease_angle())
        {
            mesh_.set_crease_angle(crease_angle_);
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Draw Options"))
    {
        ImGui::Text("Point Cloud");
        ImGui::Spacing();

        if (!pointset_.points_.empty())
        {
            ImGui::Indent(10);
            ImGui::Checkbox("Draw point cloud", &draw_pointset_);

            if (draw_pointset_)
            {
                // control point size
                int point_size = pointset_.point_size();
                ImGui::PushItemWidth(100);
                ImGui::Text("Point Size:");
                if (ImGui::SliderInt("##PointSize", &point_size, 1, 10))
                {
                    pointset_.set_point_size(point_size);
                }
                ImGui::PopItemWidth();
            }
            ImGui::Unindent(10);
            ImGui::Spacing();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Mesh");
        ImGui::Spacing();
        ImGui::Indent(10);

        if (mesh_.n_vertices() > 0)
        {
            ImGui::Checkbox("Draw mesh", &draw_mesh_);

            if (draw_mesh_)
            {
                ImGui::Text("Draw modes:");
                ImGui::PushItemWidth(150);
                int mode = static_cast<int>(draw_mode_);

                for (size_t i = 0; i < draw_mode_names_.size(); ++i)
                {
                    ImGui::RadioButton(draw_mode_names_[i].c_str(), &mode, i);
                }
                ImGui::PopItemWidth();
                if (mode != static_cast<int>(draw_mode_))
                {
                    draw_mode_ = static_cast<unsigned int>(mode);
                }
            }
        }
        else
        {
            ImGui::Text("Reconstruct first!");
        }
        ImGui::Unindent(10);
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Surface Reconstruction"))
    {
        if (!pointset_.points_.empty())
        {
            // Hoppe parameters
            static int hoppe_resolution = 50;
            static int hoppe_nneighbors = 1;
            ImGui::PushItemWidth(100);
            ImGui::Text("Grid resolution");
            ImGui::SliderInt("##MC Resolution", &hoppe_resolution, 10, 200);
            ImGui::PopItemWidth();

            if (ImGui::Button("Hoppe reconstruction"))
            {
                Timer timer; 
                timer.start();

                reconstruct_hoppe(pointset_, mesh_, hoppe_resolution, hoppe_nneighbors);
                update_mesh();
                draw_pointset_ = false;

                timer.stop();
                std::cout << "Reconstruction took " << timer << std::endl;
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Poisson parameters
            static int octree_depth = 7;
            ImGui::PushItemWidth(100);
            ImGui::Text("Octree depth");
            ImGui::SliderInt("##Poisson OD", &octree_depth, 5, 10);
            ImGui::PopItemWidth();

            if (ImGui::Button("Poisson reconstruction"))
            {
                Timer timer; 
                timer.start();

                reconstruct_poisson(pointset_, mesh_, octree_depth, 8, 2.0);
                update_mesh();
                draw_pointset_ = false;

                timer.stop();
                std::cout << "Reconstruction took " << timer << std::endl;
            }
        }
        else
        {
            ImGui::Text("Load point cloud first");
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Decimation"))
    {
        if (mesh_.n_vertices() > 0)
        {
            static int target_percentage = 10;

            ImGui::PushItemWidth(100);
            ImGui::Text("Vertices to remain");
            ImGui::SliderInt("##Percentage", &target_percentage, 1, 99, "%d%%");
            ImGui::PopItemWidth();

            ImGui::Spacing();

            if (ImGui::Button("Decimate"))
            {
                Timer timer; 
                timer.start();
                ::decimate(mesh_, mesh_.n_vertices() * 0.01 * target_percentage);
                update_mesh();
                timer.stop();
                std::cout << "Decimation took " << timer << std::endl;
            }
            ImGui::SameLine();
            if (ImGui::Button("PMP Decimate"))
            {
                Timer timer; 
                timer.start();
                pmp::decimate(mesh_, mesh_.n_vertices() * 0.01 * target_percentage, 10);
                update_mesh();
                timer.stop();
                std::cout << "Decimation took " << timer << std::endl;
            }
        }
        else
        {
            ImGui::Text("Reconstruct/load mesh first!");
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Curvature"))
    {
        if (mesh_.n_vertices() > 0)
        {
            if (ImGui::Button("Mean Curvature"))
            {
                compute_mean_curvature(mesh_);
                mesh_.use_cold_warm_texture();
                update_mesh();
                set_draw_mode("Texture");
            }

            if (ImGui::Button("Gauss Curvature"))
            {
                compute_gauss_curvature(mesh_);
                mesh_.use_cold_warm_texture();
                update_mesh();
                set_draw_mode("Texture");
            }
        }
        else
        {
            ImGui::Text("Reconstruct/load mesh first!");
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Smoothing"))
    {
        if (mesh_.n_vertices() > 0)
        {
            static int iterations = 10;
            static float timestep = 0.001;

            ImGui::PushItemWidth(100);
            ImGui::Text("#Iterations");
            ImGui::SameLine();
            ImGui::SliderInt("##Iterations", &iterations, 1, 1000);
            ImGui::PopItemWidth();

            if (ImGui::Button("Explicit smoothing (uniform)"))
            {
                explicit_smoothing(mesh_, iterations, true);
                update_mesh();
            }
            if (ImGui::Button("Explicit smoothing (cotan)"))
            {
                explicit_smoothing(mesh_, iterations, false);
                update_mesh();
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::PushItemWidth(100);
            ImGui::Text("Time-step");
            ImGui::SameLine();
            ImGui::SliderFloat("##TimeStep", &timestep, 1e-5, 0.1, "%.5f", 3.5);
            ImGui::PopItemWidth();
            ImGui::Spacing();

            if (ImGui::Button("Implicit smoothing (cotan)"))
            {
                Timer timer; 
                timer.start();
                Scalar dt = timestep * radius_ * radius_;
                implicit_smoothing(mesh_, dt);
                update_mesh();
                timer.stop();
                std::cout << "Implicit smoothing took " << timer << std::endl;
            }
        }
        else
        {
            ImGui::Text("Reconstruct/load mesh first!");
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Parameterization"))
    {
        if (mesh_.n_vertices() > 0)
        {
            draw_uv_layout_ = true;

            int w = parameterization_uniform_;
            ImGui::RadioButton("Cotan Laplace", &w, 0);
            ImGui::RadioButton("Uniform Laplace", &w, 1);
            parameterization_uniform_ = w;

            if (!run_parameterization_)
            {
                if (ImGui::Button("Start parameterization"))
                {
                    if (parameterize_boundary(mesh_))
                    {
                        run_parameterization_ = true;
                        update_mesh();
                    }
                    else
                    {
                        std::cerr << "Cannot parameterize boundary\n";
                    }
                }
            }
            else
            {
                if (ImGui::Button("Stop parameterization"))
                {
                    run_parameterization_ = false;
                }
            }

            if (ImGui::Button("Direct parameterization"))
            {

                if (parameterize_boundary(mesh_))
                {
                    run_parameterization_ = false;

                    Timer timer; 
                    timer.start();
                    parameterize_direct(mesh_);
                    timer.stop();
                    std::cout << "Parameterization took " << timer << std::endl;

                    update_mesh();
                    set_draw_mode("Texture");

                }

            }
        }
        else
        {
            ImGui::Text("Reconstruct/load mesh first!");
        }
    }
    else
    {
        draw_uv_layout_ = false;
    }


    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Write Mesh"))
    {
        if (mesh_.n_vertices() > 0)
        {
            if (ImGui::Button("Write .off"))
            {
                mesh_.write("mesh.off");
            }

            if (ImGui::Button("Write .obj"))
            {
                mesh_.write("mesh.obj");
            }
        }
        else
        {
            ImGui::Text("Reconstruct mesh first!");
        }
    }
}

//-----------------------------------------------------------------------------

void Viewer::do_processing() 
{
  if (run_parameterization_) 
  {
    parameterize_iterative(mesh_, parameterization_uniform_, 100);
    update_mesh();
    set_draw_mode("Texture");
  }
}

//=============================================================================
