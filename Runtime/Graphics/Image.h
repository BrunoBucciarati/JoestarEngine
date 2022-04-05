#pragma once
#include "../Core/Minimal.h"
#include "../Resource/Resource.h"
namespace Joestar {
	class Image : public Resource {
		REGISTER_OBJECT(Image, Resource)
	public:
		explicit Image(EngineContext* ctx);
		void Load(String path);
		U32 GetWidth() { return mWidth; }
		U32 GetHeight() { return mHeight; }
		U32 GetSize() { return mWidth * mHeight * mChannels; }
		U8* GetData() { return mData; }
	private:
		U32 mWidth, mHeight, mChannels;
		U8* mData;
	};
}