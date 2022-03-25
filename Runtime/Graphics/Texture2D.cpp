#include "Texture2D.h"
#include "../Base/StringHash.h"

namespace Joestar {
	void Texture2D::TextureFromImage(String& path) {
		img = NEW_OBJECT(Image);
		img->Load(path.CString());// "Models/viking_room/viking_room.png");
		width = img->GetWidth();
		height = img->GetHeight();
		id = hashString(path.CString());
		fmt = TEXTURE_FORMAT_RGBA8;
	}

	Texture2D::~Texture2D() {
		DELETE_OBJECT(img);
	}

	U8* Texture2D::GetData() {
		return (U8*)img->GetData();
	}
}