#pragma once

#include "DXUTcamera.h"
#include "getdata/getdata.h"
#include "tdxscene.h"
#include "utility/utility.h"
#include <array>        // for std::array
#include <cstdint>      // for std::int32_t
#include <memory>       // for std::shared_ptr, for std::unique_ptr
#include <string>       // for std::string
#include <d3dx9math.h>

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
#ifndef	SIMPLEVER2
struct SimpleVertex2
{
    D3DXVECTOR3 Pos;
    D3DXCOLOR	Col;
};
#define SIMPLEVER2
#endif



#define DEG2RAD( a ) ( a * D3DX_PI / 180.f )

class TDXHydrogenScene final :
	public TDXScene
{
    // #region �R���X�g���N�^�E�f�X�g���N�^

public:
    //! A constructor.
    /*!
        �B��̃R���X�g���N�^
    */
    TDXHydrogenScene(std::shared_ptr<getdata::GetData> const & pgd);

    //! A destructor.
    /*!
        �f�X�g���N�^
    */
    virtual ~TDXHydrogenScene(void);
	
    // #endregion �R���X�g���N�^�E�f�X�g���N�^

    // #region �����o�֐�
	
public:
    HRESULT Init(ID3D10Device* pd3dDevice) override;
    HRESULT OnFrameMove( double fTime, float fElapsedTime, void* pUserContext ) override;
    HRESULT OnRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext ) override;
    HRESULT OnResize( ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                    const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext ) override;
    HRESULT MsgPrc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) override;

    // ! A public member function.
    /*!
        �ĕ`�悷��
        \param pd3dDevice DirectX�f�o�C�X�ւ̃X�}�[�g�|�C���^
        \return �ĕ`�悪�����������ǂ���
    */
    HRESULT Redraw(ID3D10Device * pd3dDevice) override;

    // ! A private member function.
    /*!
        SimpleVertex2�Ƀf�[�^���l�߂�
        \param ver �Ώۂ�SimpleVertex2
    */
    void FillSimpleVertex2(SimpleVertex2 & ver);



    // #endregion �����o�֐�

    // #region �����o�ϐ�

private:
    // ! A private static member variable (constant).
    /*!
        �d�q�̃T���v���_
    */
    static std::size_t const N = 80000;

protected:
    // ! A protected member variable.
    /*!
        �G�t�F�N�g���V�F�[�_�v���O������ǂ܂���Ƃ���
    */
    
    std::unique_ptr<ID3D10Effect, utility::Safe_Release<ID3D10Effect>> g_pEffect;
    ID3D10InputLayout*                  g_pVertexLayout;
    ID3D10EffectTechnique*              g_pTechnique;
    ID3D10Buffer*                       g_pVertexBuffer;
    ID3D10ShaderResourceView*           g_pTextureRV;
    ID3D10EffectMatrixVariable*         g_pWorldVariable;
    ID3D10EffectMatrixVariable*         g_pViewVariable;
    ID3D10EffectMatrixVariable*         g_pProjectionVariable;
    ID3D10EffectVectorVariable*         g_pMeshColorVariable;
    ID3D10EffectVectorVariable*		    g_pLightDirVariable;
    ID3D10EffectShaderResourceVariable* g_pDiffuseVariable;
    D3DXMATRIX                          g_World;
    D3DXMATRIX                          g_View;
    D3DXMATRIX                          g_Projection;
    D3DXVECTOR4                         g_vMeshColor;
    D3DXVECTOR4							g_vLightDir;

    // ! A protected member variable.
    /*!
        A model viewing camera
    */
    CModelViewerCamera                  g_Camera;
    
private:
    // ! A private member variable (constant).
    /*!
        r�̃��b�V���Ɠd�q���x
    */
    std::shared_ptr<getdata::GetData> const pgd_;
};

// #region �񃁃��o�֐�

// ! A function.
/*!
    �}���`�o�C�g����������C�h������ɕϊ�����
    \param mbs �}���`�o�C�g������
    \param codeMulti �}���`�o�C�g������̃G���R�[�h�i�f�t�H���g=932�j
    \return ���C�h������
*/
std::wstring my_mbstowcs(std::string const & mbs, std::int32_t codeMulti = 932);

// ! A function.
/*!
    �t�@�C�����J��
    \return �t�@�C����
*/
std::string MyOpenFile();

// ! A function.
/*!
    ���C�h��������}���`�o�C�g������ɕϊ�����
    \param wcs ���C�h������
    \param codeMulti �}���`�o�C�g������̃G���R�[�h�i�f�t�H���g=932�j
    \return �}���`�o�C�g������
*/
std::string my_wcstombs(std::array<wchar_t, MAX_PATH> const & wcs, std::int32_t codeMulti = 932);

// ! A function.
/*!
    �t�@�C���I���_�C�A���O���J��
    \param hWnd �E�B���h�E�n���h��
    \param filepath �t�@�C���̃p�X
    \param filename �t�@�C����
    \param title �t�@�C���I���_�C�A���O�̃^�C�g��
    \param defextension �f�t�H���g�̃t�@�C���̎��
    \return �t�@�C���I���_�C�A���O�̖߂�l
*/
BOOL ShowFileDialog(HWND hWnd, wchar_t * filepath, wchar_t * filename, wchar_t * title, wchar_t * defextension);

// #endregion �񃁃��o�֐�