struct PS_Input
{
	float4 f4Position : SV_Position;
	float4 f4Color : ASFD;
};

float4 main(PS_Input input) : SV_TARGET
{
	return input.f4Color;
}
