#ifndef BASIS_H
#define BASIS_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

#include <string>
#include <vector>
using namespace std;

class BasisCurve
{
public:
    // default constructor
    BasisCurve() = default;

    // constructor
    BasisCurve(const vector<glm::vec3>& controlPoints, const float l = 0, const float r = 1, const int count = 100)
        : m_l(l), m_r(r), m_count(count)
    {
        for (unsigned int id = 0; id < controlPoints.size(); id++)
        {
            m_controlPointsId.push_back((float)id);
            m_controlPoints.push_back(controlPoints[id]);
        }
    }

    // initial method
    virtual void init()
    {
        createDrawVertices();
        initDrawConfig();
    }

    // update draggindId control points
    void Update(const unsigned int draggingId, const glm::vec3 dir)
    {
        m_controlPoints[draggingId] += dir;
        m_vertices.clear();
        createDrawVertices();
        //glBindVertexArray(VAO_controlPoints);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_controlPoints);

        glBufferSubData(GL_ARRAY_BUFFER, 0, m_controlPoints.size() * sizeof(glm::vec3), &m_controlPoints[0]);

        //glBindBuffer(GL_ARRAY_BUFFER, 0);
        //glBindVertexArray(0);

        //glBindVertexArray(VAO_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);

        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), &m_vertices[0], GL_STREAM_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //glBindVertexArray(0);
    }

    // draw curve
    void Draw(Shader& shader)
    {
        DrawControlPoints(shader);
        DrawVertices(shader);
    }

    void DrawControlPoints(Shader& shader)
    {
        glBindVertexArray(VAO_controlPoints);
        //glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
        glDrawArrays(GL_POINTS, 0, m_controlPoints.size());
        glBindVertexArray(0);
    }

    void DrawVertices(Shader& shader)
    {
        glBindVertexArray(VAO_vertices);
        glDrawArrays(GL_LINE_STRIP, 0, m_vertices.size());
        glBindVertexArray(0);
    }

protected:
    vector<glm::vec3> m_controlPoints;
    vector<glm::vec3> m_vertices;
    vector<float> m_controlPointsId;
    float m_l, m_r;
    unsigned int VAO_controlPoints, VBO_controlPoints, VAO_vertices, VBO_vertices;
    int m_count;

    // initialize vertex buffers and vertex arrays
    void initDrawConfig()
    {
        // Part1: initialize control points
        // create buffers/arrays
        glGenVertexArrays(1, &VAO_controlPoints);
        glGenBuffers(1, &VBO_controlPoints);

        glBindVertexArray(VAO_controlPoints);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO_controlPoints);
        glBufferData(GL_ARRAY_BUFFER,
                     m_controlPoints.size() * sizeof(glm::vec3) + m_controlPointsId.size() * sizeof(float),
                     NULL,
                     GL_STREAM_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_controlPoints.size() * sizeof(glm::vec3), &m_controlPoints[0]);
        glBufferSubData(GL_ARRAY_BUFFER,
                        m_controlPoints.size() * sizeof(glm::vec3),
                        m_controlPointsId.size() * sizeof(float),
                        &m_controlPointsId[0]);
        //  set the vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            1, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)(m_controlPoints.size() * sizeof(glm::vec3)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Part2: initialize vertices
        // create buffers/arrays
        glGenVertexArrays(1, &VAO_vertices);
        glGenBuffers(1, &VBO_vertices);

        glBindVertexArray(VAO_vertices);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), &m_vertices[0], GL_STREAM_DRAW);

        //  set the vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

private:
    // create draw vertices according to control points and parameter domain
    virtual void createDrawVertices()
    {
        for (int i = 0; i < m_controlPoints.size() - 1; i++)
        {
            for (int j = 0; j <= m_count / (m_controlPoints.size()-1); j++)
            {
                float ratio = (float)j / (float)(m_count / (m_controlPoints.size() - 1));
                m_vertices.push_back(m_controlPoints[i] * (1 - ratio) + m_controlPoints[i+1] * ratio);
			}
		}
    }
};
#endif