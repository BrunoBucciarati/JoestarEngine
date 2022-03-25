#include "TextureCube.h"
#include "../Base/StringHash.h"
#include "../Container/Vector.h"

namespace Joestar {
	void TextureCube::TextureFromImage(String& path) {
		Vector<String> faces;
		faces.Push("right.jpg");
		faces.Push("left.jpg");
		faces.Push("top.jpg");
		faces.Push("bottom.jpg");
		faces.Push("front.jpg");
		faces.Push("back.jpg");
		imgs.Reserve(6);
		Image* img;
		for (unsigned int i = 0; i < faces.Size(); i++)
		{
			img = NEW_OBJECT(Image);
			img->Load((path + faces[i]).CString());
			imgs.Push(img);
			//目前认为六个面必须一样的尺寸
			width = img->GetWidth();
			height = img->GetHeight();
			fmt = TEXTURE_FORMAT_RGBA8;
			if (!buffer) buffer = new U8[GetSize()];
			memcpy(buffer + i * width * height * TEXTURE_FORMAT_SIZE[fmt], img->GetData(), width * height * TEXTURE_FORMAT_SIZE[fmt]);
			DELETE_OBJECT(img);
		}
		id = hashString(path.CString());
	}

	TextureCube::~TextureCube() {
		//for (auto& img : imgs)
		//	delete img;
	}

	U8* TextureCube::GetData() {
		return buffer;
	}
}