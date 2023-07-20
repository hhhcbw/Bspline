#ifndef SPLINE_H
#define SPLINE_H

#include "basis.h"

using namespace std;

#define pow3(x) x*x*x

class SplineCurve : public BasisCurve
{
public:
	// default constructor
	SplineCurve() = default;

	// constructor
	SplineCurve(const vector<glm::vec3>& controlPoints, const float l = 0, const float r = 1, const int count = 100)
		: BasisCurve(controlPoints, l, r, count)
	{
	}

protected:
	// diagonal elements of tridiagonal matrix
    vector<glm::vec3> m_diag;
	vector<glm::vec3> m_upper;
	vector<glm::vec3> m_lower;

	vector<glm::vec3> m_b;
    vector<glm::vec3> m_v;
    vector<glm::vec3> m_M;

	vector<glm::vec3> m_y;

private:
	// SplineCurve initial method
	void init() override
	{
		int size = m_controlPoints.size();
        m_diag.resize(size - 1);
        m_upper.resize(size - 1);
        m_lower.resize(size - 1);
        m_b.resize(size - 1);
        m_v.resize(size - 1);
        m_M.resize(size);
        m_y.resize(size - 1);

		createDrawVertices();
        initDrawConfig();
	}

	// create draw vertices according to control points and parameter domain
	void createDrawVertices() override
	{
		// create tridiagonal matrix
        int size = m_controlPoints.size();
        
		for (int i = 0; i < size - 1; i++)
        {
            m_upper[i] = m_lower[i] = glm::vec3(1.0f);
            m_b[i] = 6.0f / m_upper[i] * (m_controlPoints[i + 1] - m_controlPoints[i]);
            if (i > 0)
            {
                m_diag[i] = 2.0f * (m_upper[i] + m_upper[i - 1]);
                m_v[i] = m_b[i] - m_b[i - 1];
            }
        }
		
		// solve tridiagonal matrix
		for (int i = 1; i < size - 2; i++)
		{
            m_lower[i] = m_lower[i] / m_diag[i];
			m_diag[i + 1] = m_diag[i + 1] - m_lower[i] * m_upper[i];
		}

		m_y[1] = m_v[1];
		for (int i = 2; i < size - 1; i++)
		{
			m_y[i] = m_v[i] - m_lower[i - 1] * m_y[i - 1];
		}

		m_M[0] = m_M[size - 1] = glm::vec3(0.0f);
		m_M[size - 2] = m_y[size - 2] / m_diag[size - 2];
		for (int i = size - 3; i > 0; i--)
		{
			m_M[i] = (m_y[i] - m_upper[i] * m_M[i + 1]) / m_diag[i];
		}

		// create draw vertices
        for (int i = 0; i < size - 1; i++)
        {
            for (int j = 0; j <= m_count / (size - 1); j++)
            {
                float ratio = (float)j / (float)(m_count / (size - 1));
                //m_vertices.push_back(pow3(ratio) * m_M[i] / 6.0f +
                //                     pow3(1.0f - ratio) * m_M[i + 1] / 6.0f +
                //                     (m_controlPoints[i] - m_M[i + 1] / 6.0f) * (1.0f - ratio) +
                //                     (m_controlPoints[i + 1] - m_M[i] / 6.0f) * ratio);
                //std::cout << (pow3(ratio) * m_M[i+1] / 6.0f).x << std::endl;
                std::cout << ((m_controlPoints[i + 1] - m_M[i + 1] / 6.0f) * ratio).x << std::endl;
                m_vertices.push_back(pow3((1.0f - ratio)) * m_M[i] / 6.0f +
                                     pow3(ratio) * m_M[i + 1] / 6.0f +
                                     (m_controlPoints[i] - m_M[i] / 6.0f) * (1 - ratio) +
                                     (m_controlPoints[i + 1] - m_M[i + 1] / 6.0f) * ratio);
                //m_vertices.push_back(pow3((1.0f - ratio)) * m_M[i] / 6.0f / m_upper[i] +
                //                     pow3(ratio) * m_M[i + 1] / 6.0f / m_upper[i] +
                //                     (m_controlPoints[i] / m_upper[i] - m_M[i + 1] * m_upper[i] / 6.0f) * ratio +
                //                     (m_controlPoints[i + 1] / m_upper[i] - m_M[i] * m_upper[i] / 6.0f) * (1 - ratio));
            }
        }

		//for (int i = 0; i < m_vertices.size(); i++)
		//{
		//	m_vertices[i].z = 0.0f;
		//}
	}
};
#endif