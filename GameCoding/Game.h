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
	// 위의 ID3D11Device을 스마트 포인터를 사용하기 위해서 ComPtr을 감싸주는 방식을 아래처럼 사용.

	// ComPtr이 하는 것?
	// 기본적으로는 AddRef()나 Release()를 자동으로 해주는 역할이 있지만, 더 중요한 것은 아래의 내용이다.
	// 
	// 현재 수많은 종류의 GPU가 존재하는데 모든 GPU의 스펙을 일일이 비교해가며 맞추는 작업을
	// 우리가 직접 하는 것은 불가능하다.
	// 따라서, Com인터페이스라는 것의 핵심은 마이크로소프트에서 만든 DirectX 라이브러리의 
	// 특정 클래스들만 우리가 작업을 해주면, GPU제품들이 알아서 하드웨어적으로 연결이 되는 인터페이스이다.
	ComPtr<ID3D11Device> _device = nullptr;
	ComPtr<ID3D11DeviceContext> _deviceContext = nullptr; // 리소스랑 렌더링 파이프라인을 묶어주는 역할.

	// DXGI란?
	// DXGI는 렌더링 하는데에 있어 하드웨어를 효율적으로 돌릴 수 있도록 여러 저수준 작업들을 제공해준다.
	// 또한, 여러가지 공통적인 그래픽 기능성을 처리하는데, 이를테면 화면 모드 전환, 디스플레이 어댑터나 모니터,
	// 지원되는 디스플레이 모드(해상도, 주사율 등) 같은 그래픽 시스템 정보의 열거 등의 기능을 제공해준다.
	// 
	// 우리 모니터에 뭔가 그려주는 더블 버퍼링과 관련된 변수.
	ComPtr<IDXGISwapChain> _swapChain = nullptr;

	/// RTV(Render Target View)
	// 우리는 더블 버퍼링을 할 때, 후면 버퍼에다가 뭔가 그려달라는 요청을해야되는 것으로 사용하고 있음.
	ComPtr<ID3D11RenderTargetView> _renderTargetView; // 후면 버퍼를 묘사하는 존재.

	// Misc
	D3D11_VIEWPORT _viewport = { 0 }; // 화면의 크기를 묘사
	float _clearColor[4] = { 0.5f, 0.5f, 0.5f, 0.5f };

};

