#ifndef BEZIER_H
#define BEZIER_H

#include "basis.h"
using namespace std;

class BezierCurve : public BasisCurve
{
public:
    // default constructor
    BezierCurve() = default;

    // constructor
    BezierCurve(const vector<glm::vec3>& controlPoints, const float l = 0, const float r = 1, const int count = 100)
        : BasisCurve(controlPoints, l, r, count)
    {
	}

private:
    // create draw vertices according to control points and parameter domain
    void createDrawVertices() override
    {
        float u = 0;
        float delta = 1.0f / (float)m_count;
        for (int i = 0; i <= m_count; i++)
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
        for (int i = 0; i < size; i++)
            for (int j = 0; j < size - i - 1; j++)
            {
                temp[j] = (1.0f - u) * temp[j] + u * temp[j + 1];
            }
        m_vertices.push_back(temp[0]);
    }
};
#endif