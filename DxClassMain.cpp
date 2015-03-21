/*! \file DxClassMain.cpp
    \brief メインファイル

    Copyright ©  2015 @dc1394 All Rights Reserved.
    (but this is originally adapted by Microsoft Corporation for Basic introduction to DXUT)
    This software is released under the BSD-2 License.
*/

#include "DXUT.h"
#include "DXUTmisc.h"
#include "DXUTgui.h"
#include "DXUTsettingsDlg.h"
#include "SDKmisc.h"
#include "TDXScene.h"
#include "resource.h"
#include <malloc.h>
#include <string>       // for std::wstring, std::to_string

#define DEG2RAD( a ) ( a * D3DX_PI / 180.f )

//! A global variable.
/*!
    _aligned_mallocで確保されたメモリを解放するラムダ式
    \param ptr _aligned_malloc _aligned_mallocで確保されたメモリへの先頭ポインタ
*/
auto const aligned_deleter = [](TDXScene * ptr)
{
    ptr->~TDXScene();
    _aligned_free(ptr);
    ptr = nullptr;
};

D3DXVECTOR4                         g_vMeshColor( 0.7f, 0.7f, 0.7f, 1.0f );

CDXUTDialogResourceManager          g_DialogResourceManager;// manager for shared resources of dialogs
CD3DSettingsDlg                     g_D3DSettingsDlg;       // Device settings dialog
CDXUTDialog                         g_HUD;                  // manages the 3D UI

bool	ROT_FLAG = true;

//! A global variable.
/*!
    描画する軌道の識別数値
*/
auto drawdata = 1U;

//! A global variable.
/*!
Font for drawing text
*/
std::unique_ptr<ID3DX10Font, utility::Safe_Release<ID3DX10Font>> font;

//! A global variable.
/*!
    データオブジェクト
*/
std::shared_ptr<getdata::GetData> pgd;

//! A global variable.
/*!
    描画クラスのオブジェクトへのスマートポインタ
*/
std::unique_ptr<TDXScene, decltype(aligned_deleter)> scene;

//! A global variable.
/*!
    Sprite for batching text drawing
*/
std::unique_ptr<ID3DX10Sprite, utility::Safe_Release<ID3DX10Sprite>> sprite;

//! A global variable.
/*!
    再描画するかどうか
*/
auto redraw = false;

//! A global variable.
/*!
    実部と虚部のどちらを描画するか
*/
auto reim = TDXScene::Re_Im_type::REAL;


//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_CHANGEDEVICE        2
#define IDC_TOGGLEROTATION      3
#define IDC_READDATA            4
#define IDC_COMBOBOX            5
#define IDC_RADIOA              6
#define IDC_RADIOB              7

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D10DeviceAcceptable( UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType,
                                       DXGI_FORMAT BufferFormat, bool bWindowed, void* pUserContext );
HRESULT CALLBACK OnD3D10CreateDevice( ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBufferSurfaceDesc,
                                      void* pUserContext );
HRESULT CALLBACK OnD3D10ResizedSwapChain( ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                          const DXGI_SURFACE_DESC* pBufferSurfaceDesc, void* pUserContext );
void CALLBACK OnD3D10FrameRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
void CALLBACK OnD3D10ReleasingSwapChain( void* pUserContext );
void CALLBACK OnD3D10DestroyDevice( void* pUserContext );

LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext );
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );

//! A function.
/*!
    ウィンドウタイトルを生成する
    \return ウィンドウタイトル
*/
std::wstring CreateWindowTitle();

//! A function.
/*!
    テキストファイルからデータを読み込む
*/
void ReadData();

//! A function.
/*!
    UIを配置する
*/
void SetUI();

//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
    g_D3DSettingsDlg.Init( &g_DialogResourceManager );
    g_HUD.Init( &g_DialogResourceManager );

    g_HUD.SetCallback( OnGUIEvent ); 
    
    ReadData();
    SetUI();    
}



//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // DXUT will create and use the best device (either D3D9 or D3D10) 
    // that is available system depending on which D3D callbacks set below

    // Set DXUT callbacks
    DXUTSetCallbackD3D10DeviceAcceptable( IsD3D10DeviceAcceptable );
    DXUTSetCallbackD3D10DeviceCreated( OnD3D10CreateDevice );
    DXUTSetCallbackD3D10SwapChainResized( OnD3D10ResizedSwapChain );
    DXUTSetCallbackD3D10SwapChainReleasing( OnD3D10ReleasingSwapChain );
    DXUTSetCallbackD3D10DeviceDestroyed( OnD3D10DestroyDevice );
    DXUTSetCallbackD3D10FrameRender( OnD3D10FrameRender );

    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackKeyboard( OnKeyboard );
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );

    DXUTInit( true, true, NULL ); // Parse the command line, show msgboxes on error, no extra command line params
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen

	InitApp();



    DXUTCreateWindow(CreateWindowTitle().c_str());
    DXUTCreateDevice( true, 640, 480 );

	DXUTDeviceSettings ds;
	ds = DXUTGetDeviceSettings();

    DXUTMainLoop(); // Enter into the DXUT render loop

    return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Reject any D3D10 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D10DeviceAcceptable( UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType,
                                       DXGI_FORMAT BufferFormat, bool bWindowed, void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D10 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D10CreateDevice( ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBufferSurfaceDesc,
                                      void* pUserContext )
{
	HRESULT hr;

    V_RETURN( g_DialogResourceManager.OnD3D10CreateDevice( pd3dDevice ) );
    V_RETURN( g_D3DSettingsDlg.OnD3D10CreateDevice( pd3dDevice ) );

    ID3DX10Font * fonttemp;
    V_RETURN( D3DX10CreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
                                OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                                L"Arial", &fonttemp ) );
    font.reset(fonttemp);

    ID3DX10Sprite * spritetmp;
    V_RETURN( D3DX10CreateSprite( pd3dDevice, 512, &spritetmp ) );
    sprite.reset(spritetmp);
    //g_pTxtHelper = new CDXUTTextHelper( NULL, NULL, g_pFont, g_pSprite, 15 );
    
    auto buf = _aligned_malloc(sizeof(TDXScene), 16);
    scene.reset(new(buf) TDXScene(pgd));
	return scene->Init(pd3dDevice);
}


//--------------------------------------------------------------------------------------
// Create any D3D10 resources that depend on the back buffer
// Create and set the depth stencil texture if needed
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D10ResizedSwapChain( ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                          const DXGI_SURFACE_DESC* pBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

    V_RETURN( g_DialogResourceManager.OnD3D10ResizedSwapChain( pd3dDevice, pBufferSurfaceDesc ) );
    V_RETURN( g_D3DSettingsDlg.OnD3D10ResizedSwapChain( pd3dDevice, pBufferSurfaceDesc ) );

    g_HUD.SetLocation( pBufferSurfaceDesc->Width - 170, 0 );
    g_HUD.SetSize( 170, 170 );


	return scene->OnResize(pd3dDevice, pSwapChain, pBufferSurfaceDesc, pUserContext);
}

//--------------------------------------------------------------------------------------
// Render the help and statistics text
//--------------------------------------------------------------------------------------
void RenderText( double fTime)
{
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D10 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10FrameRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	if( g_D3DSettingsDlg.IsActive() )
    {
	    float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
		ID3D10RenderTargetView* pRTV = DXUTGetD3D10RenderTargetView();
		pd3dDevice->ClearRenderTargetView( pRTV, ClearColor );

        g_D3DSettingsDlg.OnRender( fElapsedTime );
        return;
    }
	else
	{
        if (redraw) {
            auto const index = drawdata & 0x0F;
            switch (pgd->L) {
            case 0:
                scene->Redraw(0, pd3dDevice, reim);
                break;

            case 1:
            {
                switch (index) {
                case 1:
                    scene->Redraw(1, pd3dDevice, reim);
                    break;

                case 2:
                    scene->Redraw(-1, pd3dDevice, reim);
                    break;

                case 3:
                    scene->Redraw(0, pd3dDevice, reim);
                    break;

                default:
                    BOOST_ASSERT(!"indexの指定がおかしい！");
                    break;
                }
            }
            break;

            case 2:
            {
                switch (index) {
                case 1:
                    scene->Redraw(-2, pd3dDevice, reim);
                    break;

                case 2:
                    scene->Redraw(-1, pd3dDevice, reim);
                    break;

                case 3:
                    scene->Redraw(1, pd3dDevice, reim);
                    break;

                case 4:
                    scene->Redraw(2, pd3dDevice, reim);
                    break;

                case 5:
                    scene->Redraw(0, pd3dDevice, reim);
                    break;
                    
                default:
                    BOOST_ASSERT(!"indexの指定がおかしい！");
                    break;
                }
            }
            break;

            case 3:
            {
                switch (index) {
                case 1:
                    scene->Redraw(1, pd3dDevice, reim);
                    break;

                case 2:
                    scene->Redraw(-1, pd3dDevice, reim);
                    break;

                case 3:
                    scene->Redraw(2, pd3dDevice, reim);
                    break;

                case 4:
                    scene->Redraw(-2, pd3dDevice, reim);
                    break;

                case 5:
                    scene->Redraw(3, pd3dDevice, reim);
                    break;

                case 6:
                    scene->Redraw(-3, pd3dDevice, reim);
                    break;

                case 7:
                    scene->Redraw(0, pd3dDevice, reim);
                    break;

                default:
                    BOOST_ASSERT(!"indexの指定がおかしい！");
                    break;
                }
            }
            break;

            default:
                BOOST_ASSERT(!"量子数の指定が異常です！");
                break;
            }
            
            redraw = false;
        }

        scene->OnRender( pd3dDevice, fTime, fElapsedTime, pUserContext );
	    g_HUD.OnRender( fElapsedTime );
	}
}



//--------------------------------------------------------------------------------------
// Release D3D10 resources created in OnD3D10ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10ReleasingSwapChain( void* pUserContext )
{
    g_DialogResourceManager.OnD3D10ReleasingSwapChain();
}


//--------------------------------------------------------------------------------------
// Release D3D10 resources created in OnD3D10CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10DestroyDevice( void* pUserContext )
{
    g_DialogResourceManager.OnD3D10DestroyDevice();
    g_D3DSettingsDlg.OnD3D10DestroyDevice();
    DXUTGetGlobalResourceCache().OnDestroyDevice();

    scene.reset();
}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D9 or D3D10 device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	if(ROT_FLAG)
		scene->OnFrameMove(fTime, fElapsedTime, pUserContext );
}


//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext )
{
    // Pass messages to dialog resource manager calls so GUI state is updated correctly
    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Pass messages to settings dialog if its active
    if( g_D3DSettingsDlg.IsActive() )
    {
        g_D3DSettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }

    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

	if(scene != NULL)
		scene->MsgPrc( hWnd, uMsg, wParam, lParam );

    return 0;
}


//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
    if( bKeyDown )
    {
        switch( nChar )
        {
            case VK_F1: // Change as needed                
                break;
        }
    }
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
    switch (nControlID )
    {
        case IDC_TOGGLEFULLSCREEN:
            DXUTToggleFullScreen();
            break;

        case IDC_CHANGEDEVICE:
            g_D3DSettingsDlg.SetActive( !g_D3DSettingsDlg.IsActive() );
            break;

		case IDC_TOGGLEROTATION:
			ROT_FLAG = !ROT_FLAG;
            break;

        case IDC_READDATA:
            ReadData();
            SetUI();
            scene->Pgd = pgd;
            redraw = true;
            ::SetWindowText(DXUTGetHWND(), CreateWindowTitle().c_str());
            break;

        case IDC_COMBOBOX:
        {
            auto const pItem = (static_cast<CDXUTComboBox *>(pControl))->GetSelectedItem();
            if (pItem)
            {
                drawdata = reinterpret_cast<std::uint32_t>(pItem->pData);
                redraw = true;
            }
            break;
        }

        case IDC_RADIOA:
            reim = TDXScene::Re_Im_type::REAL;
            redraw = true;
            break;

        case IDC_RADIOB:
            reim = TDXScene::Re_Im_type::IMAGINARY;
            redraw = true;
            break;

        default:
            break;
    }
}


std::wstring CreateWindowTitle()
{
    std::string windowtitle;
    switch (pgd->Rho_wf_type_) {
    case getdata::GetData::Rho_Wf_type::RHO:
        windowtitle = "Electron density";
        break;

    case getdata::GetData::Rho_Wf_type::WF:
        windowtitle = "Wavefunction";
        break;
    }
    windowtitle += " in " + pgd->Atomname() + " for " + pgd->Orbital() + " orbital";

    return utility::my_mbstowcs(windowtitle);
}


void ReadData()
{
    while (true) {
        try {
            pgd.reset();
            pgd = std::make_shared<getdata::GetData>(utility::myOpenFile());
        }
        catch (std::runtime_error const & e) {
            ::MessageBox(nullptr, utility::my_mbstowcs(e.what()).c_str(), L"エラー", MB_OK | MB_ICONWARNING);
            continue;
        }
        break;
    }
}


void SetUI()
{
    g_HUD.RemoveAllControls();

    std::int32_t iY = 10;

    g_HUD.AddButton(IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22);
    g_HUD.AddButton(IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22, VK_F2);
    g_HUD.AddButton(IDC_TOGGLEROTATION, L"Toggle Rotaion Animation", 35, iY += 24, 125, 22);

    g_HUD.AddButton(IDC_READDATA, L"新規ファイル読み込み", 35, iY += 34, 125, 22);

    // Combobox
    CDXUTComboBox* pCombo;
    g_HUD.AddComboBox(IDC_COMBOBOX, 35, iY += 34, 125, 22, L'O', false, &pCombo);
    if (pCombo)
    {
        pCombo->SetDropHeight(100);
        pCombo->RemoveAllItems();
        BOOST_ASSERT(pgd->N > static_cast<std::int32_t>(pgd->L));
        auto orbital(std::to_wstring(pgd->N));
        switch (pgd->L) {
        case 0:
        {
            orbital += L's';
            pCombo->AddItem(orbital.c_str(), reinterpret_cast<LPVOID>(0x11111111));
        }
        break;

        case 1:
        {
            pCombo->AddItem((orbital + L"px").c_str(), reinterpret_cast<LPVOID>(0x11111111));
            pCombo->AddItem((orbital + L"py").c_str(), reinterpret_cast<LPVOID>(0x12121212));
            pCombo->AddItem((orbital + L"pz").c_str(), reinterpret_cast<LPVOID>(0x13131313));
        }
        break;

        case 2:
        {
            pCombo->AddItem((orbital + L"dxy").c_str(), reinterpret_cast<LPVOID>(0x11111111));
            pCombo->AddItem((orbital + L"dyz").c_str(), reinterpret_cast<LPVOID>(0x12121212));
            pCombo->AddItem((orbital + L"dzx").c_str(), reinterpret_cast<LPVOID>(0x13131313));
            pCombo->AddItem((orbital + L"dx^2-y^2").c_str(), reinterpret_cast<LPVOID>(0x14141414));
            pCombo->AddItem((orbital + L"dz^2").c_str(), reinterpret_cast<LPVOID>(0x15151515));
        }
        break;

        case 3:
        {
            pCombo->AddItem((orbital + L"fxz^2").c_str(), reinterpret_cast<LPVOID>(0x11111111));
            pCombo->AddItem((orbital + L"fyz^2").c_str(), reinterpret_cast<LPVOID>(0x12121212));
            pCombo->AddItem((orbital + L"fz(x^2-y^2)").c_str(), reinterpret_cast<LPVOID>(0x13131313));
            pCombo->AddItem((orbital + L"fxyz").c_str(), reinterpret_cast<LPVOID>(0x14141414));
            pCombo->AddItem((orbital + L"fx(x^2-3y^2)").c_str(), reinterpret_cast<LPVOID>(0x15151515));
            pCombo->AddItem((orbital + L"fy(3x^2-y^2)").c_str(), reinterpret_cast<LPVOID>(0x16161616));
            pCombo->AddItem((orbital + L"fz^2").c_str(), reinterpret_cast<LPVOID>(0x17171717));
        }
        break;

        default:
            throw std::runtime_error("g以上の軌道には対応していません");
            break;
        }

        if (pgd->Rho_wf_type_ == getdata::GetData::Rho_Wf_type::WF) {
            // Radio buttons
            g_HUD.AddRadioButton(IDC_RADIOA, 1, L"実部", 35, iY += 34, 125, 22, true, L'1');
            g_HUD.AddRadioButton(IDC_RADIOB, 1, L"虚部", 35, iY += 28, 125, 22, false, L'2');
        }
    }
}

