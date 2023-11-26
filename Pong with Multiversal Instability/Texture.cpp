#ifndef TEXTURED_RECTANGLE_CPP
#define TEXTURED_RECTANGLE_CPP

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

class TexturedRectangle {
	float x = 0, y = 0;
	int widthpx, heightpx, numchannels;
	GLuint textureID;

	public:
		TexturedRectangle() {};

		void init(const char *path) {
			unsigned char *image_bytes = stbi_load(path, &widthpx, &heightpx, &numchannels, 0);
			std::cout << "Nu mchannels: " << numchannels << std::endl;
			if (image_bytes == NULL) {
				std::cout << "Warning: Could not load image " << path << ". Has it been moved or deleted?" << std::endl;
			}

			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_2D, textureID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthpx, heightpx, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_bytes);

			stbi_image_free(image_bytes);
		}

		void display() {
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex2f(300, 300);
			glTexCoord2f(1, 0);
			glVertex2f(400, 300);
			glTexCoord2f(1, 1);
			glVertex2f(400, 400);
			glTexCoord2f(0, 1);
			glVertex2f(300, 400);
			glEnd();
			glDisable(GL_TEXTURE_2D);

			// std::cout << "Tried to display a something or other." << std::endl;
		}
};

#endif
