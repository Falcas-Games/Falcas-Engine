#include "Asap.h"
#include "External Libraries/Assimp/Assimp/include/cimport.h"
#include "External Libraries/Assimp/Assimp/include/scene.h"
#include "External Libraries/Assimp/Assimp/include/postprocess.h"


//#pragma comment (lib, "Source Code/External Libraries/Assimp/libx86/assimp.lib")

Asap::Asap(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

bool Asap::Init()
{
	return true;
}

bool Asap::CleanUp()
{
	return true;
}

update_status Asap::PreUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

update_status Asap::PostUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}
