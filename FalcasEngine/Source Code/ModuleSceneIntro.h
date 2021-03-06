#pragma once
#include "Module.h"
#include "External Libraries/MathGeoLib/include/Math/float3.h"
#include <string>
#include <map>
#include "External Libraries/Glew/include/glew.h"


class GameObject;
class ComponentCamera;
class Grid;
class ResourceModel;
class ResourceMesh;
class ResourceMaterial;
class ComponentTransform2D;


enum class Guizmos_Input_Letters {
	W,
	E,
	R,
};

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();



	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Draw(GLuint tex);
	bool GetDimensionsWindow(float& width, float& height);
	
	GameObject* CreateGameObject(std::string name = "", GameObject* parent = nullptr, bool is_ui = false);


	GameObject* SearchGameObject(int id, GameObject* game_obj);
	GameObject* SearchGameObject(uint uuid, GameObject* game_object);
	bool IsGameObjectNameRepeated(std::string name, GameObject* game_obj);
	std::string CheckNameGameObject(std::string name, bool numbered = false);

	void LoadModel(ResourceModel* model);

	void GetSceneDimensions(float& x, float& y, float& width, float& height);

	void SelectGameObjectWithRay(LineSegment ray);
	std::map<float, GameObject*> CheckIfGameObjectIsSelectable(GameObject* game_obj, std::map<float, GameObject*> map, LineSegment ray);

	GameObject* game_object_selected;
	ComponentCamera* camera;
	GameObject* root;
	Grid* grid;
	int GetID();
	int id_gameobject;
	std::vector<int> game_objects_to_delete;
	bool mouse_on_scene = true;

	void ChangingScreen();

	std::vector<ResourceMesh*> resources_mesh_to_delete;
	std::vector<ResourceMaterial*> resources_material_to_delete;
	std::vector<ResourceModel*> resources_model_to_delete;

	Guizmos_Input_Letters input_letter = Guizmos_Input_Letters::W;

private:
	float scene_width;
	float scene_height;
	float x;
	float y;

};
