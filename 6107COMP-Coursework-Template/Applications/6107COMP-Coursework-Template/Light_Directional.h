#pragma once
#include "Light_Base.h"
class Light_Directional : public Light_Base
{
	Light_Directional();
	Light_Directional(Vector4f lightColour, Vector3f lightDirection);
	virtual ~Light_Directional() {};

	void setDirectionalLightInfo(Vector4f lightColour, Vector3f lightDirection);
};

