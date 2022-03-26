#pragma once
#include "../Core/Minimal.h"
#include "../IO/File.h"

namespace Joestar
{
	class Resource : public Object
	{
		REGISTER_OBJECT_ROOT(Resource);
	public:
		explicit Resource(EngineContext* ctx);
		bool LoadFile(const String&);
		File* GetFile()
		{
			return mFile;
		}
	protected:
		SharedPtr<File> mFile;
		String mDirectory;
	};
}