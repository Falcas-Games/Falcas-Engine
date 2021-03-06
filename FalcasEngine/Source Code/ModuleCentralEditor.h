#pragma once
#ifndef __ModuleCentralEditor_H__
#define __ModuleCentralEditor_H__

#include "Module.h"
#include <list>
#include <vector>
#include <string>
#include "ResourceMesh.h"

enum class Shape {
    Cube,
    Rectangular_Prism,
    Triangular_Pyramid,
    Square_Pyramid,
    Rectangular_Pyramid,
	Cilinder,
	Cone,
	Sphere,
	Plane
};

class Application;
class GameObject;
typedef int ImGuiTreeNodeFlags; 
union SDL_Event;
class ResourceMaterial;

class ModuleCentralEditor : public Module
{
public:

	ModuleCentralEditor(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleCentralEditor();

	bool Init();
	bool Start();
	bool CleanUp();

	void OnTriggered(ComponentUI* component_ui);

	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);

	void Draw();
	void DrawImGuizmo();

	bool LoadFile();
	bool SaveScene(const char* file);

	void LoadScene(const char* file);

	bool ProcessEvents(SDL_Event event);

	void FilesRecursiveTree(const char* path, bool is_in_dock, bool resources_window, bool is_directory, static ImGuiTreeNodeFlags base_flags, std::string& assets_file_clicked);
	void HierarchyRecursiveTree(GameObject* game_object, static ImGuiTreeNodeFlags base_flags, int &node_clicked);
	void SearchParent(GameObject* game_object, uint uuid);

	void CreateScene();

	void SaveAllGameObjectsTree(GameObject* game_object, JsonArray arr);
	void DeleteAllGameObjects(GameObject* game_object);

	void SelectObject(GameObject* game_obj);

	void CreateDock();

	void GameControl();

	void ViewCameras(bool active, GameObject* game_object);

	void ChangingScreen();

	void CreateCurtain();

public:

	bool show_demo_window = true;
	bool show_another_window = false;
	bool show_example = false;
	bool show_about = false;
	bool show_configuration = false;
	bool show_console = false;
	bool show_openglOptions = false;
	bool show_hierarchy = true;
	bool show_inspector = true;
	bool show_assets_window = true;
	bool show_assets_explorer = true;
	bool show_references_window = true;
	bool sure_want_close = false;
	bool creating_scene = false;
	bool want_to_load_fromButton = false;
	bool changingscreen = false;
	bool show_cameras = false;
	bool cullface = false;
	bool colorMaterial = false;
	bool ambient = false;
	bool stencil = false;
	bool wireframe = false;
	bool normals_v = false;
	bool normals_f = false;
	bool aabbs = false;
	bool depth = true;
	bool lighting = true;
	bool texture = true;
	bool frustums = false;
	bool loading_file = false;
	bool grid = true;
	bool raycast = false;

	float progress = 0;
	float progress2 = 0;
	float progress3 = 0;
	float progress4 = 0;

	std::list<float> fr_arr;
	std::list<float> ms_arr;

	std::vector<char*> console_logs;

	std::vector <char*> input_list;
	bool need_scroll = false;

	bool wantToExit = false;

private:
	bool selected_through_screen = false;
	int id_popup_shown = -1;
	bool is_popup_shown = false;
	bool delete_game_object = false;
	std::vector<int> ids_of_parents_selected;
	char selected_file[100] = "";
	std::string selected_button_file = "";

	bool bool_parentFound;
	GameObject* parentFound = nullptr;
	ResourceMaterial* icon_folder = nullptr;
	ResourceMaterial* icon_model = nullptr;
	ResourceMaterial* icon_mesh = nullptr;
	ResourceMaterial* icon_material = nullptr;
	ResourceMaterial* icon_file = nullptr;
	std::string assets_explorer_path = "Assets/";
	int assets_explorer_selected = -1;

	GameObject* curtain = nullptr;
	Timer time_curtain;
	bool fadein = false;
	bool fadeout = false;
};

#endif // __ModuleCentralEditor_H__