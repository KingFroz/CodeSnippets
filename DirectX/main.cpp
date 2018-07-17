// CGS HW Project A "Line Land".
// Author: L.Norri CD CGS, FullSail University

// Introduction:
// Welcome to the hardware project of the Computer Graphics Systems class.
// This assignment is fully guided but still requires significant effort on your part. 
// Future classes will demand the habits & foundation you develop right now!  
// It is CRITICAL that you follow each and every step. ESPECIALLY THE READING!!!

// TO BEGIN: Open the word document that acompanies this project and start from the top.

//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************

#include <iostream>
#include <ctime>
#include "XTime.h"
#include <DirectXMath.h>
using namespace DirectX;

// BEGIN PART 1

// TODO: PART 1 STEP 1a
#include <d3d11.h>
#pragma comment(lib, "d3d11")

// TODO: PART 1 STEP 1b
#include "math.h"
// TODO: PART 2 STEP 6
#include "Trivial_PS.csh"
#include "Trivial_VS.csh"

#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500

//************************************************************
//************ SIMPLE WINDOWS APP CLASS **********************
//************************************************************

class DEMO_APP
{	
	HINSTANCE						application;
	WNDPROC							appWndProc;
	HWND							window;
	// TODO: PART 1 STEP 2
	XTime time;

	ID3D11Device *device = nullptr;
	ID3D11DeviceContext *context = nullptr;
	HRESULT result;

	D3D_FEATURE_LEVEL flevel1;
	D3D_FEATURE_LEVEL flevel2;

	IDXGISwapChain *swapChain = nullptr;
	ID3D11Texture2D* swapBuf = 0;

	ID3D11RenderTargetView* view = {};
	HRESULT viewpoint;

	D3D11_VIEWPORT viewport;
	DXGI_SWAP_CHAIN_DESC swap = {};


	// TODO: PART 2 STEP 2
	ID3D11Buffer *Circle = nullptr;
	unsigned int count;
	
	ID3D11InputLayout *inputLayout = nullptr;
	// BEGIN PART 5
	// TODO: PART 5 STEP 1
	
	// TODO: PART 2 STEP 4
	ID3D11VertexShader *vertShader = nullptr;
	ID3D11PixelShader *pixShader = nullptr;

	// BEGIN PART 3
	// TODO: PART 3 STEP 1
	ID3D11Buffer *constBuffer = nullptr;
	// TODO: PART 3 STEP 2b
	struct SEND_TO_VRAM
	{
		XMFLOAT4 constantColor;
		XMFLOAT2 constantOffset;
		XMFLOAT2 padding;
	};


	// TODO: PART 3 STEP 4a
	SEND_TO_VRAM toShader;
public:
	// BEGIN PART 2
	// TODO: PART 2 STEP 1
	struct Simple_Vert
	{
		float x;
		float y;
	};

	DEMO_APP(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();
};

//************************************************************
//************ CREATION OF OBJECTS & RESOURCES ***************
//************************************************************

DEMO_APP::DEMO_APP(HINSTANCE hinst, WNDPROC proc)
{
	// ****************** BEGIN WARNING ***********************// 
	// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY! 
	application = hinst; 
	appWndProc = proc; 

	WNDCLASSEX  wndClass;
    ZeroMemory( &wndClass, sizeof( wndClass ) );
    wndClass.cbSize         = sizeof( WNDCLASSEX );             
    wndClass.lpfnWndProc    = appWndProc;						
    wndClass.lpszClassName  = L"DirectXApplication";            
	wndClass.hInstance      = application;		               
    wndClass.hCursor        = LoadCursor( NULL, IDC_ARROW );    
    wndClass.hbrBackground  = ( HBRUSH )( COLOR_WINDOWFRAME ); 
	//wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));
    RegisterClassEx( &wndClass );

	RECT window_size = { 0, 0, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT };
	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

	window = CreateWindow(	L"DirectXApplication", L"CGS Hardware Project", WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
							CW_USEDEFAULT, CW_USEDEFAULT, window_size.right - window_size.left, window_size.bottom - window_size.top,
							NULL, NULL, application, this);

    ShowWindow( window, SW_SHOW );
	//********************* END WARNING ************************//

	// TODO: PART 1 STEP 3a
	struct SwapChain
	{
		HRESULT buffer;
		HRESULT outPut;
		HRESULT Desc;
		HRESULT frameStats;
		HRESULT screenState;
		HRESULT lastPresentCount;
		HRESULT present;
		HRESULT resizeBuf;
		HRESULT resizeTar;
		HRESULT setScreenState;
	};

	// TODO: PART 1 STEP 3b

	swap.BufferCount = 1;
	swap.BufferDesc.Height = BACKBUFFER_HEIGHT;
	swap.BufferDesc.Width = BACKBUFFER_WIDTH;
	swap.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap.OutputWindow = window;
	swap.SampleDesc.Count = 1;
	swap.SampleDesc.Quality = 0;
	swap.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap.Windowed = true;
	
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, 0, D3D11_SDK_VERSION, &swap, &swapChain, &device, &flevel2, &context);
	// TODO: PART 1 STEP 4
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&swapBuf);

	viewpoint = device->CreateRenderTargetView(swapBuf, 0, &view);

	MY_VERTEX geometry[6] = 
	{
		{ (-0.5f, 0.5f, 0.0f) }, 
		{ (1.0f, 0.0f, 0.0f, 1.0f) },
		{ (0.5f, 0.5f, 0.0f) }, 
		{ (0.0f, 1.0f, 0.0f, 1.0f) },
		{ (0.0f, -0.5f, 0.0f) },
		{ (0.0f, 0.0f, 1.0f, 1.0f) }
	};

	// TODO: PART 1 STEP 5
	viewport.Height = BACKBUFFER_HEIGHT;
	viewport.Width = BACKBUFFER_WIDTH;
	viewport.MaxDepth = 1;
	viewport.MinDepth = 0;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	// TODO: PART 2 STEP 3a
	Simple_Vert fullcircle[361] = {};
	
	unsigned int i = 0;
	for (; i < 361; i++)
	{
		fullcircle[i].x = (float)cos(i * PI / 180) * 1;
		fullcircle[i].y = (float)sin(i * PI / 180) * 1;
	}

	//for (i = 0; i < 361; i++)
	//{
	//	fullcircle[i].x *= 0.2f;
	//	fullcircle[i].y *= 0.2f;
	//}

	// BEGIN PART 4
	// TODO: PART 4 STEP 1

	// TODO: PART 2 STEP 3b
	D3D11_BUFFER_DESC vecBuff;
	vecBuff.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vecBuff.Usage = D3D11_USAGE_IMMUTABLE;
	vecBuff.CPUAccessFlags = NULL;
	vecBuff.ByteWidth = (sizeof(Simple_Vert) * 361);
	vecBuff.MiscFlags = NULL;
	vecBuff.StructureByteStride = (sizeof(Simple_Vert));

	D3D11_BUFFER_DESC PSBuff;
	PSBuff.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	PSBuff.Usage = D3D11_USAGE_DYNAMIC;
	PSBuff.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	PSBuff.ByteWidth = (sizeof(SEND_TO_VRAM));
	PSBuff.MiscFlags = NULL;
	PSBuff.StructureByteStride = 0;

    // TODO: PART 2 STEP 3c
	D3D11_SUBRESOURCE_DATA subD;
	subD.pSysMem = fullcircle;
	subD.SysMemPitch = 0;
	subD.SysMemSlicePitch = 0;
	// TODO: PART 2 STEP 3d
	device->CreateBuffer(&vecBuff, &subD, &Circle);
	device->CreateBuffer(&PSBuff, NULL, &constBuffer);
	// TODO: PART 5 STEP 2a
	
	// TODO: PART 5 STEP 2b
	
	// TODO: PART 5 STEP 3
		
	// TODO: PART 2 STEP 5
	// ADD SHADERS TO PROJECT, SET BUILD OPTIONS & COMPILE

	// TODO: PART 2 STEP 7
	device->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), NULL, &vertShader);
	device->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), NULL, &pixShader);

	
	// TODO: PART 2 STEP 8a
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	
	// TODO: PART 2 STEP 8b
	device->CreateInputLayout(layout, ARRAYSIZE(layout), Trivial_VS, sizeof(Trivial_VS), &inputLayout);
	// TODO: PART 3 STEP 3
	D3D11_BUFFER_DESC constBuffDesc;
	constBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	constBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBuffDesc.ByteWidth = (sizeof(SEND_TO_VRAM));
	constBuffDesc.MiscFlags = NULL;
	constBuffDesc.StructureByteStride = (sizeof(SEND_TO_VRAM));


	device->CreateBuffer(&constBuffDesc, NULL, &constBuffer);
	// TODO: PART 3 STEP 4b
	toShader.constantColor.x = D3D11_FLOAT32_MAX;
	toShader.constantColor.y = D3D11_FLOAT32_MAX;

	toShader.constantOffset.x = 0;
	toShader.constantOffset.y = 0;


}

//************************************************************
//************ EXECUTION *************************************
//************************************************************

bool DEMO_APP::Run()
{
	// TODO: PART 4 STEP 2	
	
	// TODO: PART 4 STEP 3
	
	// TODO: PART 4 STEP 5
	
	// END PART 4

	// TODO: PART 1 STEP 7a
	context->OMSetRenderTargets(1, &view, nullptr);
	// TODO: PART 1 STEP 7b
	context->RSSetViewports(1, &viewport);
	// TODO: PART 1 STEP 7c
	FLOAT color[] = { 0, 0, 1, 0 };
	context->ClearRenderTargetView(view, color);
	// TODO: PART 5 STEP 4
	
	// TODO: PART 5 STEP 5
	
	// TODO: PART 5 STEP 6
	
	// TODO: PART 5 STEP 7
	
	// END PART 5
	
	// TODO: PART 3 STEP 5
	D3D11_MAPPED_SUBRESOURCE map;
	context->Map(constBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy_s(map.pData, sizeof(SEND_TO_VRAM), &toShader, sizeof(SEND_TO_VRAM));
	context->Unmap(constBuffer, 0);
	// TODO: PART 3 STEP 6
	context->VSSetConstantBuffers(0, 1, &constBuffer);
	// TODO: PART 2 STEP 9a
	unsigned int sizeSVS = sizeof(Simple_Vert);
	unsigned int offset = 0;

	context->IASetVertexBuffers(0, 1, &Circle, &sizeSVS, &offset);
	// TODO: PART 2 STEP 9b
	context->PSSetShader(pixShader, NULL, 0);
	context->VSSetShader(vertShader, NULL, 0);
	// TODO: PART 2 STEP 9c
	context->IASetInputLayout(inputLayout);
	// TODO: PART 2 STEP 9d
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	// TODO: PART 2 STEP 10
	context->Draw(361, 0);
	// END PART 2

	// TODO: PART 1 STEP 8
	swapChain->Present(0, 0);
	// END OF PART 1
	return true; 
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************

bool DEMO_APP::ShutDown()
{
	// TODO: PART 1 STEP 6
	context->Release();
	view->Release();
	swapChain->Release();
	device->Release();
	swapBuf->Release();
	UnregisterClass( L"DirectXApplication", application ); 
	return true;
}

//************************************************************
//************ WINDOWS RELATED *******************************
//************************************************************

// ****************** BEGIN WARNING ***********************// 
// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY!
	
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine,	int nCmdShow );						   
LRESULT CALLBACK WndProc(HWND hWnd,	UINT message, WPARAM wparam, LPARAM lparam );		
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE, LPTSTR, int )
{
	srand(unsigned int(time(0)));
	DEMO_APP myApp(hInstance,(WNDPROC)WndProc);	
    MSG msg; ZeroMemory( &msg, sizeof( msg ) );
    while ( msg.message != WM_QUIT && myApp.Run() )
    {	
	    if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        { 
            TranslateMessage( &msg );
            DispatchMessage( &msg ); 
        }
    }
	myApp.ShutDown(); 
	return 0; 
}
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if(GetAsyncKeyState(VK_ESCAPE))
		message = WM_DESTROY;
    switch ( message )
    {
        case ( WM_DESTROY ): { PostQuitMessage( 0 ); }
        break;
    }
    return DefWindowProc( hWnd, message, wParam, lParam );
}
//********************* END WARNING ************************//