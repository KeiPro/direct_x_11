#pragma once
#include <string>
class Game
{
public:
	Game();
	~Game();

public:
	void Init(HWND hwnd);
	void Update();
	void Render();

private:
	void RenderBigin();
	void RenderEnd();

private:
	void CreateDeviceAndSwapChain();
	void CreateRenderTargetView();
	void SetViewport();

private:
	// ��� �ﰢ���� ������ �� �����͸� ������ִ� �Լ�.
	void CreateGeometry();
	void CreateInputLayout(); // ���ؽ��� �����ϴ� �Լ�
	void CreateVS();
	void CreatePS();

	void CreateRasterizerState();
	void CreateSamplerState();
	void CreateBlendState();
	void CreateSRV(); // Shader Resource View

	void CreateConstantBuffer();

	void LoadShaderFromFile(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob);
	

private:
	HWND _hwnd;
	uint32 _width = 0;
	uint32 _height = 0;

private:

	// Device & SwapChain
	// 
	// ID3D11Device* _device;
	// ID3D11DeviceContext* _deviceContext;
	// ���� ID3D11Device�� ����Ʈ �����͸� ����ϱ� ���ؼ� ComPtr�� �����ִ� ����� �Ʒ�ó�� ���.

	// ComPtr�� �ϴ� ��?
	// �⺻�����δ� AddRef()�� Release()�� �ڵ����� ���ִ� ������ ������, �� �߿��� ���� �Ʒ��� �����̴�.
	// 
	// ���� ������ ������ GPU�� �����ϴµ� ��� GPU�� ������ ������ ���ذ��� ���ߴ� �۾���
	// �츮�� ���� �ϴ� ���� �Ұ����ϴ�.
	// ����, Com�������̽���� ���� �ٽ��� ����ũ�μ���Ʈ���� ���� DirectX ���̺귯���� 
	// Ư�� Ŭ�����鸸 �츮�� �۾��� ���ָ�, GPU��ǰ���� �˾Ƽ� �ϵ���������� ������ �Ǵ� �������̽��̴�.
	ComPtr<ID3D11Device> _device = nullptr;
	ComPtr<ID3D11DeviceContext> _deviceContext = nullptr; // ���ҽ��� ������ ������������ �����ִ� ����.

	// DXGI��?
	// DXGI�� ������ �ϴµ��� �־� �ϵ��� ȿ�������� ���� �� �ֵ��� ���� ������ �۾����� �������ش�.
	// ����, �������� �������� �׷��� ��ɼ��� ó���ϴµ�, �̸��׸� ȭ�� ��� ��ȯ, ���÷��� ����ͳ� �����,
	// �����Ǵ� ���÷��� ���(�ػ�, �ֻ��� ��) ���� �׷��� �ý��� ������ ���� ���� ����� �������ش�.
	// 
	// �츮 ����Ϳ� ���� �׷��ִ� ���� ���۸��� ���õ� ����.
	ComPtr<IDXGISwapChain> _swapChain = nullptr;

	/// RTV(Render Target View)
	// �츮�� ���� ���۸��� �� ��, �ĸ� ���ۿ��ٰ� ���� �׷��޶�� ��û���ؾߵǴ� ��.
	ComPtr<ID3D11RenderTargetView> _renderTargetView; // �ĸ� ���۸� �����ϴ� ����.

	// Misc
	D3D11_VIEWPORT _viewport = { 0 }; // ȭ���� ũ�⸦ ����
	float _clearColor[4] = { 0.5f, 0.5f, 0.5f, 0.5f };


private:
	// Geometry
	vector<Vertex> _vertices;
	ComPtr<ID3D11Buffer> _vertexBuffer = nullptr;

	// �ε��� ���۸� ���ؼ�, �ε��� ��ϵ�� �ε��� ���� ���� ����.
	vector<uint32> _indices;
	ComPtr<ID3D11Buffer> _indexBuffer = nullptr;
	ComPtr<ID3D11InputLayout> _inputLayout = nullptr;

	// VS
	ComPtr<ID3D11VertexShader> _vertexShader = nullptr;
	ComPtr<ID3DBlob> _vsBlob = nullptr;

	// RS
	ComPtr<ID3D11RasterizerState> _rasterizerState = nullptr;

	
	// PS
	ComPtr<ID3D11PixelShader> _pixelShader = nullptr;
	ComPtr<ID3DBlob> _psBlob = nullptr;

	// SRV
	ComPtr<ID3D11ShaderResourceView> _shaderResourceView = nullptr;
	ComPtr<ID3D11ShaderResourceView> _shaderResourceView2 = nullptr;

	ComPtr<ID3D11SamplerState> _samplerState = nullptr;
	ComPtr<ID3D11BlendState> _blendState = nullptr;

	// [CPU <-> RAM] [GPU <-> VRAM]

private:
	TransformData _transformData;
	ComPtr<ID3D11Buffer> _constantBuffer;
};

