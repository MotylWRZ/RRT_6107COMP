#pragma once

#include "PCH.h"
#include "Vector2f.h"
#include "Vector3f.h"
#include "Vector4f.h"

using namespace Glyph3;


class RRTVertexDX11
{
public:
	RRTVertexDX11();
	~RRTVertexDX11();

	struct Vertex
	{
		Vector3f position;
		Vector3f normal;
		Vector3f tangent;
		Vector3f binormal;
		Vector4f color;
		Vector2f texcoords;
	};

	static unsigned int GetElementCount();
	static D3D11_INPUT_ELEMENT_DESC Elements[6];
};

