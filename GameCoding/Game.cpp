#include "pch.h"
#include "Game.h"

Game::Game()
{
}

Game::~Game()
{
}

void Game::Init(HWND hwnd)
{
	_hwnd = hwnd;
	_width = GWinSizeX;
	_height = GWinSizeY;

	// AddRef는 우리가 device에 대해서 무언가를 사용할 때 레퍼런스 카운트를 증가시키는 방식.
	// 레퍼런스 카운터를 자동으로 관리하는 개념이 스마트 포인터 방식이다.
	// 
	// AddRef와 Release는 페어가 잘 맞지 않으면 메모리 누수가 일어나게 된다.
	// 따라서, 우리가 직접 관리하는 방식은 별로 올바른 방법이 아니게 된다.
	// 
	// _device->AddRef();
	// _device->Release();

	// TODO
	CreateDeviceAndSwapChain();
	CreateRenderTargetView();
	SetViewport();
}

void Game::Update()
{
}

void Game::Render()
{
	RenderBigin();

	// TODO

	RenderEnd();
}

void Game::RenderBigin()
{
	// OM : Output Merger
	// 마지막에 전체적으로 GPU가 그림을 그리는 데까지 성공했으면 그 결과물을 이 도화지에다가 그려줘~ 하는 도화지를 건네줘야함.
	// 그 도화지가 후면 버퍼이고 후면 버퍼를 묘사하기 위해서 우리는 renderTargetView라는 것을 만들어 놨었음.
	// 1 : 우리가 사용하고 있는 후면버퍼 하나.
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), nullptr);
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), _clearColor);
	_deviceContext->RSSetViewports(1, &_viewport);
}

void Game::RenderEnd()
{
	// [ ] <- [ ]
	// 후면버퍼를 전면버퍼에 복사해달라 그리고 이걸 화면에다가 출력해달라.
	HRESULT hr = _swapChain->Present(1, 0);
	CHECK(hr);
}

void Game::CreateDeviceAndSwapChain()
{
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	{
		desc.BufferDesc.Width = _width;
		desc.BufferDesc.Height = _height;
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 1;
		desc.OutputWindow = _hwnd;
		desc.Windowed = true;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	}

	// 후면 버퍼.
	HRESULT hr = ::D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&desc,
		_swapChain.GetAddressOf(),
		_device.GetAddressOf(),
		nullptr,
		_deviceContext.GetAddressOf()
	);

	CHECK(hr);
}

// 후면 버퍼를 묘사하는 _renderTargetView를 설정하는 함수.
void Game::CreateRenderTargetView()
{
	HRESULT hr;
		
	ComPtr<ID3D11Texture2D> backBuffer = nullptr;
	hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
	CHECK(hr);

	_device->CreateRenderTargetView(backBuffer.Get(), nullptr, _renderTargetView.GetAddressOf());
	CHECK(hr);
}

void Game::SetViewport()
{
	_viewport.TopLeftX = 0.f;
	_viewport.TopLeftY = 0.f;
	_viewport.Width = static_cast<float>(_width);
	_viewport.Height = static_cast<float>(_height);
	_viewport.MinDepth = 0.f;
	_viewport.MaxDepth = 1.f;
}
