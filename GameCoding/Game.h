#pragma once
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
	// �츮�� ���� ���۸��� �� ��, �ĸ� ���ۿ��ٰ� ���� �׷��޶�� ��û���ؾߵǴ� ������ ����ϰ� ����.
	ComPtr<ID3D11RenderTargetView> _renderTargetView; // �ĸ� ���۸� �����ϴ� ����.

	// Misc
	D3D11_VIEWPORT _viewport = { 0 }; // ȭ���� ũ�⸦ ����
	float _clearColor[4] = { 0.5f, 0.5f, 0.5f, 0.5f };

};

