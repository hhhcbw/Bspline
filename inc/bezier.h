#ifndef BEZIER_H
#define BEZIER_H

#include "basis.h"
using namespace std;

class BezierCurve : public BasisCurve
{
public:
    // default constructor
    BezierCurve() = default;

    // basis bezier curve constructor
    BezierCurve(const vector<glm::vec3>& controlPoints, const int count = 100)
        : BasisCurve(controlPoints, count), m_isRational(false)
    {
	}

    // rational bezier curve constructor
    BezierCurve(const vector<glm::vec3>& controlPoints, const vector<float>& weights, const int count = 100)
        : BasisCurve(controlPoints, count), m_isRational(true)
	{
        m_weights.assign(weights.begin(), weights.end());
	}

protected:
    vector<float> m_weights; // weights of control points
    bool m_isRational; // rational bezier curve or not

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
        if (!m_isRational) // non-rational condition
        {
            for (int i = 0; i < size; i++)
            {
                temp.push_back(m_controlPoints[i]);
            }

            for (int i = 0; i < size; i++)
                for (int j = 0; j < size - i - 1; j++)
                {
                    temp[j] = (1.0f - u) * temp[j] + u * temp[j + 1];
                }
        }
		else // rational condition
		{
            vector<float> weights(m_weights);
			for (int i = 0; i < size; i++)
			{
				temp.push_back(m_controlPoints[i] * m_weights[i]);
			}

            for (int i = 0; i < size; i++)
                for (int j = 0; j < size - i - 1; j++)
                {
                    temp[j] = (1.0f - u) * temp[j] + u * temp[j + 1];
                    weights[j] = (1.0f - u) * weights[j] + u * weights[j + 1];
                }

            temp[0] /= weights[0];
		}

        m_vertices.push_back(temp[0]);
    }
};
#endif