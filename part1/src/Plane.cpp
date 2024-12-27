#include "Plane.hpp"


Plane::Plane(float size, std::array<u_int8_t, 3> planeColorRGB) : m_size(size), m_planeColorRGB(planeColorRGB) {
    // Store the meshing plane
    int resolution = 0; // To add subdivisions, for right now let's not do any subdivisions.
    float edgeOfFaceSize = size / (resolution + 1);
    glm::vec3 triangleColorNormalized = glm::vec3(planeColorRGB[0] / 255.0f, 
                                                    planeColorRGB[1] / 255.0f, 
                                                    planeColorRGB[2] / 255.0f);

    for (int row = 0; row < resolution + 1; row++)  {
        for (int col = 0; col < resolution + 1; col++)  {
            Geometry::Vertex bottomLeft;
            bottomLeft.x = (-size / 2.0f) + (col * edgeOfFaceSize);
            bottomLeft.y = -1.0f;
            bottomLeft.z = -1 * row * edgeOfFaceSize;
            bottomLeft.tx = 0.0f; // TODO: Change these tex coords if implement resolutions
            bottomLeft.ty = 0.0f;
            Geometry::Vertex bottomRight;
            bottomRight = bottomLeft;
            bottomRight.x += edgeOfFaceSize;
            bottomRight.tx = 1.0f;
            bottomRight.ty = 0.0f;
            Geometry::Vertex topLeft;
            topLeft = bottomLeft;
            topLeft.z -= edgeOfFaceSize;
            topLeft.tx = 0.0f;
            topLeft.ty = 1.0f;
            Geometry::Vertex topRight;
            topRight = bottomRight;
            topRight.z -= edgeOfFaceSize;
            topRight.tx = 1.0f;
            topRight.ty = 1.0f;
            Geometry::Triangle bottomLeftTriangle;
            bottomLeftTriangle.vertices[0] = bottomLeft;
            bottomLeftTriangle.vertices[1] = bottomRight;
            bottomLeftTriangle.vertices[2] = topLeft;
            Geometry::SetTriangleColor(bottomLeftTriangle, triangleColorNormalized);
            Geometry::SetTriangleNormalsUp(bottomLeftTriangle);
            Geometry::Triangle topRightTriangle;
            topRightTriangle.vertices[0] = bottomRight;
            topRightTriangle.vertices[1] = topRight;
            topRightTriangle.vertices[2] = topLeft;
            Geometry::SetTriangleColor(topRightTriangle, triangleColorNormalized);
            Geometry::SetTriangleNormalsUp(topRightTriangle);
            m_mesh.push_back(bottomLeftTriangle);
            m_mesh.push_back(topRightTriangle);
        }
    }

    // Setup VAO
    // Vertex Arrays Object (VAO) Setup
	glGenVertexArrays(1, &m_vertexArrayObject);
    glBindVertexArray(m_vertexArrayObject);

    // Vertex Buffer Object (VBO) creation
    m_vboData = GetVertexBufferObjectData();
	glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, 						        // Kind of buffer we are working with 
                                                                // (e.g. GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER)
                    m_vboData.size() * sizeof(GL_FLOAT), 	// Size of data in bytes
                    m_vboData.data(), 					// Raw array of data
                    GL_STATIC_DRAW);							// How we intend to use the data
    
    // TODO: This is the exact same thing as in GraphicsProgram.cpp. This would be good do abstract. 
    // =============================
    // offsets every 3 floats
    // v     v     v
    // 
    // x,y,z,r,g,b,nx,ny,nz
    //
    // |------------------| strides is '9' floats
    //
    // ============================
    // Position information (x,y,z)
	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,sizeof(GL_FLOAT)*11,(void*)0);
    // Color information (r,g,b)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,sizeof(GL_FLOAT)*11,(GLvoid*)(sizeof(GL_FLOAT)*3));
    // Normal information (nx,ny,nz)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,sizeof(GL_FLOAT)*11, (GLvoid*)(sizeof(GL_FLOAT)*6));
    // Texture information (tx,ty)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE,sizeof(GL_FLOAT)*11, (GLvoid*)(sizeof(GL_FLOAT)*9));

	// Unbind our currently bound Vertex Array Object
	glBindVertexArray(0);
	// Disable any attributes we opened in our Vertex Attribute Arrray,
	// as we do not want to leave them open. 
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);

}

std::vector<GLfloat> Plane::GetVertexBufferObjectData()    {
    std::vector<GLfloat> result;
    for (Geometry::Triangle t : m_mesh)   {
        std::vector<GLfloat> vectorToInsert = Geometry::GetGLFloatVector(t);
        result.insert(result.end(), vectorToInsert.begin(), vectorToInsert.end());
    }
    return result;
}

void Plane::Draw()  {
    // Enable our attributes
	glBindVertexArray(m_vertexArrayObject);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo);
    //std::cout << "Number of vertices to draw: " << m_numVerticesToDraw << std::endl;
    //Render data
    glDrawArrays(GL_TRIANGLES, 0, m_mesh.size() * 3);

	// Stop using our current graphics pipeline
	// Note: This is not necessary if we only have one graphics pipeline.
    glUseProgram(0);
}