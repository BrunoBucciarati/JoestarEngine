#pragma once
#include "../Base/SubSystem.h"
#include "File.h"
#include <direct.h>
namespace Joestar {
	typedef void ReadFileCallback(File*);
	class FileSystem : public SubSystem {
		REGISTER_SUBSYSTEM(FileSystem)
		explicit FileSystem(EngineContext* ctx) : Super(ctx){}
		const char* GetResourceDir() {
			return "../Resources/";
		}
		const char* GetModelDir() {
			return "../Resources/Models/";
		}
		const char* GetTextureDir() {
			return "../Resources/Textures/";
		}
		const char* GetShaderDir() {
			return "../Resources/Shaders/";
		}

		std::string GetShaderDirAbsolute() {
			std::string path = GetShaderDir();
			char workDir[260];
			if (_getcwd(workDir, 260))
				path = workDir + ("/" + path);
			return path;
		}

		void ReadFileAsync(const char* filePath, ReadFileCallback callback);
		File* ReadFile(const char* filePath);
		bool WriteFile(const char* filePath, std::string& data);
		File* OpenFile(const char* filePath);

		File* GetShaderCodeFile(const char* file);
	};
}