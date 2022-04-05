#include "TextureCube.h"
#include "../Base/StringHash.h"
#include "../Container/Vector.h"

namespace Joestar {
	TextureCube::TextureCube(EngineContext* ctx) : Texture(ctx) {
		mType = ImageType::TYPE_2D;
	}
	//void TextureCube::TextureFromImage(String& path) {
	//	Vector<String> faces;
	//	faces.Push("right.jpg");
	//	faces.Push("left.jpg");
	//	faces.Push("top.jpg");
	//	faces.Push("bottom.jpg");
	//	faces.Push("front.jpg");
	//	faces.Push("back.jpg");
	//	imgs.Reserve(6);
	//	Image* img;
	//	for (unsigned int i = 0; i < faces.Size(); i++)
	//	{
	//		img = NEW_OBJECT(Image);
	//		img->Load((path + faces[i]).CString());
	//		imgs.Push(img);
	//		//目前认为六个面必须一样的尺寸
	//		mWidth = img->GetWidth();
	//		mHeight = img->GetHeight();
	//		mFormat = ImageFormat::R8G8B8A8_SRGB;
	//		if (!buffer) buffer = new U8[GetSize()];
	//		memcpy(buffer + i * img->GetSize(), img->GetData(), img->GetSize());
	//		DELETE_OBJECT(img);
	//	}
	//	//id = hashString(path.CString());
	//}

	TextureCube::~TextureCube() {
		//for (auto& img : imgs)
		//	delete img;
	}
}