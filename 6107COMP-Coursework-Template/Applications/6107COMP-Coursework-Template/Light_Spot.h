#pragma once
#include "Light_Base.h"
class Light_Spot : public Light_Base
{
public:
	Light_Spot();
	Light_Spot(Vector4f lightColour, Vector3f lightPosition, Vector3f lightDirection, Vector2f lightRange, Vector2f lightFocus);
	virtual ~Light_Spot() {};

	void setSpotLightInfo(Vector4f lightColour, Vector3f lightPosition, Vector3f lightDirection, Vector2f lightRange, Vector2f lightFocus);
};

