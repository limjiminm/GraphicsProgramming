#include "GLVertexArray.h"
#include "glCheck.h"
#include<iostream>

GLVertexArray:: ~GLVertexArray()
{
	glCheck(glDeleteBuffers(1, &vertex_array_handle));
}

GLVertexArray::GLVertexArray(GLPrimitive::Type the_primitive_pattern)
	:primitive_pattern(the_primitive_pattern)
{
	glCheck(glCreateVertexArrays(1, &vertex_array_handle));
}

GLVertexArray::GLVertexArray(GLVertexArray&& temp) noexcept
{
	vertex_array_handle = temp.vertex_array_handle;
	vertex_buffers = std::move(temp.vertex_buffers);
	index_buffer = std::move(temp.index_buffer);
	num_indices = temp.num_indices;
	indices_type = temp.indices_type;
	primitive_pattern = temp.primitive_pattern;
	num_vertices = temp.num_vertices;

	temp.vertex_array_handle = 0;
}

GLVertexArray& GLVertexArray::operator=(GLVertexArray&& temp) noexcept
{
	vertex_array_handle = temp.vertex_array_handle;
	vertex_buffers = std::move(temp.vertex_buffers);
	index_buffer = std::move(temp.index_buffer);
	num_indices = temp.num_indices;
	indices_type = temp.indices_type;
	primitive_pattern = temp.primitive_pattern;
	num_vertices = temp.num_vertices;

	temp.vertex_array_handle = 0;
	return *this;
}

void GLVertexArray::Use(bool bind) const
{
	if (bind == true)
	{
		glBindVertexArray(vertex_array_handle);
	}
	else
	{
		glBindVertexArray(0);
	}
}

void GLVertexArray::AddVertexBuffer(GLVertexBuffer&& vertex_buffer, std::initializer_list<GLAttributeLayout> buffer_layout)
{
	for (auto& layout : buffer_layout)
	{
		glEnableVertexArrayAttrib(vertex_array_handle, layout.vertex_layout_location);
		glVertexArrayVertexBuffer(vertex_array_handle, layout.vertex_layout_location, vertex_buffer.GetHandle(), layout.offset, layout.stride);
		glVertexArrayAttribFormat(vertex_array_handle, layout.vertex_layout_location, layout.component_dimension, layout.component_type, layout.normalized, layout.relative_offset);
		glVertexArrayAttribBinding(vertex_array_handle, layout.vertex_layout_location, layout.vertex_layout_location);
	}

	vertex_buffers.emplace_back(std::move(vertex_buffer));
}

void GLVertexArray::SetIndexBuffer(GLIndexBuffer&& the_indices)
{
	glVertexArrayElementBuffer(vertex_array_handle, the_indices.GetHandle());
	num_indices = the_indices.GetCount();
	num_vertices = the_indices.GetCount();
	indices_type = the_indices.GetElementType();

	index_buffer = std::move(the_indices);
}

void GLDrawIndexed([[maybe_unused]] const GLVertexArray& vertex_array) noexcept
{
	glDrawElements(vertex_array.GetPrimitivePattern(), vertex_array.GetIndicesCount(), vertex_array.GetIndicesType(), NULL);
}

void GLDrawVertices([[maybe_unused]] const GLVertexArray& vertex_array) noexcept
{
	glDrawArrays(vertex_array.GetPrimitivePattern(), vertex_array.GetIndicesCount(), vertex_array.GetVertexCount());
}