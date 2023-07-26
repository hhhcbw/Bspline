#ifndef BSPLINE_H
#define BSPLINE_H

#include "basis.h"
using namespace std;

class BsplineCurve : public BasisCurve
{
public:
    // default constructor
    BsplineCurve() = default;

    /**
     * @brief      Parameter constructor
     * @param[in]  controlPoints  Control points
     * @param[in]  knots          Knots array
     * @param[in]  p              Degree(decide curve continuity)
     * @param[in]  count          Number of segments
     */
    BsplineCurve(const vector<glm::vec3>& controlPoints, const vector<float>& knots, const int p = 3, const int count = 100)
        : BasisCurve(controlPoints, count), m_p(p), m_knots(knots), m_isRational(false)
    {
    }

    /**
     * @brief      NURBS parameter constructor
     * @param[in]  controlPoints  Control points
     * @param[in]  knots          Knots array
     * @param[in]  weights        Weights of control points
     * @param[in]  p              Degree(decide curve continuity)
     * @param[in]  count          Number of segments
     */
    BsplineCurve(const vector<glm::vec3>& controlPoints,
                 const vector<float>& knots,
                 const vector<float>& weights,
                 const int p = 3, 
                 const int count = 100)
        : BasisCurve(controlPoints, count), m_knots(knots), m_weights(weights), m_p(p), m_isRational(true)
    {
    }

protected:
    int m_p; // degree
    vector<float> m_knots; // knots array
    vector<float> m_weights; // weights of control points
    bool m_isRational;       // rational bspline curve or not

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
        const int pos = upper_bound(m_knots.begin(), m_knots.end(), u) - m_knots.begin() - 1;
        vector<float> basis_func(m_p+1, 0.0f);
        basis_func[0] = 1.0f;
        for (int i = 1; i <= m_p; i++)
        {
            for (int j = i; j >= 0; j--) // reverse order make sure the update of basis function is correct
			{
                int k = pos - i + j;
                if (k < 0 || k >= m_knots.size() - i - 1) continue;
                
                if (j == 0)
                {
                    basis_func[j] = (m_knots[k + i + 1] - u) / (m_knots[k + i + 1] - m_knots[k + 1]) * basis_func[j];
                }
                else if (j == i)
                {
                    basis_func[j] = (u - m_knots[k]) / (m_knots[k + i] - m_knots[k]) * basis_func[j - 1];
                }
                else
                {
                    basis_func[j] = (u - m_knots[k]) / (m_knots[k + i] - m_knots[k]) * basis_func[j - 1] +
                        (m_knots[k + i + 1] - u) / (m_knots[k + i + 1] - m_knots[k + 1]) * basis_func[j];
                }
            }
        }
        glm::vec3 vertex(0.0f);
        if (!m_isRational) // non-rational condition
        {
            for (int i = m_p; i >= 0; i--)
		    {
                if (pos - i >= 0 && pos - i < size)
                {
                    vertex += m_controlPoints[pos - i] * basis_func[m_p - i];
		        }
            }
        }
        else // rational condition
        {
            float w = 0.0f;
            for (int i = m_p; i >= 0; i--)
            {
                if (pos - i >= 0 && pos - i < size)
                {
                    vertex += m_controlPoints[pos - i] * m_weights[pos - i] * basis_func[m_p - i];
                    w += m_weights[pos - i] * basis_func[m_p - i];
                }
            }
            vertex /= w;
        }

        m_vertices.push_back(vertex);
    }
};
#endif