#include <vector>
class Vector3
{
public:

	// Classic Constructor
	Vector3();

	// Custom Constructor
	Vector3(double val1, double val2, double val3);

	Vector3 crossProduct(Vector3 v2);

	double dotProduct(Vector3 v2);

	Vector3 operator*(Vector3 v2) { return crossProduct(v2);};
	Vector3 operator-(Vector3 v2) { 
		Vector3 subtracted(m_vBody[0] - v2.at(0), m_vBody[1] - v2.at(1), m_vBody[2] - v2.at(2));
		return subtracted;
	};
	Vector3 operator/(double val) { 
		Vector3 divided(m_vBody[0] / val, m_vBody[1] / val, m_vBody / val);
		return divided;
	};

	double at(unsigned short int index) const {return m_vBody.at(index);};

	std::vector<double> getVec() const { return m_vBody;};

	// Classic Destructor
	~Vector3(void){};
private:
	std::vector<double> m_vBody;
};