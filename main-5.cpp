#include "Common.h"
#include "OpenGLCommon.h"
#include "OpenGLMarkerObjects.h"
#include "OpenGLBgEffect.h"
#include "OpenGLMesh.h"
#include "OpenGLViewer.h"
#include "OpenGLWindow.h"
#include "TinyObjLoader.h"
#include "OpenGLSkybox.h"
#include <algorithm>
#include <iostream>
#include <random>
#include <unordered_set>
#include <vector>
#include <string>

#ifndef __Main_cpp__
#define __Main_cpp__

#ifdef __APPLE__
#define CLOCKS_PER_SEC 100000
#endif

class MyDriver : public OpenGLViewer
{
    std::vector<OpenGLTriangleMesh*> mesh_object_array;
    OpenGLBgEffect* bgEffect = nullptr;
    OpenGLSkybox* skybox = nullptr;
    clock_t startTime;

public:
    virtual void Initialize()
    {
        draw_axes = false;
        startTime = clock();
        OpenGLViewer::Initialize();
    }

    virtual void Initialize_Data()
    {
        //// Load all the shaders you need for the scene 

        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/basic.vert", "shaders/basic.frag", "basic");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/basic.vert", "shaders/environment.frag", "environment");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/terrain.vert", "shaders/terrain.frag", "terrain");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/skybox.vert", "shaders/skybox.frag", "skybox");
        //// Load all the textures you need for the scene

        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/spiritedaway.jpg", "sphere_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/electrictrain.png", "train_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/electrictrain_n.png", "train_normal");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/sky.png", "sky_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/Solid_white.png", "fence_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/fence_normal.jpg", "fence_normal");
        //// Add all the lights you need for the scene
        //// The four parameters are position, ambient, diffuse, and specular.

        opengl_window->Add_Light(Vector3f(3, 1, 3), Vector3f(0.1, 0.1, 0.1), Vector3f(1, 1, 1), Vector3f(0.5, 0.5, 0.5));
        opengl_window->Add_Light(Vector3f(0, 0, -5), Vector3f(0.1, 0.1, 0.1), Vector3f(0.9, 0.9, 0.9), Vector3f(0.5, 0.5, 0.5));
        opengl_window->Add_Light(Vector3f(-5, 1, 3), Vector3f(0.1, 0.1, 0.1), Vector3f(0.9, 0.9, 0.9), Vector3f(0.5, 0.5, 0.5));
        //// Sky box
       
        {
            const std::vector<std::string> cubemap_files{
               "cubemap/px.jpeg",     //// + X
               "cubemap/nx.jpeg",     //// - X
               "cubemap/py.jpeg",     //// + Y
               "cubemap/ny.jpeg",     //// - Y
               "cubemap/pz.jpeg",     //// + Z
               "cubemap/nz.jpeg",     //// - Z
            };
            OpenGLTextureLibrary::Instance()->Add_CubeMap_From_Files(cubemap_files, "cube_map");

            skybox = Add_Interactive_Object<OpenGLSkybox>();
            skybox->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("skybox"));
            skybox->Initialize();
        }
        

        ////Sky sphere

        {
            //// create object by reading an obj mesh
            auto sphere = Add_Obj_Mesh_Object("obj/sphere.obj");
            float theta = radians(90.f);
            //// set object's transform
            Matrix4f t, s, r;
            t << 1, 0, 0, 0,
                0, 1, 0, 4.5,
                0, 0, 1, -1.,
                0, 0, 0, 1;
            s << 12, 0, 0, 0,
                0, 12, 0, 0,
                0, 0, 12, 0,
                0, 0, 0, 1;
            r << cos(theta), 0., -sin(theta), 0.,
                0., 1, 0., 0.,
                sin(theta), 0, cos(theta), 0.,
                0., 0., 0., 1.;
            sphere->Set_Model_Matrix(t * s * r);

            //// set object's material
            sphere->Set_Ka(Vector3f(0.1, 0.1, 0.1));
            sphere->Set_Kd(Vector3f(0.7, 0.7, 0.7));
            sphere->Set_Ks(Vector3f(2, 2, 2));
            sphere->Set_Shininess(128);

            //// bind texture to object
            sphere->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("sky_color"));

            //// bind shader to object
            sphere->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("basic"));
        }
        {
            //// create water plane
            auto terrain = Add_Obj_Mesh_Object("obj/plane.obj");
            float theta = radians(90.f);
            //// set object's transform
            Matrix4f r, s, t;
            r << 1., 0., 0., 0.,
                0., cos(theta), sin(theta), 0.,
                0., -sin(theta), cos(theta), 0.,
                0., 0., 0., 1.;
            s << 10., 0, 0, 0,
                0, .1, 0, 0,
                0, 0, 15., 0,
                0, 0, 0, 1;
            t << 1, 0, 0, -15,
                0, 1, 0, -.5,
                0, 0, 1, 40,
                0, 0, 0, 1;
            terrain->Set_Model_Matrix(t * s * r);

            //// set object's material
            terrain->Set_Ka(Vector3f(0.1f, 0.1f, 0.1f));
            terrain->Set_Kd(Vector3f(0.3f, 0.6f, 0.9f));
            terrain->Set_Ks(Vector3f(1, 1, 1));
            terrain->Set_Shininess(128.f);

            //// bind shader to object (we do not bind texture for this object because we create noise for texture)
            terrain->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("terrain"));
        }

        //// shading (ray-tracing) a sphere with environment mapping
        {
            //// create object by reading an obj mesh
            auto train = Add_Obj_Mesh_Object("obj/etrain.obj");
            float theta = radians(90.f);
            //float theta2 = radians(30.f);
            //// set object's transform
            Matrix4f t, r;
            t << .05, 0, 0, 0,
                0, .05, 0, -.5,
                0, 0, .05, 1,
                0, 0, 0, 1;
            r << cos(theta), 0, -sin(theta), 0,
                0., 1., 0., 0,
                sin(theta), 0., cos(theta), 0,
                0, 0, 0, 1;
            train->Set_Model_Matrix(t * r);

            //// bind shader to object
            train->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("train_color"));
            train->Add_Texture("tex_normal", OpenGLTextureLibrary::Get_Texture("train_normal"));
            train->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("basic")); // bind shader to object
        }
        {
            //// create fence
            auto fence = Add_Obj_Mesh_Object("obj/Wooden_Fence1.obj");
            float theta = radians(90.f);
            //// set object's transform
            Matrix4f x,r;
            x << .1, 0, 0, -1.6,
                0, .03, 0, -.49,
                0, 0, .1, .8,
                0, 0, 0, 1;
            r << cos(theta), 0, -sin(theta), 0,
                0, 1, 0, 0,
                sin(theta), 0, cos(theta), 0,
                0, 0, 0, 1;
            fence->Set_Model_Matrix(x * r);
            fence->Set_Ka(Vector3f(1., 1., 1.));
            fence->Set_Kd(Vector3f(1., 1., 1.));
            fence->Set_Ks(Vector3f(1, 1, 1));
            fence->Set_Shininess(128);
            fence->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("fence_color"));
            fence->Add_Texture("tex_normal", OpenGLTextureLibrary::Get_Texture("fence_normal"));
            fence->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("basic")); // bind shader to object
        }
        {
            //// create second part of fence
            auto fence2 = Add_Obj_Mesh_Object("obj/Wooden_Fence1.obj");
            float theta = radians(90.f);
            //// set object's transform
            Matrix4f x, r;
            x << .1, 0, 0, 2.2,
                0, .03, 0, -.47,
                0, 0, .1, .8,
                0, 0, 0, 1;
            r << cos(theta), 0, -sin(theta), 0,
                0, 1, 0, 0,
                sin(theta), 0, cos(theta), 0,
                0, 0, 0, 1;
            fence2->Set_Model_Matrix(x * r);
            fence2->Set_Ka(Vector3f(1., 1., 1.));
            fence2->Set_Kd(Vector3f(1., 1., 1.));
            fence2->Set_Ks(Vector3f(1, 1, 1));
            fence2->Set_Shininess(128);
            fence2->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("fence_color"));
            fence2->Add_Texture("tex_normal", OpenGLTextureLibrary::Get_Texture("fence_normal"));
            fence2->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("basic")); // bind shader to object
        }
        //// This for-loop updates the rendering model for each object on the list
        for (auto& mesh_obj : mesh_object_array) {
            Set_Polygon_Mode(mesh_obj, PolygonMode::Fill);
            Set_Shading_Mode(mesh_obj, ShadingMode::TexAlpha);
            mesh_obj->Set_Data_Refreshed();
            mesh_obj->Initialize();
        }
        Toggle_Play();
    }
    OpenGLTriangleMesh* Add_Obj_Mesh_Object(std::string obj_file_name)
    {
        auto mesh_obj = Add_Interactive_Object<OpenGLTriangleMesh>();
        Array<std::shared_ptr<TriangleMesh<3>>> meshes;
        // Obj::Read_From_Obj_File(obj_file_name, meshes);
        Obj::Read_From_Obj_File_Discrete_Triangles(obj_file_name, meshes);

        mesh_obj->mesh = *meshes[0];
        std::cout << "load tri_mesh from obj file, #vtx: " << mesh_obj->mesh.Vertices().size() << ", #ele: " << mesh_obj->mesh.Elements().size() << std::endl;

        mesh_object_array.push_back(mesh_obj);
        return mesh_obj;
    }

    //// Go to next frame
    virtual void Toggle_Next_Frame()
    {
        for (auto& mesh_obj : mesh_object_array)
            mesh_obj->setTime(GLfloat(clock() - startTime) / CLOCKS_PER_SEC);

        if (bgEffect) {
            bgEffect->setResolution((float)Win_Width(), (float)Win_Height());
            bgEffect->setTime(GLfloat(clock() - startTime) / CLOCKS_PER_SEC);
            bgEffect->setFrame(frame++);
        }
        OpenGLViewer::Toggle_Next_Frame();
    }

    virtual void Run()
    {
        OpenGLViewer::Run();
    }
};

int main(int argc, char *argv[])
{
    MyDriver driver;
    driver.Initialize();
    driver.Run();
}

#endif