/*! \file SchracVisualizeMain.cpp
    \brief メインファイル

    Copyright ©  2015 @dc1394 All Rights Reserved.
    (but this is originally adapted by Microsoft Corporation for Basic introduction to DXUT)
    This software is released under the BSD 2-Clause License.
*/

#include "DXUT.h"
#include "DXUTmisc.h"
#include "DXUTgui.h"
#include "DXUTsettingsDlg.h"
#include "SDKmisc.h"
#include "TDXScene.h"
#include "resource.h"
#include <string>                       // for std::wstring, std::to_string
#include <malloc.h>                     // for _aligned_malloc, _aligned_free
#include <boost/format.hpp>             // for boost::wformat
#include <tbb/task_scheduler_init.h>    // for tbb::task_scheduler_init

using namespace tdxscene;

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

D3DXVECTOR4                         g_vMeshColor(0.7f, 0.7f, 0.7f, 1.0f);

CDXUTDialogResourceManager          g_DialogResourceManager;    // manager for shared resources of dialogs
CD3DSettingsDlg                     g_D3DSettingsDlg;           // Device settings dialog
CDXUTDialog                         g_HUD;                      // manages the 3D UI

bool	ROT_FLAG = true;

//! A global variable (constant).
/*!
    画面サイズ（高さ）
*/
static auto const WINDOWHEIGHT = 960;

//! A global variable (constant).
/*!
    画面サイズ（幅）
*/
static auto const WINDOWWIDTH = 1280;

//! A global variable.
/*!
    CPUのスレッド数
*/
auto const cputhread = tbb::task_scheduler_init::default_num_threads();

//! A global variable.
/*!
    描画する軌道の識別数値
*/
auto drawdata = 1U;

//! A global variable.
/*!
    計算開始時間
*/
double drawstarttime;

//! A global variable.
/*!
    計算終了時間
*/
double drawendtime;

//! A global variable.
/*!
    計算が終了したことを示すフラグ
*/
bool end;

//! A global variable.
/*!
    計算が開始したことを示すフラグ
*/
auto first = true;

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
    テキスト表示用
*/
std::unique_ptr<CDXUTTextHelper, utility::Safe_Delete<CDXUTTextHelper>> txthelper;

//! A global variable.
/*!
    再描画するかどうか
*/
auto redraw = true;

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
#define IDC_REDRAW              4
#define IDC_READDATA            5
#define IDC_COMBOBOX            6
#define IDC_RADIOA              7
#define IDC_RADIOB              8
#define IDC_OUTPUT              9
#define IDC_SLIDER				10

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D10DeviceAcceptable(UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType,
    DXGI_FORMAT BufferFormat, bool bWindowed, void* pUserContext);
HRESULT CALLBACK OnD3D10CreateDevice(ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBufferSurfaceDesc,
    void* pUserContext);
HRESULT CALLBACK OnD3D10ResizedSwapChain(ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
    const DXGI_SURFACE_DESC* pBufferSurfaceDesc, void* pUserContext);
void CALLBACK OnD3D10FrameRender(ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext);
void CALLBACK OnD3D10ReleasingSwapChain(void* pUserContext);
void CALLBACK OnD3D10DestroyDevice(void* pUserContext);

LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
    void* pUserContext);
void CALLBACK OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext);
void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext);
bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext);
void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext);

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
    再描画フラグをtrueにする
*/
void RedrawFlagTrue();

//! A function.
/*!
    画面の左上に情報を表示する
*/
void RenderText();

//! A function.
/*!
    UIを配置する
*/
void SetUI();

//! A function.
/*!
    描画を中止する
*/
void StopDraw();

//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
    g_D3DSettingsDlg.Init(&g_DialogResourceManager);
    g_HUD.Init(&g_DialogResourceManager);

    g_HUD.SetCallback(OnGUIEvent);

    ReadData();
    SetUI();
}


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    // DXUT will create and use the best device (either D3D9 or D3D10) 
    // that is available system depending on which D3D callbacks set below

    // Set DXUT callbacks
    DXUTSetCallbackD3D10DeviceAcceptable(IsD3D10DeviceAcceptable);
    DXUTSetCallbackD3D10DeviceCreated(OnD3D10CreateDevice);
    DXUTSetCallbackD3D10SwapChainResized(OnD3D10ResizedSwapChain);
    DXUTSetCallbackD3D10SwapChainReleasing(OnD3D10ReleasingSwapChain);
    DXUTSetCallbackD3D10DeviceDestroyed(OnD3D10DestroyDevice);
    DXUTSetCallbackD3D10FrameRender(OnD3D10FrameRender);

    DXUTSetCallbackMsgProc(MsgProc);
    DXUTSetCallbackKeyboard(OnKeyboard);
    DXUTSetCallbackFrameMove(OnFrameMove);
    DXUTSetCallbackDeviceChanging(ModifyDeviceSettings);

    DXUTInit(true, true, nullptr);      // Parse the command line, show msgboxes on error, no extra command line params
    DXUTSetCursorSettings(true, true);  // Show the cursor and clip it when in full screen

    InitApp();
    
    auto const dispx = ::GetSystemMetrics(SM_CXSCREEN);
    auto const dispy = ::GetSystemMetrics(SM_CYSCREEN);
    auto const xpos = (dispx - WINDOWWIDTH) >> 1;
    auto const ypos = (dispy - WINDOWHEIGHT) >> 1;
    DXUTCreateWindow(CreateWindowTitle().c_str(), nullptr, nullptr, nullptr, xpos, ypos);
    DXUTCreateDevice(true, WINDOWWIDTH, WINDOWHEIGHT);
    
    auto ds = DXUTGetDeviceSettings();
    ds.d3d10.SyncInterval = 0;
    DXUTCreateDeviceFromSettings(&ds);

    DXUTMainLoop();                     // Enter into the DXUT render loop

    return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Reject any D3D10 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D10DeviceAcceptable(UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType,
    DXGI_FORMAT BufferFormat, bool bWindowed, void* pUserContext)
{
    return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D10 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D10CreateDevice(ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBufferSurfaceDesc,
    void* pUserContext)
{
    HRESULT hr;

    V_RETURN(g_DialogResourceManager.OnD3D10CreateDevice(pd3dDevice));
    V_RETURN(g_D3DSettingsDlg.OnD3D10CreateDevice(pd3dDevice));
    
    ID3DX10Font * fonttemp;
    V_RETURN(D3DX10CreateFont(pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Arial", &fonttemp));
    font.reset(fonttemp);

    ID3DX10Sprite * spritetmp;
    V_RETURN(D3DX10CreateSprite(pd3dDevice, 512, &spritetmp));
    sprite.reset(spritetmp);
    txthelper.reset(new CDXUTTextHelper( nullptr, nullptr, font.get(), sprite.get(), 15 ));

    auto buf = _aligned_malloc(sizeof(TDXScene), 16);
    scene.reset(new(buf)TDXScene(pgd));
    return scene->Init(pd3dDevice);
}


//--------------------------------------------------------------------------------------
// Create any D3D10 resources that depend on the back buffer
// Create and set the depth stencil texture if needed
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D10ResizedSwapChain(ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
    const DXGI_SURFACE_DESC* pBufferSurfaceDesc, void* pUserContext)
{
    HRESULT hr;

    V_RETURN(g_DialogResourceManager.OnD3D10ResizedSwapChain(pd3dDevice, pBufferSurfaceDesc));
    V_RETURN(g_D3DSettingsDlg.OnD3D10ResizedSwapChain(pd3dDevice, pBufferSurfaceDesc));

    g_HUD.SetLocation(pBufferSurfaceDesc->Width - 170, 0);
    g_HUD.SetSize(170, 170);
    
    return scene->OnResize(pd3dDevice, pSwapChain, pBufferSurfaceDesc, pUserContext);
}

//--------------------------------------------------------------------------------------
// Render the help and statistics text
//--------------------------------------------------------------------------------------
void RenderText(ID3D10Device* pd3dDevice, double fTime)
{
    if (!scene->Complete && first) {
        drawstarttime = fTime;
        end = false;
        first = false;
    }
    else if (scene->Complete && !end) {
        drawendtime = fTime - drawstarttime;
        end = true;
    }

    std::wstring str;
    if (end) {
        str = (boost::wformat(L"計算時間 = %.3f秒") % drawendtime).str();
    }
    else {
        str = (boost::wformat(L"計算時間 = %.3f秒") % (fTime - drawstarttime)).str();
    }
    
    txthelper->Begin();
    txthelper->SetInsertionPos(2, 0);
    txthelper->SetForegroundColor(D3DXCOLOR(1.000f, 0.945f, 0.059f, 1.000f));
    txthelper->DrawTextLine(DXUTGetFrameStats(DXUTIsVsyncEnabled()));
    txthelper->DrawTextLine(DXUTGetDeviceStats());
    txthelper->DrawTextLine((boost::wformat(L"CPUスレッド数: %d") % cputhread).str().c_str());
    txthelper->DrawTextLine((boost::wformat(L"頂点数 = %d") % scene->Vertexsize()).str().c_str());
    txthelper->DrawTextLine(str.c_str());
    txthelper->End();
    pd3dDevice->IASetInputLayout(scene->PLayout().get());
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D10 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10FrameRender(ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext)
{
    if (g_D3DSettingsDlg.IsActive())
    {
        float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
        auto pRTV = DXUTGetD3D10RenderTargetView();
        pd3dDevice->ClearRenderTargetView(pRTV, ClearColor);

        g_D3DSettingsDlg.OnRender(fElapsedTime);
        return;
    }
    else
    {
        auto const index = drawdata & 0x0F;
        switch (pgd->L) {
        case 0:
            scene->RedrawFunc(0, pd3dDevice, reim);
            break;

        case 1:
        {
            switch (index) {
            case 1:
                scene->RedrawFunc(1, pd3dDevice, reim);
                break;

            case 2:
                scene->RedrawFunc(-1, pd3dDevice, reim);
                break;

            case 3:
                scene->RedrawFunc(0, pd3dDevice, reim);
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
                scene->RedrawFunc(-2, pd3dDevice, reim);
                break;

            case 2:
                scene->RedrawFunc(-1, pd3dDevice, reim);
                break;

            case 3:
                scene->RedrawFunc(1, pd3dDevice, reim);
                break;

            case 4:
                scene->RedrawFunc(2, pd3dDevice, reim);
                break;

            case 5:
                scene->RedrawFunc(0, pd3dDevice, reim);
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
                scene->RedrawFunc(1, pd3dDevice, reim);
                break;

            case 2:
                scene->RedrawFunc(-1, pd3dDevice, reim);
                break;

            case 3:
                scene->RedrawFunc(2, pd3dDevice, reim);
                break;

            case 4:
                scene->RedrawFunc(-2, pd3dDevice, reim);
                break;

            case 5:
                scene->RedrawFunc(3, pd3dDevice, reim);
                break;

            case 6:
                scene->RedrawFunc(-3, pd3dDevice, reim);
                break;

            case 7:
                scene->RedrawFunc(0, pd3dDevice, reim);
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

        scene->OnRender(pd3dDevice, fTime, fElapsedTime, pUserContext);
        g_HUD.OnRender(fElapsedTime);
        RenderText(pd3dDevice, fTime);
    }
}



//--------------------------------------------------------------------------------------
// Release D3D10 resources created in OnD3D10ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10ReleasingSwapChain(void* pUserContext)
{
    g_DialogResourceManager.OnD3D10ReleasingSwapChain();
}


//--------------------------------------------------------------------------------------
// Release D3D10 resources created in OnD3D10CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10DestroyDevice(void* pUserContext)
{
    scene->Thread_end = true;
    if (scene->Pth()->joinable()) {
        scene->Pth()->detach();
    }

    g_DialogResourceManager.OnD3D10DestroyDevice();
    g_D3DSettingsDlg.OnD3D10DestroyDevice();
    DXUTGetGlobalResourceCache().OnDestroyDevice();

    font.reset();
    sprite.reset();
    txthelper.reset();
    scene.reset();
}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D9 or D3D10 device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext)
{
    return true;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
    if (ROT_FLAG)
        scene->OnFrameMove(fTime, fElapsedTime, pUserContext);
}


//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
    void* pUserContext)
{
    // Pass messages to dialog resource manager calls so GUI state is updated correctly
    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc(hWnd, uMsg, wParam, lParam);
    if (*pbNoFurtherProcessing)
        return 0;

    // Pass messages to settings dialog if its active
    if (g_D3DSettingsDlg.IsActive())
    {
        g_D3DSettingsDlg.MsgProc(hWnd, uMsg, wParam, lParam);
        return 0;
    }

    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_HUD.MsgProc(hWnd, uMsg, wParam, lParam);
    if (*pbNoFurtherProcessing)
        return 0;
    
    if (scene != nullptr)
        scene->MsgPrc(hWnd, uMsg, wParam, lParam);

    return 0;
}


//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext)
{
    if (bKeyDown)
    {
        switch (nChar)
        {
        case VK_F1: // Change as needed                
            break;
        }
    }
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext)
{
    switch (nControlID)
    {
    case IDC_TOGGLEFULLSCREEN:
        DXUTToggleFullScreen();
        break;

    case IDC_CHANGEDEVICE:
        g_D3DSettingsDlg.SetActive(!g_D3DSettingsDlg.IsActive());
        break;

    case IDC_TOGGLEROTATION:
        ROT_FLAG = !ROT_FLAG;
        break;

    case IDC_REDRAW:
        RedrawFlagTrue();
        break;

    case IDC_READDATA:
        StopDraw();
        ReadData();
        SetUI();
        scene->Pgd = pgd;
        scene->Thread_end = false;
        scene->Redraw = true;
        first = true;
        ::SetWindowText(DXUTGetHWND(), CreateWindowTitle().c_str());
        break;

    case IDC_COMBOBOX:
    {
        auto const pItem = (static_cast<CDXUTComboBox *>(pControl))->GetSelectedItem();
        if (pItem)
        {
            drawdata = reinterpret_cast<std::uint32_t>(pItem->pData);
            RedrawFlagTrue();
        }
        break;
    }

    case IDC_RADIOA:
        reim = TDXScene::Re_Im_type::REAL;
        RedrawFlagTrue();
        break;

    case IDC_RADIOB:
        reim = TDXScene::Re_Im_type::IMAGINARY;
        RedrawFlagTrue();
        break;

    case IDC_SLIDER:
        scene->Vertexsize(static_cast<std::vector<TDXScene::SimpleVertex2>::size_type>((reinterpret_cast<CDXUTSlider*>(pControl))->GetValue()));
        RedrawFlagTrue();
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


void RedrawFlagTrue()
{
    StopDraw();
    scene->Thread_end = false;
    scene->Redraw = true;
    first = true;
}


void SetUI()
{
    g_HUD.RemoveAllControls();

    std::int32_t iY = 10;

    g_HUD.AddButton(IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22);
    g_HUD.AddButton(IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22, VK_F2);
    g_HUD.AddButton(IDC_TOGGLEROTATION, L"Toggle Rotaion Animation", 35, iY += 24, 125, 22);

    g_HUD.AddButton(IDC_REDRAW, L"再描画", 35, iY += 34, 125, 22);
    g_HUD.AddButton(IDC_READDATA, L"新規ファイル読み込み", 35, iY += 24, 125, 22);

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

    // 角度の調整
    g_HUD.AddStatic(IDC_OUTPUT, L"頂点数", 20, iY += 34, 125, 22);
    g_HUD.GetStatic(IDC_OUTPUT)->SetTextColor(D3DCOLOR_ARGB(255, 255, 255, 255));
    g_HUD.AddSlider(IDC_SLIDER, 35, iY += 24, 125, 22, 0, 1000000, TDXScene::VERTEXSIZE_FIRST);
}


void StopDraw()
{
    scene->Thread_end = true;
    if (scene->Pth()->joinable()) {
        scene->Pth()->join();
    }
}
