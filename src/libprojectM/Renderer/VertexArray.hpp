#pragma once

#include "Renderer/OpenGL.h"

namespace libprojectM {
namespace Renderer {

/**
 * @brief Wraps a vertex array object.
 * Creates, destroys and binds a single VAO.
 */
class VertexArray
{
public:
    /**
     * Constructor. Creates a new VAO.
     * @param glContext The OpenGL context to use.
     */
    VertexArray(const GladGLContext& glContext)
        : m_glContext(glContext)
    {
        m_glContext.GenVertexArrays(1, &m_vaoID);
    }

    /**
     * Destructor. Deletes the stored VAO.
     */
    virtual ~VertexArray()
    {
        m_glContext.DeleteVertexArrays(1, &m_vaoID);
        m_vaoID = 0;
    }

    /**
     * Binds the stored VAO.
     */
    void Bind() const
    {
        m_glContext.BindVertexArray(m_vaoID);
    }

    /**
     * Binds the default VAO with ID 0.
     */
    void Unbind() const
    {
        m_glContext.BindVertexArray(0);
    }

    /**
     * Binds the default VAO with ID 0.
     * @param glContext The OpenGL context to use.
     */
    static void Unbind(const GladGLContext& glContext)
    {
        glContext.BindVertexArray(0);
    }

private:
    GLuint m_vaoID{0}; //!< The vertex array object ID for this mesh's vertex data.

    const GladGLContext& m_glContext; //!< The glad OpenGL context information used for rendering.
};


} // namespace Renderer
} // namespace libprojectM
