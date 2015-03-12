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
//  ID3D10Device* pd3dDevice;  // �ΏۂƂȂ�f�o�C�X�ւ̃|�C���^�����ׂ����H
                             //�ΏۂƂȂ�f�o�C�X�͌Ăяo���̊Ԃɕς�邩������Ȃ��i��ʃ��[�h�̕ύX�Ȃǂ̗��R�ɂ���āj
                 //����āA����͊e�Ăяo���ɂ���Ĉ����œn�����ׂ�

};
