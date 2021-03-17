#include "Light_Spot.h"

Light_Spot::Light_Spot()
{
	this->m_LightInfo.iLightType = Vector4f(static_cast<float>(ELightType::LIGHT_TYPE_SPOT), 0.0f, 0.0f, 0.0f);
}

Light_Spot::Light_Spot(Vector4f lightColour, Vector3f lightPosition, Vector3f lightDirection, Vector2f lightRange, Vector2f lightFocus)
{
	this->m_LightInfo.iLightType = Vector4f(static_cast<float>(ELightType::LIGHT_TYPE_SPOT), 0.0f, 0.0f, 0.0f);
	this->setSpotLightInfo(lightColour, lightPosition, lightDirection, lightRange, lightFocus);
}

void Light_Spot::setSpotLightInfo(Vector4f lightColour, Vector3f lightPosition, Vector3f lightDirection, Vector2f lightRange, Vector2f lightFocus)
{
	this->m_LightInfo.LightColour = lightColour;
	this->m_LightInfo.LightPosition = Vector4f(lightPosition, 0.0f);
	this->m_LightInfo.LightDirection = Vector4f(lightDirection, 0.0f);
	this->m_LightInfo.LightRange = Vector4f(lightRange.x, 0.0f, 0.0f, 0.0f);
	this->m_LightInfo.LightFocus = Vector4f(lightFocus.x, 0.0f, 0.0f, 0.0f);
	this->m_LightInfo.LightDirection.Normalize();
}
