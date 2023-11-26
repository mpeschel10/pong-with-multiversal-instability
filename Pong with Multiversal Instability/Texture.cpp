#ifndef TEXTURED_RECTANGLE_CPP
#define TEXTURED_RECTANGLE_CPP

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

class TexturedRectangle {
	float x = 0, y = 0;
	int widthpx, heightpx, numchannels;

	public:
		TexturedRectangle() {};
		void init(const char *path) {
		unsigned char *image_bytes = stbi_load(path, &widthpx, &heightpx, &numchannels, 0);
		if (image_bytes == NULL) {
			std::cout << "Warning: Could not load image " << path << ". Has it been moved or deleted?" << std::endl;
		}
		stbi_image_free(image_bytes);
	}
};

#endif
