#pragma once
#include "Component.h"

class GameObject;
class ResourceMesh;
class ComponentMesh : public Component {
public:
	ComponentMesh(GameObject* owner);
	~ComponentMesh();
	void Update();
	
	void Initialization();
	void Render();
	bool SaveComponent(JsonObj& obj);

	void Inspector();


	bool show_normals_v = false;
	bool show_normals_f = false;
	float length_normals = 1;
	int  meshNumber = 0;

	ResourceMesh* resource_mesh = nullptr;
	

};