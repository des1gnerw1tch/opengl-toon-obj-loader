/** @file Geometry.hpp
 *  @brief Holds geometry data types
 *  
 *  Includes triangle and vertex
 *  
 *
 *  @author Zachary Walker-Liang
 *  @bug No known bugs.
 */

#pragma once

#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <vector>
#include <functional>

namespace Geometry  {
    struct Vertex{
        float x,y,z;    // position
        float r,g,b; 	// color
        float nx,ny,nz; // normals
        float tx,ty; // texture coordinates
    };

    //TODO: This should really be a class. 
    struct Triangle{
        Vertex vertices[3]; // 3 vertices per triangle
    };

    // Triangle that has indexes of vertices
    struct IndexedTriangle  {
        int vertexIndices[3];
    };

    inline void SetTriangleNormalsUp(Triangle &t) {
        for (int i = 0; i < 3; i++) {
            t.vertices[i].nx = 0;
            t.vertices[i].ny = 1;
            t.vertices[i].nz = 0;
        }
    }

    inline void SetTriangleColor(Triangle &t, glm::vec3 c)   {
        for (int i = 0; i < 3; i++) {
            t.vertices[i].r = c[0];
            t.vertices[i].g = c[1];
            t.vertices[i].b = c[2];
        }
    }

    inline std::vector<GLfloat> GetGLFloatVector(const Triangle &t)   {
        std::vector<GLfloat> result;
        for (int i = 0; i < 3; i++) {
            Vertex v = t.vertices[i];
            result.push_back(v.x);
            result.push_back(v.y);
            result.push_back(v.z);
            result.push_back(v.r);
            result.push_back(v.g);
            result.push_back(v.b);
            result.push_back(v.nx);
            result.push_back(v.ny);
            result.push_back(v.nz);
            result.push_back(v.tx);
            result.push_back(v.ty);
        }
        return result;
    }

    // =============================
    // offsets every 3 floats
    // v     v     v
    // 
    // x,y,z,r,g,b,nx,ny,nz
    //
    // |------------------| strides is '9' floats
    //
    // ============================
    inline std::vector<GLfloat> GetGLFloatVector(const std::vector<Geometry::Vertex> vertices) {
        std::vector<GLfloat> result;
        for (Geometry::Vertex v : vertices)   {
            result.push_back(v.x);
            result.push_back(v.y);
            result.push_back(v.z);
            result.push_back(v.r);
            result.push_back(v.g);
            result.push_back(v.b);
            result.push_back(v.nx);
            result.push_back(v.ny);
            result.push_back(v.nz);
            result.push_back(v.tx);
            result.push_back(v.ty);
        }
        return result;
    }


    inline std::vector<GLuint> GetElementArrayFromTriangles(const std::vector<Geometry::IndexedTriangle> &triangles)  {
        std::vector<GLuint> result;
        for (Geometry::IndexedTriangle t : triangles)   {
            result.push_back(t.vertexIndices[0]);
            result.push_back(t.vertexIndices[1]);
            result.push_back(t.vertexIndices[2]);
        }
        return result;
    }
}