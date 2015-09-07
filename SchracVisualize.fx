//--------------------------------------------------------------------------------------
// File: SchracVisualize.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse;
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

cbuffer cbNeverChanges
{
    matrix View;
};

cbuffer cbChangeOnResize
{
    matrix Projection;
};

cbuffer cbChangesEveryFrame
{
    matrix World;
    float4 vMeshColor;
};

struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

struct VS_INPUT2
{
    float4 Pos : POSITION;
    float4 Col : COLOR0;
};

struct PS_INPUT2
{
    float4 Pos : SV_POSITION;
    float4 Col : COLOR0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( input.Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Tex = input.Tex;
    
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input) : SV_Target
{
    return txDiffuse.Sample( samLinear, input.Tex ) * vMeshColor;
}


//--------------------------------------------------------------------------------------
// Vertex Shader for PixelStream
//--------------------------------------------------------------------------------------
PS_INPUT2 VS2( VS_INPUT2 input )
{
    PS_INPUT2 output = (PS_INPUT2)0;
    output.Pos = mul( input.Pos, World );
    float zz = 1  - output.Pos.z * 0.0625;
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Col = input.Col * zz;
    
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader for PixelStream
//--------------------------------------------------------------------------------------
float4 PS2( PS_INPUT2 input) : SV_Target
{
    return input.Col;
}



//--------------------------------------------------------------------------------------
technique10 Render
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

technique10 Render2
{
    pass P1
    {
        SetVertexShader( CompileShader( vs_4_0, VS2() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS2() ) );
    }
}

