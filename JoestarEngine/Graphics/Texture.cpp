#include "Texture.h"
#include "../Base/StringHash.h"

namespace Joestar {
	void Texture::TextureFromImage(std::string& path) {
		img = new Image(path.c_str());// "Models/viking_room/viking_room.png");
		width = img->GetWidth();
		height = img->GetHeight();
		id = hashString(path.c_str());
		fmt = TEXTURE_FORMAT_RGBA8;
	}
}