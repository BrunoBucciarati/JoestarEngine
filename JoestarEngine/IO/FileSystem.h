#pragma once
#include "../Base/SubSystem.h"
#include "File.h"
#include <direct.h>
namespace Joestar {
	typedef void ReadFileCallback(File*);
	class FileSystem : public SubSystem {
		REGISTER_OBJECT(FileSystem, SubSystem)
		static const char* GetResourceDir() {
			return "../Resources/";
		}
		static const char* GetModelDir() {
			return "../Resources/Models/";
		}
		static const char* GetTextureDir() {
			return "../Resources/Textures/";
		}
		static const char* GetShaderDir() {
			return "../Resources/Shaders/";
		}

		void ReadFileAsync(const char* filePath, ReadFileCallback callback);
		File* ReadFile(const char* filePath);
		File* OpenFile(const char* filePath);
	};
}