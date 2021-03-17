#pragma once

#include "RendererDX11.h"
#include "MaterialDX11.h"


#include "SamplerStateConfigDX11.h"

//Include the Logging System
#include "Log.h"

//Include the DirectX Rendering Components
#include "PipelineManagerDX11.h"
#include "BlendStateConfigDX11.h"
#include "BufferConfigDX11.h"
#include "DepthStencilStateConfigDX11.h"
#include "RasterizerStateConfigDX11.h"
#include "SwapChainConfigDX11.h"
#include "Texture2dConfigDX11.h"
#include "MaterialGeneratorDX11.h"

using namespace Glyph3;

//#define LIGHT_TYPE_NONE			 0
//#define LIGHT_TYPE_POINT         1
//#define LIGHT_TYPE_SPOT          2
//#define LIGHT_TYPE_DIRECTIONAL   3

enum class ELightType
{
	LIGHT_TYPE_NONE,
	LIGHT_TYPE_POINT,
	LIGHT_TYPE_SPOT,
	LIGHT_TYPE_DIRECTIONAL
};

struct LightInfo
{
	Vector4f iLightType = Vector4f(static_cast<float>(ELightType::LIGHT_TYPE_NONE), 0.0f, 0.0f, 0.0f);
	Vector4f LightColour = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
	Vector4f LightPosition = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
	Vector4f LightDirection = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
	Vector4f LightRange = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
	Vector4f LightFocus = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
};

// Abstract base class for all light types
class Light_Base
{
public:
	Light_Base() {};
	virtual ~Light_Base() = 0 {};

	void setLightInfo(LightInfo Info) { m_LightInfo = Info; }
	inline const LightInfo& getLightInfo() const { return m_LightInfo; }

protected:
	LightInfo m_LightInfo;
};

