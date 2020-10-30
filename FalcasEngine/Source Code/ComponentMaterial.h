#ifndef _COMPONENT_MATERIAL_
#define _COMPONENT_MATERIAL_

#include "External Libraries/Devil/Include/ilut.h"
#include "Component.h"

#pragma comment (lib, "DevIl/lib/DevIl.lib")
#pragma comment (lib, "DevIl/lib/ILUT.lib")
#pragma comment (lib, "DevIl/lib/ILU.lib")

class GameObject;

class ComponentMaterial : public Component {
public:
	ComponentMaterial(GameObject* parent);
	~ComponentMaterial();
	ComponentMaterial();
public:
	void LoadTexture(const char* file);
	void LoadDefault();
	
public:
	ILuint image_name;
	ILuint checkers;
	uint texture_id;
	uint defaultTex;

	bool wantTex = false;

	GameObject* parent;
};
#endif // !_COMPONENT_MATERIAL_