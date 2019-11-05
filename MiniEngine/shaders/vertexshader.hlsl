struct VS_Output
{
	float4 f4Position : SV_Position;
	float4 f4Color : ASFD;
};

VS_Output main(uint id : SV_VertexID)
{
	VS_Output output;
	if (id == 0)
	{
		output.f4Position = float4(-0.5f, -0.5f, 0.0f, 1.0f); // Lower left vertex
		output.f4Position = float4(1.0f, 0.7f, 0.7f, 1.0f); // Rose
	}
	else if (id == 1)
	{
		output.f4Position = float4(0.5f, -0.5f, 0.0f, 1.0f); // lower right vertex
		output.f4Position = float4(1.0f, 0.7f, 0.7f, 1.0f); // Rose
	}
	else if (id == 2)
	{
		output.f4Position = float4(0.0f, 0.5f, 0.0f, 1.0f); // top vertex
		output.f4Position = float4(1.0f, 0.7f, 0.7f, 1.0f); // Rose
	}

	return output;
}
