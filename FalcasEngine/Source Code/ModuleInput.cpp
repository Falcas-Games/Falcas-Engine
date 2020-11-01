#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCentralEditor.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleSceneIntro.h"
#include "FileSystem.h"
#include "External Libraries/SDL/include/SDL.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
}

// Destructor
ModuleInput::~ModuleInput()
{
	delete[] keyboard;
}

// Called before render is available
bool ModuleInput::Init()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate(float dt)
{
	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN) {
				keyboard[i] = KEY_UP;
				char entry[50];
				sprintf_s(entry, 50, "Key ID: %i\n", i);
				App->central_editor->input_list.append(entry);
				App->central_editor->need_scroll = true;
			}
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	mouse_z = 0;

	for(int i = 0; i < 5; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if(mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if (mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN) {
				mouse_buttons[i] = KEY_UP;
				char entry[50];
				sprintf_s(entry, 50, "Key ID: %i\n", i);
				App->central_editor->input_list.append(entry);
				App->central_editor->need_scroll = true;
			}
			else
				mouse_buttons[i] = KEY_IDLE;
		}

	}

	mouse_x_motion = mouse_y_motion = 0;

	bool quit = false;
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		App->central_editor->ProcessEvents(e);
		switch(e.type)
		{
			case SDL_MOUSEWHEEL:
			mouse_z = e.wheel.y;
			break;

			case SDL_MOUSEMOTION:
			mouse_x = e.motion.x / SCREEN_SIZE;
			mouse_y = e.motion.y / SCREEN_SIZE;

			mouse_x_motion = e.motion.xrel / SCREEN_SIZE;
			mouse_y_motion = e.motion.yrel / SCREEN_SIZE;
			break;

			case SDL_QUIT:
			quit = true;
			break;

			case SDL_WINDOWEVENT:
			{
				if(e.window.event == SDL_WINDOWEVENT_RESIZED)
					App->renderer3D->OnResize(e.window.data1, e.window.data2);
			}
			break;

			case SDL_DROPFILE:
			{
				switch (GetTypeFile(e.drop.file)) {
				case FILE_TYPE::FBX:
					GameObject* game_object = App->scene_intro->CreateGameObject(GetFileName(e.drop.file), App->scene_intro->root);
					ComponentMesh* mesh;
					int num = 0;
					const aiScene* scene = mesh->GetNumberOfMeshes(e.drop.file, num);
					if (num > 1) {
						GameObject* game_object_iterator = nullptr;
						for (int i = 0; i < num; i++) {
							game_object_iterator = App->scene_intro->CreateGameObject(scene, i, GetFileName(e.drop.file), game_object);
						}
					}
					else {
						mesh = (ComponentMesh*)game_object->CreateComponent(Component_Type::Mesh);
						mesh->LoadMesh(scene, 0);
					}
					mesh->CleanScene(scene);
					break;
				}
			}
			break;
		}
	}

	if(quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP)
		return UPDATE_STOP;

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

KEY_STATE ModuleInput::GetKey(int id) const
{
	return keyboard[id];
}

std::string ModuleInput::GetFileName(char* file)
{
	std::string name = file;
	do {
		name.pop_back();
	} while (name.back() != '.');
	name.pop_back();

	int pos=name.find_last_of('\\');
	name = name.substr(pos + 1);
	return name.c_str();
}

FILE_TYPE ModuleInput::GetTypeFile(char* file)
{
	std::string name = file;
	

	uint size = name.find_last_of('.');
	name = name.substr(size + 1);

	
	if (name == "fbx") return FILE_TYPE::FBX;
	else if (name == "png") return FILE_TYPE::PNG;
	else if (name == "dds") return FILE_TYPE::DDS;
	else return FILE_TYPE::UNKNOWN;

}

