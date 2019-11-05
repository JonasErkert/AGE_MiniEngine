struct PS_Input
{
	float4 f4Position : SV_Position;
	float4 f4Color : ASDF;
};

float4 main(PS_Input input) : SV_TARGET
{
	return input.f4Color;
}
