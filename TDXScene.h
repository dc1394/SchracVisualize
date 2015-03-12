#pragma once

class TDXScene
{
public:
	TDXScene(void);
	virtual ~TDXScene(void);
  virtual HRESULT Init(ID3D10Device* pd3dDevice) = 0;
  virtual HRESULT OnFrameMove( double fTime, float fElapsedTime, void* pUserContext ) = 0;
  virtual HRESULT OnRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext ) = 0;
  virtual HRESULT OnResize( ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                           const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext ) = 0;
  virtual HRESULT MsgPrc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
  virtual HRESULT Redraw(ID3D10Device * pd3dDevice) = 0;
protected:
//  ID3D10Device* pd3dDevice;  // 対象となるデバイスへのポインタを持つべきか？
                             //対象となるデバイスは呼び出しの間に変わるかもしれない（画面モードの変更などの理由によって）
                 //よって、これは各呼び出しによって引数で渡されるべき

};
