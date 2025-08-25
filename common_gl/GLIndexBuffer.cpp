#include "GLIndexBuffer.h"

GLIndexBuffer::GLIndexBuffer(std::span<const unsigned int> indices)
{
	create_indices_buffer(indices);
	count = static_cast<GLsizei>(indices.size());
	element_type = GLIndexElement::UInt;
}

GLIndexBuffer::GLIndexBuffer(std::span<const unsigned short> indices)
{
	create_indices_buffer(indices);
	count = static_cast<GLsizei>(indices.size());
	element_type = GLIndexElement::UShort;
}

GLIndexBuffer::GLIndexBuffer(std::span<const unsigned char> indices)
{
	create_indices_buffer(indices);
	count = static_cast<GLsizei>(indices.size());
	element_type = GLIndexElement::UByte;
}

GLIndexBuffer::~GLIndexBuffer()
{
	glDeleteBuffers(1, &indices_handle);
}

GLIndexBuffer::GLIndexBuffer(GLIndexBuffer&& temp) noexcept
{
	indices_handle = temp.indices_handle;
	count = temp.count;
	element_type = temp.element_type;
	temp.indices_handle = 0;
	temp.count = 0;
	temp.element_type = GLIndexElement::None;
}

GLIndexBuffer& GLIndexBuffer::operator=(GLIndexBuffer&& temp) noexcept
{
	indices_handle = temp.indices_handle;
	count = temp.count;
	element_type = temp.element_type;
	temp.indices_handle = 0;
	temp.count = 0;
	temp.element_type = GLIndexElement::None;
	return *this;
}

void GLIndexBuffer::Use(bool bind) const
{
	if (bind == true)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_handle);
	}

	if (bind == false)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}