/*
 * Jimin Lim
 * CS200
 * Fall 2023
 */

#include"GLTexture.h"
#include"glCheck.h"
#include<stb_image.h>
#include<string>

GLTexture::~GLTexture() noexcept
{
	delete_texture();
}

void GLTexture::delete_texture() noexcept
{
	glDeleteTextures(1, &texture_handle);
	texture_handle = 0;
	width = 0;
	height = 0;
}

GLTexture::GLTexture(GLTexture&& other) noexcept
{
	texture_handle = other.texture_handle;
	width = other.width;
	height = other.height;
	other.texture_handle = 0;
	other.width = 0;
	other.height = 0;
}

GLTexture& GLTexture::operator=(GLTexture&& other) noexcept
{
	if (this != &other)
	{
		// 현재 객체의 멤버 변수를 이동
		texture_handle = other.texture_handle;
		width = other.width;
		height = other.height;

		// 다른 객체의 멤버 변수 초기화
		other.texture_handle = 0;
		other.width = 0;
		other.height = 0;
	}
	return *this;
}

bool GLTexture::LoadFromFileImage(const std::filesystem::path& image_filepath, bool flip_vertical) noexcept
{
	// Set whether to flip the image vertically
	stbi_set_flip_vertically_on_load(flip_vertical);

	// Read image files to obtain RGBA data, width, and height
	int channel;
	bool success = false;
	std::string filename = image_filepath.generic_string();
	unsigned char* ptr_texels = stbi_load(filename.c_str(), &width, &height, &channel, 0);

	//When image data is successfully obtained, the LoadFromMemory function is called to create a texture
	if (ptr_texels != nullptr)
	{
		success = LoadFromMemory(width, height, reinterpret_cast<RGBA*>(ptr_texels));
		// 이미지 데이터의 메모리를 해제
		stbi_image_free(ptr_texels);
	}

	// 로드가 성공적으로 완료되었는지 여부 반환
	return success;
}



bool GLTexture::LoadFromMemory(int image_width, int image_height, const RGBA* colors) noexcept
{
	delete_texture();
	width = image_width;
	height = image_height;
	
	glCreateTextures(GL_TEXTURE_2D, 1, &texture_handle);

	constexpr GLsizei ONE_TEXTURE_LEVEL = 1;
	glTextureStorage2D(texture_handle, ONE_TEXTURE_LEVEL, GL_RGBA8, width, height);

	constexpr GLint FIRST_LEVEL = 0;
	constexpr GLsizei OFFSET_X = 0, OFFSET_Y = 0;
	glTextureSubImage2D(texture_handle, FIRST_LEVEL, OFFSET_X, OFFSET_Y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, colors);

	return true;
}

void GLTexture::SetFiltering(Filtering how_to_filter) noexcept
{
	if (texture_handle == 0)
		return;

	filtering = how_to_filter;

	if (filtering == Filtering::NearestPixel)
	{
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	}
	else if (filtering == Filtering::Linear)
	{
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	}
}


void GLTexture::SetWrapping(Wrapping how_to_wrap, Coordinate coord) noexcept
{
	if (texture_handle == 0)
		return;

	switch (coord)
	{
	case Coordinate::Both:
		glCheck(glTextureParameteri(texture_handle, GL_TEXTURE_WRAP_S, how_to_wrap));
		glCheck(glTextureParameteri(texture_handle, GL_TEXTURE_WRAP_S, how_to_wrap));
		break;
	case Coordinate::S:
		glCheck(glTextureParameteri(texture_handle, GL_TEXTURE_WRAP_S, how_to_wrap));
		break;
	case Coordinate::T:
		glCheck(glTextureParameteri(texture_handle, GL_TEXTURE_WRAP_T, how_to_wrap));
		break;
	}
}

void GLTexture::UseForSlot(unsigned int texture_unit) const noexcept
{
	glCheck(glBindTextureUnit(texture_unit, texture_handle););
}