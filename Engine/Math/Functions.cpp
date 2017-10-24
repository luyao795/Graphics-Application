// Include Files
//==============

#include "Functions.h"

#include <cmath>

// Interface
//==========

float eae6320::Math::ConvertHorizontalFieldOfViewToVerticalFieldOfView(const float i_horizontalFieldOfView_inRadians,
	const float i_aspectRatio)
{
	return 2.0f * std::atan(std::tan(i_horizontalFieldOfView_inRadians * 0.5f) / i_aspectRatio);
}

bool eae6320::Math::IsInputNaN(const float i_float)
{
	volatile float value = i_float;
	return value != value;
}

bool eae6320::Math::AreAboutEqual(const float i_first, const float i_second, const float i_epsilon)
{
	return fabs(i_second - i_first) <= i_epsilon;
}
