Texture2D       DiffuseTexture : register( t0 );
//Texture2D       DiffuseTexture2 : register(t1);
SamplerState    TextureSampler : register( s0 );

cbuffer TransformMatrices
{
    matrix WorldViewProjMatrix;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float4 rotationdata : COLOR; // x:speed, yzw:axis ---- Addition ---
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float2 tex : TEXCOORD0;
    float4 rotationdata : COLOR; // x:speed, yzw:axis ---- Addition ---
};

// ---- Addition Start ---
struct GS_INPUTOUTPUT
{
    float4 position : SV_Position;
    float2 tex : TEXCOORD0;
    float4 rotationdata : COLOR; // x:speed, yzw:axis ---- Addition ---
};

cbuffer Time
{
    float4 time;
};

float4x4 AngleAxis4x4(float angle, float3 axis)
{
    float c, s;
    sincos(angle, s, c);

    float t = 1 - c;
    float x = axis.x;
    float y = axis.y;
    float z = axis.z;

    return float4x4(
		t * x * x + c, t * x * y - s * z, t * x * z + s * y, 0,
		t * x * y + s * z, t * y * y + c, t * y * z - s * x, 0,
		t * x * z - s * y, t * y * z + s * x, t * z * z + c, 0,
		0, 0, 0, 1
		);
}

[maxvertexcount(3)]
void GSMain(triangle GS_INPUTOUTPUT input[3], inout TriangleStream<GS_INPUTOUTPUT> TriangleOutputStream)
{
    GS_INPUTOUTPUT output;

    float explosion_speed = 10;
    float explosion_radius = 180;
    float elapsed_time = time.x;
    float total_time = time.y;


	// calculate sine and cosine
    float c, s;
    sincos(total_time, s, c);
    s = saturate(s);
    c = saturate(c);

	// calculate the average rotation speed of the three vertices of the triangle
    float aveRotSpeed = (input[0].rotationdata.x + input[1].rotationdata.x + input[2].rotationdata.x) / 3;

	// calculate the average rotation axis of the three vertices of the triangle
    float3 axis = normalize(input[0].rotationdata.yzw + input[1].rotationdata.yzw + input[2].rotationdata.yzw);

	// get the rotation angle
    float rotAngle = aveRotSpeed * total_time * explosion_speed;

    rotAngle = rotAngle * s;

	// make sure it is within 0 - 2 x pi
    rotAngle = fmod(rotAngle, 2 * 3.141592);

	// calculate the rotation matrix - we modulate using the saturate(s) so that it starts opaque and
    float4x4 rotMat = AngleAxis4x4(rotAngle, axis);





	// calculate the center of the triangle. This willbe the origin of the primitive space
    float3 centre = (input[0].position.xyz + input[1].position.xyz + input[2].position.xyz) / 3;

    float3 outwardvector = normalize(centre);
    outwardvector = normalize(outwardvector + axis);

	// This is the current outward displacement of this primitive, i.e., how far from the centre
    float3 displacement = explosion_radius * outwardvector * s * aveRotSpeed;

	// Output vertices
    for (int i = 0; i < 3; i++)
    {

		// calculate the vertex location in the primitive space
        float4 position = input[i].position;

		// translate to origin
        position = float4(position.xyz - centre, 1);

		// Rotate the vertex using the calculated rotation matrix
        position = mul(position, rotMat);

		// translate the vertex back to its model space by offsetting by the primitive space origin
        position = float4(position.xyz + centre, 1);

		// here we translate the vertex by the displacement vector
        position = float4(position.xyz + displacement, 1);

		// then we do the rest of the transforms
        output.position = mul(position, WorldViewProjMatrix);
        output.tex = input[i].tex;

        output.rotationdata = float4(0, 0, 0, c); // ---- Addition ---

        TriangleOutputStream.Append(output);
    }

    TriangleOutputStream.RestartStrip();
}
// ---- Addition End ---


VS_OUTPUT VSMain(in VS_INPUT input)
{
    VS_OUTPUT output;

    output.position = float4(input.position, 1);

    output.tex = input.tex;

    output.rotationdata = input.rotationdata; // ---- Addition ---

    return output;
}


float4 PSMain(in VS_OUTPUT input) : SV_Target
{
    float4 pixelcolour = DiffuseTexture.Sample(TextureSampler, input.tex);

    pixelcolour.w = input.rotationdata.w;

    return (pixelcolour);
}

