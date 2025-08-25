/*
 * Rudy Castan, Jimin Lim
 * CS200
 * Fall 2022
 */

#include <array>
#include <filesystem>

#include "GL/glew.h"
#include "GLHandle.h"

#pragma once

class [[nodiscard]] GLTexture
{
public:
    GLTexture() = default;

    /*This function is the Destructor of the GLTexture class.
    Call the delete_texture function that deletes the texture.
    Initializes the texture handle and size.*/
    ~GLTexture() noexcept;


    GLTexture(const GLTexture& other) = delete;
    /*This function is the Move generator, which moves data from other GLTexture objects.
    Move the texture handle, width, and height of another object to the current object and initialize the other object.*/
    GLTexture(GLTexture&& other) noexcept;

    GLTexture& operator=(const GLTexture& other) = delete;
    /*This function is a moving substitution operator that moves data from other GLTexture objects.
    Move the texture handle, width, and height of another object to the current object and initialize the other object.*/
    GLTexture& operator                          =(GLTexture&& other) noexcept;

    /*Load the image file to get the RGBA data, width, and height information.
    Sets whether to flip the image vertically.If successfully obtain image data, you use it to create a texture.
    Clears the memory of the image data and returns whether the load was successful.*/
    [[nodiscard]] bool LoadFromFileImage(const std::filesystem::path& image_filepath, bool flip_vertical = true) noexcept;

    using RGBA = unsigned int;
    static_assert(sizeof(RGBA) == 4, "RGBA should be the same as 4 bytes, so we can easily treat it as an RGBA int.");

    /*Load image data from memory into the GLTexture object.
    Create a texture and perform the required initialization.
    Assign loaded image data to textures to create and initialize textures.The glTextureSubImage2D function is a function used to update texture image data in OpenGL.*/
    [[nodiscard]] bool LoadFromMemory(int image_width, int image_height, const RGBA* colors) noexcept;

    /*This function connects or "binds" texture objects currently created by OpenGL to specific texture units. These bound textures are later referenced in graphical operations using the texture unit. 
    This allows the graphic elements to be rendered using images from the texture.*/
    void UseForSlot(unsigned int texture_unit) const noexcept;

    [[nodiscard]] unsigned int GetHandle() const noexcept
    {
        return texture_handle;
    }
    [[nodiscard]] int GetWidth() const noexcept
    {
        return width;
    }
    [[nodiscard]] int GetHeight() const noexcept
    {
        return height;
    }
    enum Filtering : GLint
    {
        NearestPixel = GL_NEAREST,
        Linear = GL_LINEAR
    };

    /*This function is responsible for setting the texture filtering mode.
    Sets texture filtering according to the specified filtering mode.*/
    void                    SetFiltering(Filtering how_to_filter) noexcept;


    [[nodiscard]] Filtering GetFiltering() const noexcept
    {
        return filtering;
    }
    enum Coordinate
    {
        S,
        T,
        Both
    };

    enum Wrapping : GLint
    {
        Repeat = GL_REPEAT,
        ClampToEdge = GL_CLAMP_TO_EDGE,
        ClampToBorder = GL_CLAMP_TO_BORDER,
        MirroredRepeat = GL_MIRRORED_REPEAT,
        MirrorClampToEdge = GL_MIRROR_CLAMP_TO_EDGE
    };

    /*This function is responsible for determining how OpenGL handles the image coordinates of textures. Simply put, it's a function that sets how to repeat or border images of textures. 
    This setting applies to the horizontal (S) and vertical (T) coordinate axes of the texture, respectively.*/
    void SetWrapping(Wrapping how_to_wrap, Coordinate coord = Coordinate::Both) noexcept;

    [[nodiscard]] std::array<Wrapping, 2> GetWrapping() const noexcept
    {
        return wrapping;
    }

private:
    /*This function is responsible for deleting textures.
    Use the OpenGL function glDeleteTextures to delete the texture and initialize the handle.
    Initialize by setting the width and height of the texture to zero.*/
    void delete_texture() noexcept;

private:
    GLHandle                texture_handle = 0;
    int                     width = 0;
    int                     height = 0;
    Filtering               filtering = Filtering::NearestPixel;
    std::array<Wrapping, 2> wrapping = { Wrapping::Repeat, Wrapping::Repeat };
};