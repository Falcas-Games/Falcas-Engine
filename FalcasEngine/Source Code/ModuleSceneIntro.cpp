#pragma once
#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleCentralEditor.h"
#include "ModuleCamera3D.h"
#include <gl/GL.h>
#include "External Libraries/Assimp/Assimp/include/cimport.h"
#include "External Libraries/Assimp/Assimp/include/postprocess.h"
#include "External Libraries/Assimp/Assimp/include/scene.h"
#include "Console.h"
#include "Mesh.h"
#include "Shape.h"



ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	total_game_objects = total_cilinder = total_cone = total_cube = total_empty = total_plane = total_prism = total_rect_pyr = total_sphere = total_sqr_pyr = total_tri_pyr = 0;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	
	grid = new Grid(Shape::Grid, { 0,0,0 }, "Grid", 500);
	game_object_selected = nullptr;
	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");
	for (int i = 0; i < total_game_objects; i++) {
		delete game_objects.at(i);
	}
	game_objects.clear();
	delete grid;

	return true;
}

void ModuleSceneIntro::CreateGameObject(Shape shape)
{

	GameObject* game_object = nullptr;
	std::string name;
	std::string num;;
	switch (shape)
	{
	case Shape::Empty:
		name = "GameObject";
		if (total_empty != 0) {
			num = std::to_string(total_empty);
			name = name + num;
		}
		game_object = new GameObject(Shape::Empty, { 0,0,0, }, name);
		total_empty++;
		break;
	case Shape::SolidPlane:
		name = "Plane";
		if (total_plane != 0) {
			num = std::to_string(total_plane);
			name = name + num;
		}
		game_object = new SolidPlane(Shape::SolidPlane, { 0,0,0 }, name, 1);
		total_plane++;
		break;
	case Shape::Cube:
		name = "Cube";
		if (total_cube != 0) {
			num = std::to_string(total_cube);
			name = name + num;
		}
		game_object = new Cube(Shape::Cube, { 0,0,0 }, name, 1);
		total_cube++;
		break;
	case Shape::RectangularPrism:
		name = "RectangularPrism";
		if (total_prism != 0) {
			num = std::to_string(total_prism);
			name = name + num;
		}
		game_object = new RectangularPrism(Shape::RectangularPrism, { 0,0,0 }, name, 1, 1, 2);
		total_prism++;
		break;
	case Shape::TriangularPyramid:
		name = "TriangularPyramid";
		if (total_tri_pyr != 0) {
			num = std::to_string(total_tri_pyr);
			name = name + num;
		}
		game_object = new TriangularPyramid(Shape::TriangularPyramid, { 0,0,0 }, name, 1);
		total_tri_pyr++;
		break;
	case Shape::SquarePyramid:
		name = "SquarePyramid";
		if (total_sqr_pyr != 0) {
			num = std::to_string(total_sqr_pyr);
			name = name + num;
		}
		game_object = new SquarePyramid(Shape::SquarePyramid, { 0,0,0 }, name, 1, 1);
		total_sqr_pyr++;
		break;
	case Shape::RectangularPyramid:
		name = "RectangularPyramid";
		if (total_rect_pyr != 0) {
			num = std::to_string(total_rect_pyr);
			name = name + num;
		}
		game_object = new RectangularPyramid(Shape::RectangularPyramid, { 0,0,0 }, name, 1, 3, 2);
		total_rect_pyr++;
		break;
	case Shape::SolidSphere:
		name = "Sphere";
		if (total_sphere != 0) {
			num = std::to_string(total_sphere);
			name = name + num;
		}
		game_object = new SolidSphere(Shape::SolidSphere, { 0,0,0 }, name, 1, 12, 24);
		total_sphere++;
		break;
	case Shape::Cilinder:
		name = "Cilinder";
		if (total_cilinder != 0) {
			num = std::to_string(total_cilinder);
			name = name + num;
		}
		game_object = new Cilinder(Shape::Cilinder, { 0,0,0 }, name, 1, 1, 3, 8);
		total_cilinder++;
		break;
	case Shape::SolidCone:
		name = "Cone";
		if (total_cone != 0) {
			num = std::to_string(total_cone);
			name = name + num;
		}
		game_object = new SolidCone(Shape::SolidCone, { 0,0,0 }, name, 1, 1, 16);
		total_cone++;
		break;
	}
	if (game_object != nullptr) {
		game_objects.push_back(game_object);
		total_game_objects++;
	}
}

void ModuleSceneIntro::LoadGameObject(float3 position, char* file, char* name)
{
	scene = aiImportFile(file, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			GameObject* m = new GameObject(Shape::Mesh, position, name);
			aiMesh* ai_mesh = scene->mMeshes[i];
			m->num_vertices = ai_mesh->mNumVertices * 3;
			m->vertices = new float[m->num_vertices];
			memcpy(m->vertices, ai_mesh->mVertices, sizeof(float) * m->num_vertices);
			LOG("Loading FBX correctly");
			LOG("New mesh with %d vertices", m->num_vertices);



			if (ai_mesh->HasFaces())
			{
				m->num_indices = ai_mesh->mNumFaces * 3;
				m->indices = new uint[m->num_indices]; // assume each face is a triangle
				for (uint j = 0; j < ai_mesh->mNumFaces; ++j)
				{
					if (ai_mesh->mFaces[j].mNumIndices != 3) {
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else {
						memcpy(&m->indices[j * 3], ai_mesh->mFaces[j].mIndices, 3 * sizeof(uint));
					}

				}
			}

			m->Initialization();
			game_objects.push_back(m);
			total_game_objects++;
			aiReleaseImport(scene);
		}
	}
	else {
		const char* error = aiGetErrorString();
		LOG("Error loading FBX: %s", error)
	}
}

// Update: draw background
update_status ModuleSceneIntro::Update(float dt)
{
	grid->Render(false);
	for (int i = 0; i < total_game_objects; i++) {
		game_objects.at(i)->Render();

	}
	
	if (App->central_editor->wireframe) {
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
	}
	
	return UPDATE_CONTINUE;
}

