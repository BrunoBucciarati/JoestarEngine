#include "Image.h"
#include "../IO/FileSystem.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Joestar {
	void Image::Load(const char* path)
	{
		String resDir = GetSubsystem<FileSystem>()->GetResourceDir();
		data = (char*)stbi_load((resDir + path).CString(), &width, &height, &channels, STBI_rgb_alpha);
	}
	Image::~Image()
	{
		if (data)
			stbi_image_free(data);
	}
}