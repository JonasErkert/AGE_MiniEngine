struct PS_Input
{
	float4 f4Position:			SV_Position;
	float4 f4PosWorld:			POSITION;
	float4 f4Color :			ASFD;
	float4 f4Normal:			NORMAL;
	float4 fCamPos:				POSITION2;
	float4 f4LightDirection:	NORMAL2;
};

cbuffer ColorChanging : register(b1)
{
	matrix mColorChanging: packoffset(c0);	// 4 Bytes * 16 = 64 Bytes
	matrix mPad1: packoffset(c4);			// 4 Bytes * 16 = 64 Bytes
	matrix mPad2: packoffset(c8);			// 4 Bytes * 16 = 64 Bytes
	matrix mPad3: packoffset(c12);			// 4 Bytes * 16 = 64 Bytes
	// Sum: 256 Bytes
}

float4 main(PS_Input input) : SV_TARGET
{
	// Fractional (0...1) light amount from the original quantity
	// Phong-Shading
	float fShadingForce = mul(input.f4Normal, -input.f4LightDirection);

	// Apply color changing matrix for color animation
	input.f4Color = mul(input.f4Color, mColorChanging);

	// Clamp color values
	input.f4Color = saturate(input.f4Color);

	// Weight color
	input.f4Color *= fShadingForce;

	// Return weighted color to the output merger
	return input.f4Color;
}
