#include "Image.h"
#include "../IO/FileSystem.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Joestar {
	void Image::Load(const char* path) {
		std::string resDir = GetSubsystem<FileSystem>()->GetResourceDir();
		data = (char*)stbi_load((resDir + path).c_str(), &width, &height, &channels, STBI_rgb_alpha);
	}
	Image::~Image() {
		if (data)
			stbi_image_free(data);
	}
}