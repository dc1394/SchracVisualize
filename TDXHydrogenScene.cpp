#include "DXUT.h"
#include "DXUTmisc.h"
#include "resource.h"
#include "myrandom/myrand.h"

#include "TDXHydrogenScene.h"

#define _USE_MATH_DEFINES
#include <math.h>


#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>

#define USE_CAMERA 1	//DXUT‚ÌCameraƒNƒ‰ƒX‚ðŽg‚¤‚©‚Ç‚¤‚©


TDXHydrogenScene::TDXHydrogenScene(void) :
g_pEffect(NULL),
g_pVertexLayout(NULL),
g_pTechnique(NULL),
g_pVertexBuffer(NULL),
g_pIndexBuffer(NULL),
g_pTextureRV(NULL),
g_pWorldVariable(NULL),
g_pViewVariable(NULL),
g_pProjectionVariable(NULL),
g_pMeshColorVariable(NULL),
g_pLightDirVariable(NULL),
g_vMeshColor( 0.7f, 0.7f, 0.7f, 1.0f ),
grmar_("rho_H_1s.csv")
{

}

TDXHydrogenScene::~TDXHydrogenScene(void)
{
	SAFE_RELEASE( g_pVertexBuffer );
    SAFE_RELEASE( g_pIndexBuffer );
    SAFE_RELEASE( g_pVertexLayout );
    SAFE_RELEASE( g_pTextureRV );
    SAFE_RELEASE( g_pEffect );
}




HRESULT TDXHydrogenScene::Init(ID3D10Device* pd3dDevice)
{
    HRESULT hr = S_OK;

    // Read the D3DX effect file
    DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3D10_SHADER_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3D10_SHADER_DEBUG;
    #endif
    hr = D3DX10CreateEffectFromFile( L"DxClassTest.fx", NULL, NULL, "fx_4_0", dwShaderFlags, 0, pd3dDevice, NULL,
                                         NULL, &g_pEffect, NULL, NULL );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be located.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        V_RETURN( hr );
    }

    g_pTechnique = g_pEffect->GetTechniqueByName( "Render2" );
    g_pWorldVariable = g_pEffect->GetVariableByName( "World" )->AsMatrix();
    g_pViewVariable = g_pEffect->GetVariableByName( "View" )->AsMatrix();
    g_pProjectionVariable = g_pEffect->GetVariableByName( "Projection" )->AsMatrix();
    g_pMeshColorVariable = g_pEffect->GetVariableByName( "vMeshColor" )->AsVector();
    g_pDiffuseVariable = g_pEffect->GetVariableByName( "txDiffuse" )->AsShaderResource();

    // Define the input layout
    D3D10_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = sizeof( layout ) / sizeof( layout[0] );

    // Create the input layout
    D3D10_PASS_DESC PassDesc;
    g_pTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
    V_RETURN( pd3dDevice->CreateInputLayout( layout, numElements, PassDesc.pIAInputSignature,
                                             PassDesc.IAInputSignatureSize, &g_pVertexLayout ) );

    // Set the input layout
    pd3dDevice->IASetInputLayout( g_pVertexLayout );

    // Create vertex buffer
    std::vector<SimpleVertex2> vertices(N);

	//std::ofstream	ofs("calc_log.txt");
	
    myrandom::MyRand mr(-5.0, 5.0);
    myrandom::MyRand mr2(0.0, grmar_.Rhomax + 0.1);

	auto const L = 0.3f;

	for( int i=0; i<N ; ++i)
	{
        double x, y, z, p, pp, a;
        
		do {
			x = mr.myrand();
            y = mr.myrand();
            z = mr.myrand();

            auto const rmin = grmar_.R_meshmin();
            if (std::fabs(x) < rmin || std::fabs(y) < rmin || std::fabs(z) < rmin) {
                continue;
            }

            p = mr2.myrand();

            auto const r = std::sqrt((x / L) * (x / L) + (y / L) * (y / L) + (z / L) * (z / L));
            pp = static_cast<float>(grmar_(r));
            //prob1s(x / L,y / L ,z / L);	//‚±‚±‚ðŒ©‚½‚¢”g“®ŠÖ”‚É•Ï‚¦‚é
			a = (pp > 0.0) - (pp < 0.0);
			//pp *= pp;
			//nn++;
			//sum += pp;
		} while (pp < p);

		vertices[i].Pos.x = static_cast<float>(x);
        vertices[i].Pos.y = static_cast<float>(y);
        vertices[i].Pos.z = static_cast<float>(z);

		vertices[i].Col.r = a > 0.0 ? 0.8f : 0.0f;
		vertices[i].Col.b = 0.8f;
		vertices[i].Col.g = a < 0.0 ? 0.8f : 0.0f;
		vertices[i].Col.a = 1.0f;
		//ofs <<  x << "," << y << "," << z <<  std::endl;
/*
		px = (prob2s(x + dx, y, z ) - pp) / dx / pp; 
		py = (prob2s(x, y + dx, z ) - pp) / dx / pp; 
		pz = (prob2s(x, y, z + dx ) - pp) / dx / pp; 
		ofs <<  px << "," << py << "," << pz << "," << (x*px+y*py+z*pz) /sqrt((x*x+y*y+z*z)*(px*px+py*py+pz*pz)) <<   std::endl;
*/
		//if ( maxpp < pp)
		//	maxpp = pp;
	}

	/*ofs << -1 << -1 << -1 << std::endl;
	ofs << "maxpp = " << maxpp << std::endl;
	ofs << "nn = " << nn << ", ave = " << sum / nn << std::endl;
	ofs.close();*/

    D3D10_BUFFER_DESC bd;
    bd.Usage = D3D10_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( SimpleVertex2 ) * N;
    bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = vertices.data();
    V_RETURN( pd3dDevice->CreateBuffer( &bd, &InitData, &g_pVertexBuffer ) );

    // Set vertex buffer
    UINT stride = sizeof( SimpleVertex2 );
    UINT offset = 0;
    pd3dDevice->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &stride, &offset );


    // Set primitive topology
    pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

    // Load the Texture
    hr = D3DX10CreateShaderResourceViewFromFile( pd3dDevice, L"seafloor.dds", NULL, NULL, &g_pTextureRV, NULL );

    // Initialize the world matrices
    D3DXMatrixIdentity( &g_World );

    // Initialize the view matrix
#ifdef	USE_CAMERA
    D3DXVECTOR3 Eye( 0.0f, 3.0f, -6.0f );
    D3DXVECTOR3 At( 0.0f, 0.0f, 0.0f );
//    D3DXVECTOR3 Eye( 0.0f, 0.0f, -800.0f );
//    D3DXVECTOR3 At( 0.0f, 0.0f, 0.0f );
    g_Camera.SetViewParams( &Eye, &At );
#else
    D3DXVECTOR3 Eye( 0.0f, 3.0f, -6.0f );
    D3DXVECTOR3 At( 0.0f, 0.0f, 0.0f );

	D3DXVECTOR3 Up( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &g_View, &Eye, &At, &Up );
    // Update Variables that never change
    g_pViewVariable->SetMatrix( ( float* )&g_View );
#endif


    return S_OK;
}


HRESULT TDXHydrogenScene::OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
    // Update the camera's position based on user input 

#ifdef	USE_CAMERA
	g_Camera.FrameMove( fElapsedTime );
#else
    // Rotate cube around the origin
    D3DXMatrixRotationY( &g_World, 60.0f * DEG2RAD((float)fTime) );
#endif

    // Modify the color
    g_vMeshColor.x = ( sinf( ( float )fTime * 1.0f ) + 1.0f ) * 0.5f;
    g_vMeshColor.y = ( cosf( ( float )fTime * 3.0f ) + 1.0f ) * 0.5f;
    g_vMeshColor.z = ( sinf( ( float )fTime * 5.0f ) + 1.0f ) * 0.5f;

	return S_OK;
}


HRESULT TDXHydrogenScene::OnRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    //
    // Clear the back buffer
    //
//    float ClearColor[4] = { 0.0f, 0.0625f, 0.15f, 1.0f }; // red, green, blue, alpha
    float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // red, green, blue, alpha
    ID3D10RenderTargetView* pRTV = DXUTGetD3D10RenderTargetView();
    pd3dDevice->ClearRenderTargetView( pRTV, ClearColor );

    //
    // Clear the depth stencil
    //
    ID3D10DepthStencilView* pDSV = DXUTGetD3D10DepthStencilView();
    pd3dDevice->ClearDepthStencilView( pDSV, D3D10_CLEAR_DEPTH, 1.0, 0 );

    //
    // Update variables that change once per frame
    //
#ifdef	USE_CAMERA
	const D3DXMATRIX	*mat = g_Camera.GetProjMatrix();
    g_pProjectionVariable->SetMatrix( ( float* )mat );
	mat = g_Camera.GetViewMatrix();
    g_pViewVariable->SetMatrix( ( float* )mat );
#endif

    g_pWorldVariable->SetMatrix( ( float* )&g_World );

    g_pMeshColorVariable->SetFloatVector( ( float* )g_vMeshColor );

    //
    // Render the cube
    //
    D3D10_TECHNIQUE_DESC techDesc;
    g_pTechnique->GetDesc( &techDesc );
    for( UINT p = 0; p < techDesc.Passes; ++p )
    {
        g_pTechnique->GetPassByIndex( p )->Apply( 0 );
        pd3dDevice->Draw(N,0 );
    }

	return S_OK;
}


HRESULT TDXHydrogenScene::OnResize( ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                          const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    // Setup the projection parameters again
    float fAspect = static_cast<float>( pBackBufferSurfaceDesc->Width ) / static_cast<float>( pBackBufferSurfaceDesc->Height );
#ifdef	USE_CAMERA
	g_Camera.SetProjParams( D3DX_PI / 4, fAspect, 0.1f, 100.0f );
    g_Camera.SetWindow( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );
    g_Camera.SetButtonMasks( MOUSE_MIDDLE_BUTTON, MOUSE_WHEEL, MOUSE_LEFT_BUTTON );
#else
    D3DXMatrixPerspectiveFovLH( &g_Projection, D3DX_PI * 0.25f, fAspect, 0.1f, 100.0f );
    g_pProjectionVariable->SetMatrix( ( float* )&g_Projection );
#endif
    return S_OK;
}

HRESULT TDXHydrogenScene::MsgPrc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{

#ifdef	USE_CAMERA
    return g_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );
#else
	return S_OK;
#endif
}


