#ifndef BEZIER_H
#define BEZIER_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

#include <string>
#include <vector>
using namespace std;

class BezierCurve {
public:
	// default constructor
	BezierCurve() = default;

	// constructor
	BezierCurve(const vector<glm::vec3>& controlPoints, const double l = 0, const double r = 1, const int count = 100) : m_l(l), m_r(r)
	{
		for (auto x : controlPoints)
		{
			m_controlPoints.push_back(x);
		}
		init(count);
	}

	// draw bezier curve
	void Draw(Shader &shader)
	{
		glBindVertexArray(VAO_controlPoints);
		glDrawArrays(GL_POINTS, 0, m_controlPoints.size());
		glBindVertexArray(VAO_vertices);
		glDrawArrays(GL_LINE_STRIP, 0, m_vertices.size());
		glBindVertexArray(0);
	}

private:
	vector<glm::vec3> m_controlPoints;
	vector<glm::vec3> m_vertices;
	double m_l, m_r;
	unsigned int VAO_controlPoints, VBO_controlPoints, VAO_vertices, VBO_vertices;

	// initial method
	void init(const int count)
	{
		createDrawVertices(count);
		initDrawConfig();
	}
    
	// create draw vertices according to control points and parameter domain
	void createDrawVertices(const int count)
	{
		float u = 0;
		float delta = 1.0f / (float)count;
		for (int i = 0; i <= count; i++)
		{
			createVertexByU(u);
			u += delta;
		}
	}

	// create vertex by parameter u
	void createVertexByU(const float u)
	{
		const int size = m_controlPoints.size();
		vector<glm::vec3> temp;
		for (auto x : m_controlPoints)
		{
			temp.push_back(x);
		}
		for (int i = 0; i < size; i ++)
			for (int j = 0; j < size - i - 1; j++)
			{
				temp[j] = (1.0f - u) * temp[j] + u * temp[j + 1];
			}
		m_vertices.push_back(temp[0]);
	}

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
		glBufferData(GL_ARRAY_BUFFER, m_controlPoints.size() * sizeof(glm::vec3), &m_controlPoints[0], GL_STREAM_DRAW);

		//  set the vertex attribute pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glEnableVertexAttribArray(0);

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
};
#endif