struct VS_Input
{
	float3 f3Pos		: POSITION;
	float3 f3Normal		: NORMAL;
	float3 f3Tangent	: TANGENT;
	float3 f3Bitangent	: BITANGENT;
	float3 f3TexCoord	: TEXCOORD;
};

struct VS_Output
{
	float4 f4Position		: SV_Position;
	float4 f4PosWorld		: POSITION;
	float4 f4Color			: ASFD;
	float4 f4Normal			: NORMAL;
	float4 f4CamPos			: POSITION2;
	float4 f4LightDirection	: NORMAL2;
};

cbuffer ObjectInfo : register(b0)
{
	matrix mWorld:		packoffset(c0);			// 4 Bytes * 16 = 64 Bytes
	matrix mView:		packoffset(c4);			// 4 Bytes * 16 = 64 Bytes
	matrix mProj:		packoffset(c8);			// 4 Bytes * 16 = 64 Bytes
	float4 f4CamPos:	packoffset(c12);		// 4 Bytes * 4 = 16 Bytes
	float4 f4LightDirection: packoffset(c13);	// 4 Bytes * 4 = 16 Bytes
	float4 f4Pad1:		packoffset(c14);		// 4 Bytes * 4 = 16 Bytes
	float4 f4Pad2s:		packoffset(c15);		// 4 Bytes * 4 = 16 Bytes
	// Sum: 256 Bytes
}

// #IMPORTANT
VS_Output main(VS_Input input)
{
	VS_Output output;
	
	// Convert vector3 into vector4
	float4 f4PosInput;
	f4PosInput.xyz = input.f3Pos;
	f4PosInput.w = 1.0f;

	// World transformation
	output.f4Position = mul(f4PosInput, mWorld);

	output.f4PosWorld = output.f4Position;

	// Unify view- and projection matrices 
	matrix mViewProj = mul(mView, mProj);

	// Project points correctly and consider camera position when calculating
	output.f4Position = mul(output.f4Position, mViewProj);

	// Calculate normals
	output.f4Normal.xyz = input.f3Normal;
	output.f4Normal.w = 0.0f; // Direction vector => 0
	output.f4Normal = mul(output.f4Normal, mWorld);

	// Camera pos
	output.f4CamPos = f4CamPos;

	// Light dir
	output.f4LightDirection = f4LightDirection;
	
	output.f4Color = float4(0.0f, 1.0f, 0.0f, 1.0f); // Green, no transparency

	return output;
}
