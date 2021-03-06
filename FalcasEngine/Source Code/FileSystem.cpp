#include "FileSystem.h"
#include <algorithm>
#include "Application.h"
#include "ModuleResources.h"
#include "Resource.h"
#include <WinBase.h>
#include "External Libraries/MathGeoLib/include/Algorithm/Random/LCG.h"
#include "External Libraries/PhysFS/include/physfs.h"

#pragma comment(lib, "Source Code/External Libraries/PhysFS/libx86/physfs.lib")

FileSystem::FileSystem(Application* app, bool start_enabled) : Module(app, start_enabled, "FileSystem")
{
	PHYSFS_init(nullptr);
	PHYSFS_mount(".", nullptr, 1);
}

FileSystem::~FileSystem()
{
	PHYSFS_deinit();
}

bool FileSystem::Start()
{
	if (PHYSFS_isInit()) {
		LOG("Asset Manager is Loaded");
	}
	else {
		LOG("Failed to load Asset Manager");
	}



	PHYSFS_mount("Assets.zip", nullptr, 1);


	return true;
}

void FileSystem::SetWritenDir()
{
	PHYSFS_setWriteDir(".");
}

std::string FileSystem::GetFileName(std::string file, bool has_filename_extension)
{
	if (has_filename_extension) {
		do {
			file.pop_back();
		} while (file.back() != '.');
		file.pop_back();
	}
	int pos = file.find_last_of('\\');
	if (pos == -1)
		pos = file.find_last_of('/');
	file = file.substr(pos + 1);
	return file;
}

std::string FileSystem::GetPathFile(std::string file)
{
	int pos = file.find_last_of('\\');
	if (pos == -1)
		pos = file.find_last_of('/');
	file = file.substr(0, pos+1);
	return file;
}


FILE_TYPE FileSystem::GetTypeFile(char* file)
{
	std::string name = file;
	return GetTypeFile(name);
}

FILE_TYPE FileSystem::GetTypeFile(std::string file)
{
	uint size = file.find_last_of('.');
	file = file.substr(size + 1);


	if (file == "fbx" || file == "FBX" || file == "obj") return FILE_TYPE::FBX;
	else if (file == "png" || file == "PNG") return FILE_TYPE::PNG;
	else if (file == "dds" || file == "DDS") return FILE_TYPE::DDS;
	else if (file == "scenefalcas") return FILE_TYPE::SCENE;
	else if (file == "TGA" || file == "tga")return FILE_TYPE::TGA;
	else if (file == "falcasmodel")return FILE_TYPE::MODEL;
	else if (file == "meta")return FILE_TYPE::META;
	else if (file == "falcasmesh")return FILE_TYPE::MESH;
	else return FILE_TYPE::UNKNOWN;
}

bool FileSystem::FileExists(std::string file)
{
	bool ret = false;
	if (PHYSFS_exists(file.c_str())) {
		ret = true;
	}
	return ret;
}

void FileSystem::DeleteAFile(std::string file)
{
	if (!FileExists(file.c_str()))
		return;
	PHYSFS_delete(file.c_str());
}

char* FileSystem::ReadPhysFile(std::string file, uint& size)
{
	char* buffer = nullptr;
	std::replace(file.begin(), file.end(), '\\', '/');
	PHYSFS_file* file_phys = PHYSFS_openRead(file.c_str());
	if (file_phys == nullptr) return"";
	size = (PHYSFS_sint32)PHYSFS_fileLength(file_phys);
	buffer = new char[size];
	PHYSFS_read(file_phys, buffer, 1, size);
	PHYSFS_close(file_phys);
	return buffer;
}

std::string FileSystem::CopyPhysFile(std::string file)
{
	std::string actual_location = PHYSFS_getBaseDir();
#ifdef _DEBUG
	int pos = actual_location.find_last_of("D");
	if (pos == -1)
		pos = actual_location.find_last_of('/');
	actual_location = actual_location.substr(0, pos);
	actual_location+="FalcasEngine\\Project Folder\\Assets\\";
#else
	actual_location += "Assets\\";
#endif
	actual_location += GetFileName(file, false);
	CopyFile(file.c_str(), actual_location.c_str(), TRUE);
	return "Assets/" + GetFileName(actual_location, false);
	
}

uint FileSystem::GetSizePhysFile(std::string file)
{
	PHYSFS_file* file_phys = PHYSFS_openRead(file.c_str());
	PHYSFS_sint32 size = (PHYSFS_sint32)PHYSFS_fileLength(file_phys);
	PHYSFS_close(file_phys);
	return size;
}

void FileSystem::DiscoverFiles(const char* directory, std::vector<std::string>& file_list, std::vector<std::string>& dir_list, std::string extension_to_ignore) const
{
	char** rc = PHYSFS_enumerateFiles(directory);
	char** i;

	std::string dir(directory);

	for (i = rc; *i != nullptr; i++)
	{
		if (!strcmp((dir + *i).c_str(), "Assets/Icons (read_only)"))
			continue;
		if (PHYSFS_isDirectory((dir + *i).c_str()))
			dir_list.push_back(*i);
		else {
			std::string extension = *i;
			if(extension.substr(extension.find_last_of('.') + 1)!=extension_to_ignore)
				file_list.push_back(*i);
		}
	}

	PHYSFS_freeList(rc);
}

void FileSystem::DiscoverFilesLibrary(const char* directory, std::vector<std::string>& file_list, std::vector<uint>& ids) const
{
	char** rc = PHYSFS_enumerateFiles(directory);
	char** i;

	std::string dir(directory);

	for (i = rc; *i != nullptr; i++)
	{	
		uint id = std::stoi(App->filesystem->GetFileName(*i, true));
		ids.push_back(id);
		std::string assets_name = App->resources->GetResource(id)->GetAssetsFile();
		file_list.push_back(App->filesystem->GetFileName(assets_name,true));
	}

	PHYSFS_freeList(rc);
}

std::vector<std::string> FileSystem::GetAllFiles(std::string file, std::vector<std::string> vector_file, std::string extension_file, bool ignore_extension, bool only_extension)
{
	char** rc = PHYSFS_enumerateFiles(file.c_str());
	char** i;


	for (i = rc; *i != nullptr; i++)
	{
		std::string file_with_extension = file + *i;
		uint size = file_with_extension.find_last_of('.');
		
		if (PHYSFS_isDirectory(file_with_extension.c_str()))
			vector_file = GetAllFiles(file_with_extension + '/', vector_file, extension_file, ignore_extension, only_extension);
		else if (file_with_extension.substr(size + 1) != extension_file && !only_extension)
			vector_file.push_back(file_with_extension);
		else if (file_with_extension.substr(size + 1) == extension_file && !ignore_extension)
			vector_file.push_back(file_with_extension);
	}

	PHYSFS_freeList(rc);
	return vector_file;

}

void FileSystem::SaveInternal(const char* file, const void* buffer, uint size)
{
	PHYSFS_file* file_phys = PHYSFS_openWrite(file);
	if (file_phys == nullptr) return;
	PHYSFS_write(file_phys, buffer, 1, size);
	PHYSFS_close(file_phys);
}

uint FileSystem::GetLastModificationTime(std::string file)
{
	return PHYSFS_getLastModTime(file.c_str());
}

void FileSystem::CreateFolders()
{
	CreateOneFolder("Library/Meshes/");
	CreateOneFolder("Library/Textures/");
	CreateOneFolder("Library/Scenes");
	CreateOneFolder("Library/Models");
	CreateOneFolder("/Config");
}

void FileSystem::CreateOneFolder(char* name)
{
	if (PHYSFS_isDirectory(name) == false) {
		PHYSFS_mkdir(name);
	}
}

JsonObj FileSystem::GenerateConfigContent()
{
	JsonObj obj;
	obj.AddString("icon", "");
	JsonArray arr = obj.AddArray("configModules");
	JsonObj _obj;
	_obj.AddString("name", "moduleWindow");
	arr.AddObject(_obj);
	_obj.AddString("appName", "Falcas Engine");
	arr.AddObject(_obj);
	_obj.AddInt("height", 994);
	arr.AddObject(_obj);
	_obj.AddInt("width", 1664);
	arr.AddObject(_obj);
	_obj.AddBool("resizeable", true);
	arr.AddObject(_obj);
	_obj.AddBool("fullscreen", false);
	arr.AddObject(_obj);
	_obj.AddBool("border", true);
	arr.AddObject(_obj);

	return obj;
}

uint FileSystem::GenerateUUID()
{
	return LCG().Int();
}



uint FileSystem::Load(const char* path, char** buffer) const
{
	uint ret=0;
	std::string str = path;
	std::replace(str.begin(), str.end(), '\\', '/');
	int size = -1;
	size = str.find("Assets/");
	if(size>0)
		str=str.substr(size).c_str();
	const char* path_relative = str.c_str();
	PHYSFS_file* file = PHYSFS_openRead(path_relative);
	if (file!=nullptr&&!PHYSFS_eof(file))
	{
		uint lenght = PHYSFS_fileLength(file);
		*buffer = new char[lenght];
		uint bytes = PHYSFS_readBytes(file, *buffer, lenght);

		if (bytes != lenght)
		{
			LOG("%s", path, "ERROR: %s", PHYSFS_getLastError());
			if (buffer != NULL)  
			{               
				delete[] buffer;
				buffer = NULL;
			}
		}
		else
			ret = bytes;
	}
	else
		LOG("%s", path, "ERROR: %s", PHYSFS_getLastError());

	PHYSFS_close(file);

	return ret;
}

uint FileSystem::LoadPath(char* file, char** buffer) {
	PHYSFS_file* fs_file = PHYSFS_openRead(file);
	if (fs_file == nullptr) {
		*buffer = nullptr;
		return 0;
	}
	PHYSFS_sint32 size = (PHYSFS_sint32)PHYSFS_fileLength(fs_file);
	*buffer = new char[size + 1];
	PHYSFS_read(fs_file, *buffer, 1, size);
	(*buffer)[size] = '\0';
	PHYSFS_close(fs_file);
	return size;
}

