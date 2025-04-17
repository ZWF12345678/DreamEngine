#include "Math/MathUtility.h"

float HMath::Fmod(float X, float Y)
{
	const float AbsY = HMath::Abs(Y);
	if (AbsY <= DE_SMALL_NUMBER) // Note: this constant should match that used by VectorMod() implementations
	{
		FmodReportError(X, Y);
		return 0.0;
	}

	return fmodf(X, Y);
}

double HMath::Fmod(double X, double Y)
{
	const double AbsY = HMath::Abs(Y);
	if (AbsY <= DE_DOUBLE_SMALL_NUMBER) // Note: this constant should match that used by VectorMod() implementations
	{
		FmodReportError(X, Y);
		return 0.0;
	}

	return fmod(X, Y);
}

void HMath::FmodReportError(float X, float Y)
{
	if (Y == 0)
	{
        //TODO: Add log error message
		// KASSERT_MSG(Y != 0, "HMath::FMod(X=%f, Y=%f) : Y is zero, this is invalid and would result in NaN!", X, Y);
	}
}

void HMath::FmodReportError(double X, double Y)
{
	if (Y == 0)
	{
        //TODO: Add log error message
		//KASSERT_MSG(Y != 0, "FMath::FMod(X=%f, Y=%f) : Y is zero, this is invalid and would result in NaN!", X, Y);
	}
}
