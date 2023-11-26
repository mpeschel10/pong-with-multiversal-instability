#ifndef TEXTURED_RECTANGLE_CPP
#define TEXTURED_RECTANGLE_CPP

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

class TexturedRectangle {
	float x = 0, y = 0;
	float w = 50, h = 50;
	
	const char * texturePath;
	int textureWidth, textureHeight, textureChannelsCount;
	GLuint textureID;

	public:
	TexturedRectangle(const char *path) { texturePath = path; }

	void init() {
		unsigned char *image_bytes = stbi_load(texturePath, &textureWidth, &textureHeight, &textureChannelsCount, 0);
		if (image_bytes == NULL) {
			std::cout << "Warning: Could not load image " << texturePath << ". Has it been moved or deleted?" << std::endl;
		}

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		GLenum channelOrder = textureChannelsCount == 3 ? GL_RGB : GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, channelOrder, textureWidth, textureHeight, 0, channelOrder, GL_UNSIGNED_BYTE, image_bytes);

		stbi_image_free(image_bytes);
	}

	void xywh(float x, float y, float w, float h) {
		this->x = x; this->y = y;
		this->w = w; this->h = h;
	}

	void display() {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 1);
		glVertex2f(x, y);
		glTexCoord2f(1, 1);
		glVertex2f(x + w, y);
		glTexCoord2f(1, 0);
		glVertex2f(x + w, y + h);
		glTexCoord2f(0, 0);
		glVertex2f(x, y + h);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
};

#endif
