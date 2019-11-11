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
		output.f4Position = float4(-0.5f, -0.3f, 0.0f, 1.0f); // Lower left vertex
		output.f4Color = float4(0.0f, 0.7f, 0.0f, 1.0f); // Green
	}
	else if (id == 1)
	{
		output.f4Position = float4(0.5f, -0.3f, 0.0f, 1.0f); // lower right vertex
		output.f4Color = float4(0.0f, 0.7f, 0.0f, 1.0f); // Green
	}
	else if (id == 2)
	{
		output.f4Position = float4(0.0f, 0.5f, 0.0f, 1.0f); // top vertex
		output.f4Color = float4(0.0f, 0.7f, 0.0f, 1.0f); // Green
	}
	if (id == 3) // Triangle 2
	{
		output.f4Position = float4(-0.4f, 0.1f, 0.0f, 1.0f); // Lower left vertex
		output.f4Color = float4(0.0f, 0.7f, 0.0f, 1.0f); // Green
	}
	else if (id == 4)
	{
		output.f4Position = float4(0.4f, 0.1f, 0.0f, 1.0f); // lower right vertex
		output.f4Color = float4(0.0f, 0.7f, 0.0f, 1.0f); // Green
	}
	else if (id == 5)
	{
		output.f4Position = float4(0.0f, 0.8f, 0.0f, 1.0f); // top vertex
		output.f4Color = float4(0.0f, 0.7f, 0.0f, 1.0f); // Green
	}
	if (id == 6) // Triangle 3
	{
		output.f4Position = float4(-0.6f, -0.8f, 0.0f, 1.0f); // Lower left vertex
		output.f4Color = float4(0.0f, 0.7f, 0.0f, 1.0f); // Green
	}
	else if (id == 7)
	{
		output.f4Position = float4(0.6f, -0.8f, 0.0f, 1.0f); // lower right vertex
		output.f4Color = float4(0.0f, 0.7f, 0.0f, 1.0f); // Green
	}
	else if (id == 8)
	{
		output.f4Position = float4(0.0f, 0.3f, 0.0f, 1.0f); // top vertex
		output.f4Color = float4(0.0f, 0.7f, 0.0f, 1.0f); // Green
	}
	return output;
}
