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

	CreateGeometry();
	CreateVS();
	CreateInputLayout();
	CreatePS();

	CreateSRV();
	CreateConstantBuffer();
}

void Game::Update()
{
	// Scale Rotation Translation
	_transformData.offset.x = 0.3f;
	_transformData.offset.y = 0.3f;
	D3D11_MAPPED_SUBRESOURCE subResource;
	ZeroMemory(&subResource, sizeof(subResource));

	_deviceContext->Map(_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	::memcpy(subResource.pData, &_transformData, sizeof(_transformData));
	_deviceContext->Unmap(_constantBuffer.Get(), 0);
}

void Game::Render()
{
	RenderBigin();

	// IA - VS - RS - PS - OM
	{
		uint32 stride = sizeof(Vertex);
		uint32 offset = 0;

		// IA
		_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
		// 인덱스 버퍼도 렌더링 파이프 라인에 묶이게 됨.
		_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0); 
		_deviceContext->IASetInputLayout(_inputLayout.Get());
		_deviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// VS
		_deviceContext->VSSetShader(_vertexShader.Get(), nullptr, 0);
		_deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());

		// RS : 보간 단계

		// PS 
		_deviceContext->PSSetShader(_pixelShader.Get(), nullptr, 0);
		_deviceContext->PSSetShaderResources(0, 1, _shaderResourceView.GetAddressOf());
		_deviceContext->PSSetShaderResources(1, 1, _shaderResourceView2.GetAddressOf());

		// OM

		//_deviceContext->Draw(_vertices.size(), 0);
		_deviceContext->DrawIndexed(_indices.size(), 0, 0);
	}

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

void Game::CreateGeometry()
{
	// VertexData
	// 동적 배열을 사용함으로 기본적으로 CPU의 RAM을 사용하게 된다.
	// 1 3 
	// 0 2
	{
		_vertices.resize(4);

		_vertices[0].position = Vec3(-0.5f, -0.5f, 0.f);
		//_vertices[0].color = Color(1.f, 0.f, 0.f, 1.f);
		_vertices[0].uv = Vec2(0.f, 2.f); // 0번 좌표는 아래이므로 v는 1의 값을 가진다.

		_vertices[1].position = Vec3(-0.5f, 0.5f, 0);
		//_vertices[1].color = Color(1.f, 0.f, 0.f, 1.f);
		_vertices[1].uv = Vec2(0.f, 0.f);

		_vertices[2].position = Vec3(0.5f, -0.5f, 0);
		//_vertices[2].color = Color(1.f, 0.f, 0.f, 1.f);
		_vertices[2].uv = Vec2(2.f, 2.f); // 0번 좌표는 아래이므로 v는 1의 값을 가진다.

		_vertices[3].position = Vec3(0.5f, 0.5f, 0);
		//_vertices[3].color = Color(1.f, 0.f, 0.f, 1.f);
		_vertices[3].uv = Vec2(2.f, 0.f); // 0번 좌표는  아래이므로 v는 1의 값을 가진다.
	}
	
	// VertexBuffer
	// GPU의 VRAM을 사용하기 위해서 위에서 만든 버텍스 버퍼를 GPU에게 건네주는 처리.
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		// cpu와 gpu가 얘를 가지고 어떻게 작업을 할 지에 대한 처리.
		desc.Usage = D3D11_USAGE_IMMUTABLE; // D3D11_USAGE_IMMUTABLE : GPU만 읽을 수 있는 데이터로 한다.
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 정점 버퍼를 만드는데 사용하겠다.
		desc.ByteWidth = (uint32)sizeof(Vertex) * _vertices.size();

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = _vertices.data();

		// 새로운 리소스를 만들어주는 것은 우리의 Command Center인 _device의 CreateBuffer함수 사용.
		HRESULT hr = _device->CreateBuffer(&desc, &data, _vertexBuffer.GetAddressOf());
		CHECK(hr);
	}

	// Index
	// 0, 1, 2 와 2, 1, 3 삼각형.
	{
		_indices = { 0, 1, 2, 2, 1, 3 };
	}

	// IndexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER; // 인덱스 버퍼용으로 사용.
		desc.ByteWidth = (uint32)sizeof(uint32) * _indices.size();

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = _indices.data();

		// 새로운 리소스를 만들어주는 것은 우리의 Command Center인 _device의 CreateBuffer함수 사용.
		HRESULT hr = _device->CreateBuffer(&desc, &data, _indexBuffer.GetAddressOf());
		CHECK(hr);
	}
}

void Game::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		// POSITION이 12바이트를 차지하고 있으므로, COLOR의 시작 Offset은 12이다.
		// { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		// UV이므로, 32바이트 32바이트.
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	const int32 count = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	_device->CreateInputLayout(layout, count, _vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), _inputLayout.GetAddressOf());
}

void Game::CreateVS()
{
	// hlsl파일을 blob이라는 형태로 바꿔준다.
	LoadShaderFromFile(L"Default.hlsl", "VS", "vs_5_0", _vsBlob);

	HRESULT hr = _device->CreateVertexShader(_vsBlob->GetBufferPointer(),
		_vsBlob->GetBufferSize(), nullptr, _vertexShader.GetAddressOf());

	CHECK(hr);
}

void Game::CreatePS()
{
	LoadShaderFromFile(L"Default.hlsl", "PS", "ps_5_0", _psBlob);

	HRESULT hr = _device->CreatePixelShader(_psBlob->GetBufferPointer(),
		_psBlob->GetBufferSize(), nullptr, _pixelShader.GetAddressOf());

	CHECK(hr);
}

void Game::CreateSRV()
{
	DirectX::TexMetadata md;
	DirectX::ScratchImage img;
	HRESULT hr = ::LoadFromWICFile(L"Character1.png", WIC_FLAGS_NONE, &md, img);
	CHECK(hr);

	hr = ::CreateShaderResourceView(_device.Get(), img.GetImages(), img.GetImageCount(), md, _shaderResourceView.GetAddressOf());
	CHECK(hr);

	hr = ::LoadFromWICFile(L"Character2.png", WIC_FLAGS_NONE, &md, img);
	CHECK(hr);

	hr = ::CreateShaderResourceView(_device.Get(), img.GetImages(), img.GetImageCount(), md, _shaderResourceView2.GetAddressOf());
	CHECK(hr);


}

void Game::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DYNAMIC; // CPU_Write + GPU_Read 방식.
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // 상수 버퍼로 사용하겠다.
	desc.ByteWidth = sizeof(TransformData);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = _device->CreateBuffer(&desc, nullptr, _constantBuffer.GetAddressOf());
	CHECK(hr);
}

void Game::LoadShaderFromFile(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob)
{
	const uint32 compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	::D3DCompileFromFile(
		path.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		name.c_str(),
		version.c_str(),
		compileFlag,
		0,
		blob.GetAddressOf(),
		nullptr
	);
}

