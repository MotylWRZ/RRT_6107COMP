#include "MeshImporter.h"



//Include our own application Namespace


// #3: Header file include
#include "LJMUMeshOBJ.h"

#include <FileSystem.h>
#include <SamplerStateConfigDX11.h>

BasicMeshPtr MeshImporter::generateMeshOBJ(std::wstring meshOBJFile, Vector4f meshColour)
{
	FileSystem fs;
	LJMUDX::LJMUMeshOBJ* tmesh = new LJMUDX::LJMUMeshOBJ(fs.GetModelsFolder() + meshOBJFile);
	int tvertcount = tmesh->positions.size();

	auto tia = std::make_shared<DrawExecutorDX11<RRTVertexDX11::Vertex>>();
	tia->SetLayoutElements(RRTVertexDX11::GetElementCount(), RRTVertexDX11::Elements);
	tia->SetPrimitiveType(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	tia->SetMaxVertexCount(tvertcount);

	RRTVertexDX11::Vertex tv;
	tv.color = meshColour;

	for (auto& tobject : tmesh->objects)
	{
		for (auto& tface : tobject.faces)
		{
			for (size_t i = 0; i < 3; ++i)
			{
				tv.position = tmesh->positions[tface.PositionIndices[i]];
				tv.normal = tmesh->normals[tface.NormalIndices[i]];
				tv.texcoords = tmesh->coords[tface.CoordIndices[i]];

				tia->AddVertex(tv);
			}
		}
	}
	return tia;
}

BasicMeshPtr MeshImporter::generateMeshOBJWithSurfaceVectors(std::wstring meshOBJFile, Vector4f meshColour)
{
	FileSystem fs;
	LJMUDX::LJMUMeshOBJ* tmesh = new LJMUDX::LJMUMeshOBJ(fs.GetModelsFolder() + meshOBJFile);
	int tvertcount = tmesh->positions.size();

	auto tia = std::make_shared<DrawExecutorDX11<RRTVertexDX11::Vertex>>();
	tia->SetLayoutElements(RRTVertexDX11::GetElementCount(), RRTVertexDX11::Elements);
	tia->SetPrimitiveType(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	tia->SetMaxVertexCount(tvertcount);

	RRTVertexDX11::Vertex tv;
	tv.color = meshColour;

	for (auto& tobject : tmesh->objects)
	{
		for (auto& tface : tobject.faces)
		{
			for (size_t i = 0; i < 3; ++i)
			{
				tv.position = tmesh->positions[tface.PositionIndices[i]];
				tv.normal = tmesh->normals[tface.NormalIndices[i]];
				tv.texcoords = tmesh->coords[tface.CoordIndices[i]];
				tv.tangent = Vector3f(0, 1, 0);
				tv.binormal = Vector3f(0, 0, 1);

				tia->AddVertex(tv);
			}
		}
	}
	return tia;
}