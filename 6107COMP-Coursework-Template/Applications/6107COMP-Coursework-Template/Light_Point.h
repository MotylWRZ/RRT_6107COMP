#pragma once
#include "Light_Base.h"
class Light_Point : public Light_Base
{
	Light_Point();
	Light_Point(Vector4f lightColour, Vector3f lightPosition, Vector2f lightRange);
	virtual ~Light_Point() {};

	void setPointLightInfo(Vector4f lightColour, Vector3f lightPosition, Vector2f lightRange);
};

