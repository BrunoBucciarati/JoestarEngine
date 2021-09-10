#include "TextureCube.h"
#include "../Base/StringHash.h"

namespace Joestar {
	void TextureCube::TextureFromImage(std::string& path) {
		std::vector<std::string> faces {
			"right.jpg",
			"left.jpg",
			"top.jpg",
			"bottom.jpg",
			"front.jpg",
			"back.jpg"
		};
		imgs.reserve(6);
		Image* img;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			img = NEW_OBJECT(Image);
			img->Load((path + faces[i]).c_str());
			imgs.push_back(img);
			//目前认为六个面必须一样的尺寸
			width = img->GetWidth();
			height = img->GetHeight();
			fmt = TEXTURE_FORMAT_RGBA8;
			if (!buffer) buffer = new U8[GetSize()];
			memcpy(buffer + i * width * height * TEXTURE_FORMAT_SIZE[fmt], img->GetData(), width * height * TEXTURE_FORMAT_SIZE[fmt]);
			DELETE_OBJECT(img);
		}
		id = hashString(path.c_str());
	}

	TextureCube::~TextureCube() {
		//for (auto& img : imgs)
		//	delete img;
	}

	U8* TextureCube::GetData() {
		return buffer;
	}
}