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

	// AddRef�� �츮�� device�� ���ؼ� ���𰡸� ����� �� ���۷��� ī��Ʈ�� ������Ű�� ���.
	// ���۷��� ī���͸� �ڵ����� �����ϴ� ������ ����Ʈ ������ ����̴�.
	// 
	// AddRef�� Release�� �� �� ���� ������ �޸� ������ �Ͼ�� �ȴ�.
	// ����, �츮�� ���� �����ϴ� ����� ���� �ùٸ� ����� �ƴϰ� �ȴ�.
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
		// �ε��� ���۵� ������ ������ ���ο� ���̰� ��.
		_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0); 
		_deviceContext->IASetInputLayout(_inputLayout.Get());
		_deviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// VS
		_deviceContext->VSSetShader(_vertexShader.Get(), nullptr, 0);
		_deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());

		// RS : ���� �ܰ�

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
	// �������� ��ü������ GPU�� �׸��� �׸��� ������ ���������� �� ������� �� ��ȭ�����ٰ� �׷���~ �ϴ� ��ȭ���� �ǳ������.
	// �� ��ȭ���� �ĸ� �����̰� �ĸ� ���۸� �����ϱ� ���ؼ� �츮�� renderTargetView��� ���� ����� ������.
	// 1 : �츮�� ����ϰ� �ִ� �ĸ���� �ϳ�.
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), nullptr);
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), _clearColor);
	_deviceContext->RSSetViewports(1, &_viewport);
}

void Game::RenderEnd()
{
	// [ ] <- [ ]
	// �ĸ���۸� ������ۿ� �����ش޶� �׸��� �̰� ȭ�鿡�ٰ� ����ش޶�.
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

	// �ĸ� ����.
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

// �ĸ� ���۸� �����ϴ� _renderTargetView�� �����ϴ� �Լ�.
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
	// ���� �迭�� ��������� �⺻������ CPU�� RAM�� ����ϰ� �ȴ�.
	// 1 3 
	// 0 2
	{
		_vertices.resize(4);

		_vertices[0].position = Vec3(-0.5f, -0.5f, 0.f);
		//_vertices[0].color = Color(1.f, 0.f, 0.f, 1.f);
		_vertices[0].uv = Vec2(0.f, 2.f); // 0�� ��ǥ�� �Ʒ��̹Ƿ� v�� 1�� ���� ������.

		_vertices[1].position = Vec3(-0.5f, 0.5f, 0);
		//_vertices[1].color = Color(1.f, 0.f, 0.f, 1.f);
		_vertices[1].uv = Vec2(0.f, 0.f);

		_vertices[2].position = Vec3(0.5f, -0.5f, 0);
		//_vertices[2].color = Color(1.f, 0.f, 0.f, 1.f);
		_vertices[2].uv = Vec2(2.f, 2.f); // 0�� ��ǥ�� �Ʒ��̹Ƿ� v�� 1�� ���� ������.

		_vertices[3].position = Vec3(0.5f, 0.5f, 0);
		//_vertices[3].color = Color(1.f, 0.f, 0.f, 1.f);
		_vertices[3].uv = Vec2(2.f, 0.f); // 0�� ��ǥ��  �Ʒ��̹Ƿ� v�� 1�� ���� ������.
	}
	
	// VertexBuffer
	// GPU�� VRAM�� ����ϱ� ���ؼ� ������ ���� ���ؽ� ���۸� GPU���� �ǳ��ִ� ó��.
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		// cpu�� gpu�� �긦 ������ ��� �۾��� �� ���� ���� ó��.
		desc.Usage = D3D11_USAGE_IMMUTABLE; // D3D11_USAGE_IMMUTABLE : GPU�� ���� �� �ִ� �����ͷ� �Ѵ�.
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ���� ���۸� ����µ� ����ϰڴ�.
		desc.ByteWidth = (uint32)sizeof(Vertex) * _vertices.size();

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = _vertices.data();

		// ���ο� ���ҽ��� ������ִ� ���� �츮�� Command Center�� _device�� CreateBuffer�Լ� ���.
		HRESULT hr = _device->CreateBuffer(&desc, &data, _vertexBuffer.GetAddressOf());
		CHECK(hr);
	}

	// Index
	// 0, 1, 2 �� 2, 1, 3 �ﰢ��.
	{
		_indices = { 0, 1, 2, 2, 1, 3 };
	}

	// IndexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER; // �ε��� ���ۿ����� ���.
		desc.ByteWidth = (uint32)sizeof(uint32) * _indices.size();

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = _indices.data();

		// ���ο� ���ҽ��� ������ִ� ���� �츮�� Command Center�� _device�� CreateBuffer�Լ� ���.
		HRESULT hr = _device->CreateBuffer(&desc, &data, _indexBuffer.GetAddressOf());
		CHECK(hr);
	}
}

void Game::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		// POSITION�� 12����Ʈ�� �����ϰ� �����Ƿ�, COLOR�� ���� Offset�� 12�̴�.
		// { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		// UV�̹Ƿ�, 32����Ʈ 32����Ʈ.
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	const int32 count = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	_device->CreateInputLayout(layout, count, _vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), _inputLayout.GetAddressOf());
}

void Game::CreateVS()
{
	// hlsl������ blob�̶�� ���·� �ٲ��ش�.
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
	desc.Usage = D3D11_USAGE_DYNAMIC; // CPU_Write + GPU_Read ���.
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // ��� ���۷� ����ϰڴ�.
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

