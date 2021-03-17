#include "Light_Directional.h"

Light_Directional::Light_Directional()
{
	this->m_LightInfo.iLightType = Vector4f(static_cast<float>(ELightType::LIGHT_TYPE_DIRECTIONAL), 0.0f, 0.0f, 0.0f);
}

Light_Directional::Light_Directional(Vector4f lightColour, Vector3f lightDirection)
{
	this->m_LightInfo.iLightType = Vector4f(static_cast<float>(ELightType::LIGHT_TYPE_DIRECTIONAL), 0.0f, 0.0f, 0.0f);
	this->setDirectionalLightInfo(lightColour, lightDirection);
}

void Light_Directional::setDirectionalLightInfo(Vector4f lightColour, Vector3f lightDirection)
{
	this->m_LightInfo.LightColour = lightColour;
	this->m_LightInfo.LightPosition = Vector4f(lightDirection, 0.0f);
}
