#pragma once
#include "../Core/SubSystem.h"
#include "File.h"
#include "../Container/Ptr.h"
#include <direct.h>
namespace Joestar {
	typedef void ReadFileCallback(File*);
	class FileSystem : public SubSystem {
		REGISTER_SUBSYSTEM(FileSystem);
		explicit FileSystem(EngineContext* ctx) : Super(ctx){}
		String GetResourceDir() {
			return "../Resources/";
		}
		String GetModelDir() {
			return "../Resources/Models/";
		}
		String GetTextureDir() {
			return "../Resources/Textures/";
		}
		String GetShaderDir() {
			return "../Resources/Shaders/";
		}

		String GetShaderDirAbsolute() {
			String path = GetShaderDir();
			char workDir[260];
			if (_getcwd(workDir, 260))
				path = workDir + ("/" + path);
			return path;
		}

		void ReadFileAsync(const String& filePath, ReadFileCallback callback);
		File* ReadFile(const String& filePath);
		bool WriteFile(const String& filePath, String& data);
		File* OpenFile(const String& filePath);

		File* GetShaderCodeFile(const String& file);
	};
}