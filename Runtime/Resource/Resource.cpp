#include "Resource.h"
#include "../IO/FileSystem.h"
namespace Joestar {
	Resource::Resource(EngineContext* ctx) : Super(ctx)
	{
		FileSystem* fs = GetSubsystem<FileSystem>();
		mDirectory = fs->GetResourceDir();
	}
	Resource::~Resource()
	{

	}

	bool Resource::LoadFile(const String& path)
	{
		FileSystem* fs = GetSubsystem<FileSystem>();
		mFile = fs->ReadFile(path);
		if (mFile)
		{
			return true;
		}
		return false;
	}
}