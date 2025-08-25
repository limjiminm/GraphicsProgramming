#include "GLVertexBuffer.h"
#include <GLHandle.h>
#include <GL/glew.h>

GLVertexBuffer::GLVertexBuffer(GLsizei size_in_bytes) : size(size_in_bytes) {
    glGenBuffers(1, &buffer_handle);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_handle);
    glNamedBufferStorage(buffer_handle, size_in_bytes, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLVertexBuffer::~GLVertexBuffer() {
    glDeleteBuffers(1, &buffer_handle);
}

GLVertexBuffer::GLVertexBuffer(GLVertexBuffer&& temp) noexcept {
    *this = std::move(temp);
}

GLVertexBuffer& GLVertexBuffer::operator=(GLVertexBuffer&& temp) noexcept {
    if (this != &temp) {
        size = temp.size;
        buffer_handle = temp.buffer_handle;
        temp.size = 0;
        temp.buffer_handle = 0;
    }
    return *this;
}

void GLVertexBuffer::Use(bool bind) const {
    if (bind) {
        glBindBuffer(GL_ARRAY_BUFFER, buffer_handle);
    }
    else {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void GLVertexBuffer::send_buffer_data_to_gpu(const void* data, GLsizei size_bytes, GLsizei starting_offset) const {
    glBindBuffer(GL_ARRAY_BUFFER, buffer_handle);
    glNamedBufferSubData(buffer_handle, starting_offset, size_bytes, data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}