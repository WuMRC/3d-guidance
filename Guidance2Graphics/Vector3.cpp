#include "Vector3.h"

// Custom Constructor
Vector3::Vector3(double val1, double val2, double val3)
{
	set(val1, val2, val3);
};

void Vector3::set(double val1, double val2, double val3)
{
	m_vBody.push_back(val1);
	m_vBody.push_back(val2);
	m_vBody.push_back(val3);
};

Vector3 Vector3::crossProduct(Vector3 v2)
{
	// Computes the cross product between instance's m_vBody and that of v2 utilizing the formula:
	//				<a,b,c> x <d,e,f>		=		<	b*f - c*e	,	c*d - a*f	,	a*e - b*f	>
	//
	// NOTE:	It is important to distinguish which instance is used as input and which is calling crossProduct:
	//				Suppose there are two instances of Vector3: vecA and vecB.
	//				Calling vecA.crossProduct(vecB) computes vecA x vecB, while vecB.crossProduct(vecA) computes vecB x vecA.
	//			This same logic is necessary when using the overidden * operator:
	//				Calling vecA * vecB computes vecA x vecB, while vecB * vecA computes vecB x vecA
	
	Vector3 result(	m_vBody[1]*v2.at(2) - m_vBody[2]*v2.at(1), 
					m_vBody[2]*v2.at(0) - m_vBody[0]*v2.at(2), 
					m_vBody[0]*v2.at(1) - m_vBody[1]*v2.at(0));
	return result;
}

double Vector3::dotProduct(Vector3 v2)
{
	// Computes the dot product between instance's m_vBody and that of v2 utilizing the formula:
	//				<a,b,c> * <d,e,f>		=		a*d		+		b*e		+		e*f
	//
	// NOTE:	Unlike Vector3::crossProduct, Vector3::dotProduct does not depend on the order of use or which instance calls the other
	
	return m_vBody[0] * v2.at(0) + m_vBody[1] * v2.at(1) + m_vBody[2] * v2.at(2);
}