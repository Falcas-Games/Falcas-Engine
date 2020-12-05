#include "ModuleResources.h"
#include "Application.h"
#include "FileSystem.h"
#include "Importer.h"

ModuleResources::ModuleResources(Application* app, bool start_enabled) : Module(app, start_enabled, "moduleResources")
{
}

bool ModuleResources::Start()
{
	std::vector<std::string> vector_assets_files=App->filesystem->GetAllFiles("Assets/", vector_assets_files, "meta");
	for (int i = 0; i < vector_assets_files.size(); i++) {
		uint id = App->filesystem->GenerateUUID();
		resources.insert(std::pair<uint, std::string>(id, vector_assets_files[i]));
		int last_modificated = 0;
		if (App->filesystem->FileExists(vector_assets_files[i] + ".meta")) {
			char* buffer;
			App->filesystem->Load((vector_assets_files[i] + ".meta").c_str(), &buffer);
			JsonObj mod_time(buffer);
			if (mod_time.GetInt("Date") == App->filesystem->GetLastModificationTime(vector_assets_files[i])) {
				UpdateMetaFile(vector_assets_files[i], id, buffer);
				continue;
			}
			delete[] buffer;
		}
			
		CreateNewMetaFile(vector_assets_files[i], id);
	}
	return true;
}

void ModuleResources::UpdateMetaFile(std::string meta_file, uint id, char* buffer)
{
	JsonObj obj_meta_existing(buffer);
	JsonObj obj;
	obj.AddInt("ID", id);
	obj.AddString("Name", obj_meta_existing.GetString("Name"));
	obj.AddInt("Type", obj_meta_existing.GetInt("Type"));
	obj.AddInt("Date", obj_meta_existing.GetInt("Date"));
	delete[] buffer;
	obj.Save(&buffer);
	App->filesystem->SaveInternal((meta_file + ".meta").c_str(), buffer, strlen(buffer));
	delete[] buffer;

}

void ModuleResources::CreateNewMetaFile(std::string file, uint id)
{
	JsonObj obj;
	obj.AddInt("ID", id);
	obj.AddString("Name", App->filesystem->GetFileName(file,true).c_str());
	char* file_char = new char[file.length() + 1 ];
	strcpy(file_char, file.c_str());
	switch (App->filesystem->GetTypeFile(file_char)) {
	case FILE_TYPE::FBX:
		obj.AddInt("Type", 1);
		ImportFBX(file);
		//MeshImporter::Import()
		break;
	case FILE_TYPE::DDS:
	case FILE_TYPE::PNG:
	case FILE_TYPE::TGA:
		obj.AddInt("Type", 2);
		TextureImporter::Import(file);
		break;
	deafult:
		obj.AddInt("Type", 3);
		break;
	}
	delete[] file_char;
	obj.AddInt("Date", App->filesystem->GetLastModificationTime(file));
	char* buffer;
	obj.Save(&buffer);
	App->filesystem->SaveInternal((file+".meta").c_str(), buffer, strlen(buffer));
	delete[] buffer;
}