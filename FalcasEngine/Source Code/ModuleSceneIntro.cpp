#pragma once
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleCentralEditor.h"
#include "ModuleCamera3D.h"

#include "Console.h"
#include "GameObject.h"
#include "External Libraries/MathGeoLib/include/Math/Quat.h"
#include "ComponentMesh.h"
#include "External Libraries/Devil/Include/il.h"
#include "ComponentMaterial.h"
#include <gl/GL.h>
#include "Shape.h"
#include "FileSystem.h"

#pragma comment( lib, "Source Code/External Libraries/Devil/lib/ILU.lib" )
#pragma comment( lib, "Source Code/External Libraries/Devil/lib/DevIL.lib" )


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

	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
	ilEnable(IL_ORIGIN_SET);


	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));
	id_gameobject = -1;
	root = new GameObject(id_gameobject, "Grid", nullptr, { 0,0,0 }, Quat::identity, { 500,1,500 });
	id_gameobject++;
	ComponentMesh* mesh = (ComponentMesh*)root->CreateComponent(Component_Type::Mesh);
	mesh->grid = true;
	CreateGrid(mesh->num_vertices, mesh->num_indices, mesh->indices, mesh->vertices, { 0,0,0 }, { 500,1,500 });
	mesh->Initialization();
	game_object_selected = nullptr;
	int num = 0;
	GameObject* house=CreateGameObject("BakerHouse", root);
	const aiScene* scene = mesh->GetNumberOfMeshes("Assets/BakerHouse.fbx", num);
	if (num > 1) {
		GameObject* game_object = nullptr;
		for (int i = 0; i < num; i++) {
			game_object =CreateGameObject(scene, i, "Assets/BakerHouse.fbx", "BakerHouse", house);
			game_object->CreateComponent(Component_Type::Material, "Assets/Baker_house.png");
		}
	}
	else {
		mesh = (ComponentMesh*)house->CreateComponent(Component_Type::Mesh);
		mesh->LoadMesh(scene, 0);
		house->CreateComponent(Component_Type::Material, "Assets/Baker_house.png");
	}
	mesh->CleanScene(scene);


	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");
	delete root;
	game_object_selected = nullptr;
	ilDisable(IL_ORIGIN_SET);

	return true;
}

int ModuleSceneIntro::GetID()
{
	return id_gameobject;
}


// Update: draw background
update_status ModuleSceneIntro::Update(float dt)
{
	
	root->Update();

	
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

GameObject* ModuleSceneIntro::CreateGameObject(std::string name, GameObject* parent)
{
	GameObject* game_object = nullptr;
	name = CheckNameGameObject(name);
	game_object = new GameObject(App->scene_intro->id_gameobject, name, parent);
	if (game_object != nullptr) {
		App->scene_intro->id_gameobject++;
		if (parent != nullptr) {
			parent->children.push_back(game_object);
		}

	}
	return game_object;
}

GameObject* ModuleSceneIntro::CreateGameObject(float3 position, Quat rotation, float3 size, std::string name, GameObject* parent)
{
	GameObject* game_object = nullptr;
	name = CheckNameGameObject(name);
	game_object = new GameObject(App->scene_intro->id_gameobject, name, parent, position, rotation, size);
	if (game_object != nullptr) {
		App->scene_intro->id_gameobject++;
		if (parent != nullptr) {
			parent->children.push_back(game_object);
		}
	}
	return game_object;
}

GameObject* ModuleSceneIntro::CreateGameObject(const aiScene* scene, int num_of_mesh, std::string file_name, std::string name, GameObject* parent)
{
	GameObject* game_object = nullptr;
	name = CheckNameGameObject(name);
	game_object = new GameObject(App->scene_intro->id_gameobject, name, parent);
	if (game_object != nullptr) {
		App->scene_intro->id_gameobject++;
		if (parent != nullptr) {
			parent->children.push_back(game_object);
		}

	}
	ComponentMesh* mesh= (ComponentMesh*)game_object->CreateComponent(Component_Type::Mesh);
	mesh->LoadMesh(scene, num_of_mesh);
	mesh->SetFileName(file_name);
	return game_object;
}

GameObject* ModuleSceneIntro::CreateGameObject(const aiScene* scene, int num_of_mesh, float3 position, Quat rotation, float3 size, std::string file_name, std::string name, GameObject* parent)
{
	GameObject* game_object = nullptr;
	name = CheckNameGameObject(name);
	game_object = new GameObject(App->scene_intro->id_gameobject, name, parent, position, rotation, size);
	if (game_object != nullptr) {
		App->scene_intro->id_gameobject++;
		if (parent != nullptr) {
			parent->children.push_back(game_object);
		}
	}
	ComponentMesh* mesh = (ComponentMesh*)game_object->CreateComponent(Component_Type::Mesh);
	mesh->LoadMesh(scene, num_of_mesh);
	mesh->SetFileName(file_name);
	return game_object;
}


GameObject* ModuleSceneIntro::SearchGameObject(int id, GameObject* game_obj)
{
	GameObject* game_object = nullptr;
	if (game_obj->id == id) {
		return game_obj;
	}
	for (int i = 0; i < game_obj->children.size() && game_object == nullptr; i++) {
		game_object = SearchGameObject(id, game_obj->children.at(i));
	}
	return game_object;
}

std::string ModuleSceneIntro::CheckNameGameObject(std::string name, bool numbered, int digits)
{
	bool stop = false;
	GameObject* game_object = nullptr;
	for (int i = 0; i <= id_gameobject && stop == false; i++) {
		game_object = SearchGameObject(i, App->scene_intro->root);
		if (game_object != nullptr && game_object->name == name) {
			stop = true;
		}
	}
	if (stop == true) {
		if (numbered) {
			name.pop_back();
			int number = name.back() - '0';
			if (number >= 9)
				name = NameGameObjectWhenMoreThan2Digits(name, digits);
			else {
				number++;
				name.pop_back();
				name.push_back(number + '0');
				name.push_back(')');
			}
		}
		else {
			name.push_back(' ');
			name.push_back('(');
			name.push_back('1');
			name.push_back(')');
		}
		name = CheckNameGameObject(name, true, digits);
	}
	return name;
}

std::string ModuleSceneIntro::NameGameObjectWhenMoreThan2Digits(std::string name, int& digits)
{
	int number = 0;
	for (int i = 0; i < digits; i++) {
		number += (name.back() - '0') * (pow(10, i));
		name.pop_back();
	}
	number++;
	if (pow(10, digits) <= number) {
		digits++;
	}
	for (int i = digits - 1; i > 0; i--) {
		if (number > (pow(10, i)) - 1) {
			int n = number / pow(10, i);
			name.push_back(n + '0');
			number -= n * (pow(10, i));
		}
		else {
			name.push_back(0 + '0');
		}
	}

	name.push_back(number + '0');
	name.push_back(')');
	return name;
}