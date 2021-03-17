#include "Light_Point.h"

Light_Point::Light_Point()
{
	this->m_LightInfo.iLightType = Vector4f(static_cast<float>(ELightType::LIGHT_TYPE_POINT), 0.0f, 0.0f, 0.0f);
}

Light_Point::Light_Point(Vector4f lightColour, Vector3f lightPosition, Vector2f lightRange)
{
	this->m_LightInfo.iLightType = Vector4f(static_cast<float>(ELightType::LIGHT_TYPE_POINT), 0.0f, 0.0f, 0.0f);
	this->setPointLightInfo(lightColour, lightPosition, lightRange);
}

void Light_Point::setPointLightInfo(Vector4f lightColour, Vector3f lightPosition, Vector2f lightRange)
{
	this->m_LightInfo.LightColour = lightColour;
	this->m_LightInfo.LightPosition = Vector4f(lightPosition, 0.0f);
	this->m_LightInfo.LightRange = Vector4f(lightRange.x, lightRange.y, 0.0f, 0.0f);
}


