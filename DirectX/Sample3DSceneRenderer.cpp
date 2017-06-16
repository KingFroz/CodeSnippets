#define _CRT_SECURE_NO_DEPRECATE
#include "pch.h"
#include "Sample3DSceneRenderer.h"
#include "DDSTextureLoader.h"
#include "..\Common\DirectXHelper.h"
#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <Windows.h>
using namespace GXII_ListonH;

using namespace std;
using namespace DirectX;
using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_indexCount(0),
	m_tracking(false),
	m_deviceResources(deviceResources)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();

	static const XMVECTORF32 eye = { 0.0f, 0.0f, -1.5f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };
	XMStoreFloat4x4(&camera, XMMatrixLookAtRH(eye, at, up));

	constPerFrame.camPosition = { camera._31, camera._32, camera._33, camera._34 };
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixInverse(0, XMMatrixLookAtRH(eye, at, up))));
}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = (outputSize.Width / 2.0f) / outputSize.Height;
	float fovAngleY = 40.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
		);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	XMStoreFloat4x4(
		&m_constantBufferData.projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
		);

	// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
	static const XMVECTORF32 eye = { 0.0f, 0.7f, 1.5f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
}

void Sample3DSceneRenderer::ModelLoader(const char* path, std::vector<VertexPositionColor> &modelverts, std::vector<unsigned int> &modelindices)
{
	std::vector<unsigned int> vIndices, uvIndices, nIndices;
	std::vector<XMFLOAT3> temp_vertices;
	std::vector<XMFLOAT2> temp_uvs;
	std::vector<XMFLOAT3> temp_normals;

	FILE * file;
	fopen_s(&file, path, "r");

	if (file == NULL)
	{
		cerr << "Error Opening File" << endl;
		exit(1);
	}

	while (1)
	{
		char header[128];

		unsigned int res = fscanf_s(file, "%s", header, _countof(header));
		if (res == EOF)
			break;

		if (strcmp(header, "v") == 0)
		{
			XMFLOAT3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(header, "vt") == 0)
		{
			XMFLOAT2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(header, "vn") == 0)
		{
			XMFLOAT3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(header, "f") == 0)
		{
			unsigned int pos[3], uv[3], norm[3];
			int count = 0;
			count = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n"
				, &pos[0], &uv[0], &norm[0]
				, &pos[1], &uv[1], &norm[1]
				, &pos[2], &uv[2], &norm[2]
			);

			if (count != 9)
			{
				break;
			}

			vIndices.push_back(pos[0]);
			vIndices.push_back(pos[2]);
			vIndices.push_back(pos[1]);

			uvIndices.push_back(uv[0]);
			uvIndices.push_back(uv[2]);
			uvIndices.push_back(uv[1]);

			nIndices.push_back(norm[0]);
			nIndices.push_back(norm[2]);
			nIndices.push_back(norm[1]);
		}
	}

	fclose(file);

	for (unsigned int i = 0; i < vIndices.size(); i++)
	{
		VertexPositionColor temp;

		unsigned int vIndex = vIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int nIndex = nIndices[i];

		temp.pos = temp_vertices[vIndex - 1];
		temp.uv = temp_uvs[uvIndex - 1];
		temp.normals = temp_normals[nIndex - 1];

		temp.uv.y = 1 - temp.uv.y;

		modelverts.push_back(temp);
		modelindices.push_back(i);
	}
}

using namespace Windows::UI::Core;
extern CoreWindow^ gwindow;
#include <atomic>
extern bool mouse_move;
extern float diffx;
extern float diffy;
extern bool w_down;
extern bool a_down;
extern bool s_down;
extern bool d_down;
extern bool left_click;
extern bool g_down;

extern char buttons[256];

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{
	if (!m_tracking)
	{
		// Convert degrees to radians, then convert seconds to rotation angle
		float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
		double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

		Rotate(radians);
	}

	XMMATRIX newcamera = XMLoadFloat4x4(&camera);

	if (w_down)
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[2] * -timer.GetElapsedSeconds() * 5.0;
	}

	if (a_down)
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[0] * -timer.GetElapsedSeconds() *5.0;
	}

	if (s_down)
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[2] * timer.GetElapsedSeconds() * 5.0;
	}

	if (d_down)
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[0] * timer.GetElapsedSeconds() * 5.0;
	}

	Windows::UI::Input::PointerPoint^ point = nullptr;

	//if(mouse_move)/*This crashes unless a mouse event actually happened*/
	//point = Windows::UI::Input::PointerPoint::GetCurrentPoint(pointerID);

	if (mouse_move)
	{
		// Updates the application state once per frame.
		if (left_click)
		{
			XMVECTOR pos = newcamera.r[3];
			newcamera.r[3] = XMLoadFloat4(&XMFLOAT4(0, 0, 0, 1));
			newcamera = XMMatrixRotationX(-diffy*0.01f) * newcamera * XMMatrixRotationY(-diffx*0.01f);
			newcamera.r[3] = pos;
		}
	}

	XMStoreFloat4x4(&camera, newcamera);

	/*Be sure to inverse the camera & Transpose because they don't use pragma pack row major in shaders*/
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixInverse(0, newcamera)));

	mouse_move = false;/*Reset*/
}

// Rotate the 3D cube model a set amount of radians.
void Sample3DSceneRenderer::Rotate(float radians)
{
	// Prepare to pass the updated model matrix to the shader
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(radians)));
}

void Sample3DSceneRenderer::StartTracking()
{
	m_tracking = true;
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void Sample3DSceneRenderer::TrackingUpdate(float positionX)
{
	if (m_tracking)
	{
		float radians = XM_2PI * 2.0f * positionX / m_deviceResources->GetOutputSize().Width;
		Rotate(radians);
	}
}

void Sample3DSceneRenderer::StopTracking()
{
	m_tracking = false;
}

// Renders one frame using the vertex and pixel shaders.
void Sample3DSceneRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

#pragma region LIGHTBUFFERSETUP
	if (toggle == true)
	{
		light.pos.z += 0.02f;

		_spotlight.dir.x -= 0.01f;
		_spotlight.pos.x += 0.01f;

		dirLight.dir.z += 0.01f;
		dirLight.dir.y += 0.01f;
		dirLight.dir.x += 0.01f;

		if (light.pos.z > 5.0f)
			toggle = false;
	}
	else {
		light.pos.z -= 0.02f;

		_spotlight.dir.x += 0.01f;
		_spotlight.pos.x -= 0.01f;

		dirLight.dir.z -= 0.01f;
		dirLight.dir.y -= 0.01f;
		dirLight.dir.x -= 0.01f;

		if (light.pos.z < 0.2)
			toggle = true;
	}

	_spotlight.pos.x = 2.0f;
	_spotlight.pos.y = 0.5f;
	_spotlight.pos.z = 0.5f;

	if (g_down)
		constPerFrame.GrayScale = true;
	else
		constPerFrame.GrayScale = false;

	constPerFrame.dirlight = dirLight;
	constPerFrame.light = light;
	constPerFrame._spotlight = _spotlight;

	constPerFrame.camPosition = { camera._41, camera._42, camera._43, camera._44 };

	context->UpdateSubresource(m_perframeBuffer, 0, NULL, &constPerFrame, 0, 0);
	context->PSSetConstantBuffers(0, 1, &m_perframeBuffer);
#pragma endregion

#pragma region BLENDING
#if 1
	////////////////BLENDING/////////////////////
	XMMATRIX CamPos = XMLoadFloat4x4(&camera);
	XMVECTOR pos = CamPos.r[3];

	XMVECTOR position = XMVectorZero();
	position = XMVector3TransformCoord(position, cWorld1);

	float dis_x = XMVectorGetX(position) - XMVectorGetX(pos);
	float dis_y = XMVectorGetY(position) - XMVectorGetY(pos);
	float dis_z = XMVectorGetZ(position) - XMVectorGetZ(pos);

	float dis_one = (dis_x * dis_x) + (dis_y * dis_y) + (dis_z * dis_z);
	//////////////////////////////////////////////////////////////
	position = XMVectorZero();
	position = XMVector3TransformCoord(position, cWorld2);

	dis_x = XMVectorGetX(position) - XMVectorGetX(pos);
	dis_y = XMVectorGetY(position) - XMVectorGetY(pos);
	dis_z = XMVectorGetZ(position) - XMVectorGetZ(pos);

	float dis_two = (dis_x * dis_x) + (dis_y * dis_y) + (dis_z * dis_z);
	//////////////////////////////////////////////////////////////
	position = XMVectorZero();
	position = XMVector3TransformCoord(position, cWorld3);

	dis_x = XMVectorGetX(position) - XMVectorGetX(pos);
	dis_y = XMVectorGetY(position) - XMVectorGetY(pos);
	dis_z = XMVectorGetZ(position) - XMVectorGetZ(pos);

	float dis_three = (dis_x * dis_x) + (dis_y * dis_y) + (dis_z * dis_z);
	//////////////////////////////////////////////////////////////
	if (dis_one < dis_two)
		reversed = true;

	if (dis_three < dis_two)
		reversed = false;


	float blendFactor[] = { 0.0f, 0.0f , 0.0f , 0.0f };
	context->OMSetBlendState(0, 0, 0xffffffff);
	context->OMSetBlendState(Transparency, blendFactor, 0xffffffff);
#endif
#pragma endregion

	UINT stride = { sizeof(VertexPositionColor) };
	UINT offset = 0;

#pragma region RENDERTOTEXTURE
#if 1
	context->ClearRenderTargetView(m_rtViewMap, DirectX::Colors::White);
	context->OMSetRenderTargets(1, &m_rtViewMap, m_dsViewMap);
	context->ClearDepthStencilView(m_dsViewMap, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 1.0f);


	trans = XMMatrixTranslation(0.5f, 0.0f, 1.5f);
	gumbaWorld2 = trans;

	static float ang = 0;
	ang += 0.01f;
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationX(ang) * gumbaWorld2));

	XMFLOAT4X4 temp = m_constantBufferData.view;

	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(g_XMZero, gumbaWorld2.r[3], g_XMIdentityR1)));

	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(
		m_constantBuffer.Get(),
		0,
		NULL,
		&m_constantBufferData,
		0,
		0,
		0
	);

	m_constantBufferData.view = temp;

	context->IASetVertexBuffers(
		0,
		1,
		m_vertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetIndexBuffer(
		m_indexBuffer.Get(),
		DXGI_FORMAT_R32_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_constantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);

	context->PSSetShaderResources(
		0,
		1,
		ModelTexture.GetAddressOf());

	context->PSSetSamplers(
		0,
		1,
		ModelTextureSampleState.GetAddressOf());

	//Draw the objects.
	context->DrawIndexed(
		m_indexCount,
		0,
		0
	);

	context->GenerateMips(m_srvTextureMap.Get());

	ID3D11RenderTargetView *const target2[1] = { m_deviceResources->GetBackBufferRenderTargetView() };

	context->OMSetRenderTargets(1, target2, m_deviceResources->GetDepthStencilView());


#endif
#pragma endregion

#pragma region SKYBOX
#if 0
	trans = XMMatrixTranslation(0.0f, 0.0f, 1.0f);
	sphereWorld = trans;
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(sphereWorld));

	context->UpdateSubresource1(
		m_SkyconstantBuffer.Get(),
		0,
		NULL,
		&m_constantBufferData,
		0,
		0,
		0
	);

	// Each vertex is one instance of the VertexPositionColor struct.
	context->IASetVertexBuffers(
		0,
		1,
		SkyBox_VB.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetIndexBuffer(
		SkyBox_IB.Get(),
		DXGI_FORMAT_R32_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_SkyconstantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);

	context->PSSetShaderResources(
		0,
		1,
		SPModelTexture.GetAddressOf());

	context->PSSetSamplers(
		0,
		1,
		SPModelTextureSampleState.GetAddressOf());

	context->RSSetState(CCWcullMode);

	context->DrawIndexed(
		m_SkyindexCount,
		0,
		0
	);
#endif
#pragma endregion

#pragma region 1st GUMBA
#if 1
	trans = XMMatrixTranslation(0.0f, 0.0f, 1.0f);
	gumbaWorld1 = trans;
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(gumbaWorld1));
	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(
		m_constantBuffer.Get(),
		0,
		NULL,
		&m_constantBufferData,
		0,
		0,
		0
	);

	context->IASetVertexBuffers(
		0,
		1,
		m_vertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetIndexBuffer(
		m_indexBuffer.Get(),
		DXGI_FORMAT_R32_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_constantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);

	context->PSSetShaderResources(
		0,
		1,
		ModelTexture.GetAddressOf());

	context->PSSetSamplers(
		0,
		1,
		ModelTextureSampleState.GetAddressOf());

	context->RSSetState(CWcullMode);

	// Draw the objects.
	context->DrawIndexed(
		m_indexCount,
		0,
		0
	);
#endif
#pragma endregion

#pragma region 2nd GUMBA
#if 1
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(gumbaWorld2));
	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(
		m_constantBuffer.Get(),
		0,
		NULL,
		&m_constantBufferData,
		0,
		0,
		0
	);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_constantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	context->PSSetSamplers(
		0,
		1,
		ModelTextureSampleState.GetAddressOf());

	context->RSSetState(CWcullMode);

	// Draw the objects.
	context->DrawIndexed(
		m_indexCount,
		0,
		0
	);
#endif
#pragma endregion

#pragma region 3rd GUMBA
#if 1
	trans = XMMatrixTranslation(1.0f, 0.0f, 3.0f);
	gumbaWorld3 = trans;

	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(gumbaWorld3));
	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(
		m_constantBuffer.Get(),
		0,
		NULL,
		&m_constantBufferData,
		0,
		0,
		0
	);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_constantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	context->PSSetSamplers(
		0,
		1,
		ModelTextureSampleState.GetAddressOf());

	context->RSSetState(CWcullMode);

	// Draw the objects.
	context->DrawIndexed(
		m_indexCount,
		0,
		0
	);
#endif
#pragma endregion

#pragma region SPOTLIGHT
#if 1
	spWorld = XMMatrixIdentity();
	spWorld = XMMatrixTranslation(0.0f, 2.0f, 2.5f);
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(spWorld));

	context->UpdateSubresource1(
		m_SPconstantBuffer.Get(),
		0,
		NULL,
		&m_constantBufferData,
		0,
		0,
		0
	);

	// Each vertex is one instance of the VertexPositionColor struct.
	context->IASetVertexBuffers(
		0,
		1,
		m_SPvertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetIndexBuffer(
		m_SPindexBuffer.Get(),
		DXGI_FORMAT_R32_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_SPconstantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);

	context->PSSetShaderResources(
		0,
		1,
		SPModelTexture.GetAddressOf());

	context->PSSetSamplers(
		0,
		1,
		SPModelTextureSampleState.GetAddressOf());

	context->RSSetState(CCWcullMode);

	context->DrawIndexed(
		m_SPindexCount,
		0,
		0
	);
#endif
#pragma endregion

#pragma region FLOOR
#if 1
	///////////FLOOR///////////
	spWorld = XMMatrixIdentity();
	spWorld = XMMatrixTranslation(0.0f, -0.5f, 2.5f);
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(spWorld));

	context->UpdateSubresource1(
		m_FconstantBuffer.Get(),
		0,
		NULL,
		&m_constantBufferData,
		0,
		0,
		0
	);

	// Each vertex is one instance of the VertexPositionColor struct.
	context->IASetVertexBuffers(
		0,
		1,
		m_FvertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetIndexBuffer(
		m_FindexBuffer.Get(),
		DXGI_FORMAT_R32_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_FconstantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);

	context->PSSetShaderResources(
		0,
		1,
		m_srvTextureMap.GetAddressOf());

	//context->PSSetSamplers(
	//	0,
	//	1,
	//	FModelTextureSampleState.GetAddressOf());

	// Draw the objects.
	//context->RSSetState(CWcullMode);

	context->DrawIndexed(
		m_FindexCount,
		0,
		0
	);
#endif
#pragma endregion

if (reversed == false)
{
#pragma region Tree ONE
#if 1
		cWorld1 = XMMatrixTranslation(-2.0f, 2.0f, 2.5f);
		XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixScaling(0.1f, 0.1f, 0.1f) * cWorld1));

		context->UpdateSubresource1(
			m_SBconstantBuffer.Get(),
			0,
			NULL,
			&m_constantBufferData,
			0,
			0,
			0
		);

		// Each vertex is one instance of the VertexPositionColor struct.
		context->IASetVertexBuffers(
			0,
			1,
			m_SBvertexBuffer.GetAddressOf(),
			&stride,
			&offset
		);

		context->IASetIndexBuffer(
			m_SBindexBuffer.Get(),
			DXGI_FORMAT_R32_UINT, // Each index is one 16-bit unsigned integer (short).
			0
		);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->IASetInputLayout(m_inputLayout.Get());

		// Attach our vertex shader.
		context->VSSetShader(
			m_vertexShader.Get(),
			nullptr,
			0
		);

		// Send the constant buffer to the graphics device.
		context->VSSetConstantBuffers1(
			0,
			1,
			m_SBconstantBuffer.GetAddressOf(),
			nullptr,
			nullptr
		);

		// Attach our pixel shader.
		context->PSSetShader(
			m_pixelShader.Get(),
			nullptr,
			0
		);

		context->PSSetShaderResources(
			0,
			1,
			TModelTexture.GetAddressOf());

		context->PSSetSamplers(
			0,
			1,
			TModelTextureSampleState.GetAddressOf());


		context->RSSetState(CWcullMode);
		context->DrawIndexed(
			m_SBindexCount,
			0,
			0
		);

		//context->RSSetState(CCWcullMode);
		//context->DrawIndexed(
		//	m_SBindexCount,
		//	0,
		//	0
		//);
#endif
#pragma endregion

#pragma region Tree TWO
#if 1
		cWorld2 = XMMatrixTranslation(0.0f, 2.0f, 2.5f);
		XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixScaling(0.1f, 0.1f, 0.1f) * cWorld2));

		context->UpdateSubresource1(
			m_SBconstantBuffer.Get(),
			0,
			NULL,
			&m_constantBufferData,
			0,
			0,
			0
		);

		// Each vertex is one instance of the VertexPositionColor struct.
		context->IASetVertexBuffers(
			0,
			1,
			m_SBvertexBuffer.GetAddressOf(),
			&stride,
			&offset
		);

		context->IASetIndexBuffer(
			m_SBindexBuffer.Get(),
			DXGI_FORMAT_R32_UINT, // Each index is one 16-bit unsigned integer (short).
			0
		);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->IASetInputLayout(m_inputLayout.Get());

		// Attach our vertex shader.
		context->VSSetShader(
			m_vertexShader.Get(),
			nullptr,
			0
		);

		// Send the constant buffer to the graphics device.
		context->VSSetConstantBuffers1(
			0,
			1,
			m_SBconstantBuffer.GetAddressOf(),
			nullptr,
			nullptr
		);

		// Attach our pixel shader.
		context->PSSetShader(
			m_pixelShader.Get(),
			nullptr,
			0
		);

		context->PSSetShaderResources(
			0,
			1,
			TModelTexture.GetAddressOf());

		context->PSSetSamplers(
			0,
			1,
			TModelTextureSampleState.GetAddressOf());

		//context->RSSetState(CWcullMode);
		//context->DrawIndexed(
		//	m_SBindexCount,
		//	0,
		//	0
		//);

		context->RSSetState(CCWcullMode);
		context->DrawIndexed(
			m_SBindexCount,
			0,
			0
		);
#endif
#pragma endregion

#pragma region Tree THREE
#if 1
		cWorld3 = XMMatrixTranslation(2.0f, 2.0f, 2.5f);
		XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixScaling(0.1f, 0.1f, 0.1f) * cWorld3));

		context->UpdateSubresource1(
			m_SBconstantBuffer.Get(),
			0,
			NULL,
			&m_constantBufferData,
			0,
			0,
			0
		);

		// Each vertex is one instance of the VertexPositionColor struct.
		context->IASetVertexBuffers(
			0,
			1,
			m_SBvertexBuffer.GetAddressOf(),
			&stride,
			&offset
		);

		context->IASetIndexBuffer(
			m_SBindexBuffer.Get(),
			DXGI_FORMAT_R32_UINT, // Each index is one 16-bit unsigned integer (short).
			0
		);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->IASetInputLayout(m_inputLayout.Get());

		// Attach our vertex shader.
		context->VSSetShader(
			m_vertexShader.Get(),
			nullptr,
			0
		);

		// Send the constant buffer to the graphics device.
		context->VSSetConstantBuffers1(
			0,
			1,
			m_SBconstantBuffer.GetAddressOf(),
			nullptr,
			nullptr
		);

		// Attach our pixel shader.
		context->PSSetShader(
			m_pixelShader.Get(),
			nullptr,
			0
		);

		context->PSSetShaderResources(
			0,
			1,
			TModelTexture.GetAddressOf());

		context->PSSetSamplers(
			0,
			1,
			TModelTextureSampleState.GetAddressOf());

		context->RSSetState(CWcullMode);
		context->DrawIndexed(
			m_SBindexCount,
			0,
			0
		);
		//context->RSSetState(CCWcullMode);
		//context->DrawIndexed(
		//	m_SBindexCount,
		//	0,
		//	0
		//);
#endif
#pragma endregion
}

if (reversed == true)
{
#pragma region Tree THREE
#if 1
	cWorld3 = XMMatrixTranslation(2.0f, 2.0f, 2.5f);
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixScaling(0.1f, 0.1f, 0.1f) * cWorld3));

	context->UpdateSubresource1(
		m_SBconstantBuffer.Get(),
		0,
		NULL,
		&m_constantBufferData,
		0,
		0,
		0
	);

	// Each vertex is one instance of the VertexPositionColor struct.
	context->IASetVertexBuffers(
		0,
		1,
		m_SBvertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetIndexBuffer(
		m_SBindexBuffer.Get(),
		DXGI_FORMAT_R32_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_SBconstantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);

	context->PSSetShaderResources(
		0,
		1,
		TModelTexture.GetAddressOf());

	context->PSSetSamplers(
		0,
		1,
		TModelTextureSampleState.GetAddressOf());

	context->RSSetState(CWcullMode);
	context->DrawIndexed(
		m_SBindexCount,
		0,
		0
	);
	//context->RSSetState(CCWcullMode);
	//context->DrawIndexed(
	//	m_SBindexCount,
	//	0,
	//	0
	//);
#endif
#pragma endregion

#pragma region Tree TWO
#if 1
	cWorld2 = XMMatrixTranslation(0.0f, 2.0f, 2.5f);
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixScaling(0.1f, 0.1f, 0.1f) * cWorld2));

	context->UpdateSubresource1(
		m_SBconstantBuffer.Get(),
		0,
		NULL,
		&m_constantBufferData,
		0,
		0,
		0
	);

	// Each vertex is one instance of the VertexPositionColor struct.
	context->IASetVertexBuffers(
		0,
		1,
		m_SBvertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetIndexBuffer(
		m_SBindexBuffer.Get(),
		DXGI_FORMAT_R32_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_SBconstantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);

	context->PSSetShaderResources(
		0,
		1,
		TModelTexture.GetAddressOf());

	context->PSSetSamplers(
		0,
		1,
		TModelTextureSampleState.GetAddressOf());

	//context->RSSetState(CWcullMode);
	//context->DrawIndexed(
	//	m_SBindexCount,
	//	0,
	//	0
	//);

	context->RSSetState(CCWcullMode);
	context->DrawIndexed(
		m_SBindexCount,
		0,
		0
	);
#endif
#pragma endregion

#pragma region Tree ONE
#if 1
	cWorld1 = XMMatrixTranslation(-2.0f, 2.0f, 2.5f);
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixScaling(0.1f, 0.1f, 0.1f) * cWorld1));

	context->UpdateSubresource1(
		m_SBconstantBuffer.Get(),
		0,
		NULL,
		&m_constantBufferData,
		0,
		0,
		0
	);

	// Each vertex is one instance of the VertexPositionColor struct.
	context->IASetVertexBuffers(
		0,
		1,
		m_SBvertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetIndexBuffer(
		m_SBindexBuffer.Get(),
		DXGI_FORMAT_R32_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_SBconstantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);

	context->PSSetShaderResources(
		0,
		1,
		TModelTexture.GetAddressOf());

	context->PSSetSamplers(
		0,
		1,
		TModelTextureSampleState.GetAddressOf());


	context->RSSetState(CWcullMode);
	context->DrawIndexed(
		m_SBindexCount,
		0,
		0
	);

	//context->RSSetState(CCWcullMode);
	//context->DrawIndexed(
	//	m_SBindexCount,
	//	0,
	//	0
	//);

#endif
#pragma endregion
}

}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"SampleVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"SamplePixelShader.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_vertexShader
				)
			);

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc [] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMALS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			//{ "INST_POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
		};
		
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_inputLayout
				)
			);
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShader
				)
			);

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer) , D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_constantBuffer
				)
			);

		CD3D11_BUFFER_DESC PSconstantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&PSconstantBufferDesc,
				nullptr,
				&m_SPconstantBuffer
			)
		);

		CD3D11_BUFFER_DESC FconstantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&FconstantBufferDesc,
				nullptr,
				&m_FconstantBuffer
			)
		);

		CD3D11_BUFFER_DESC SBconstantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&SBconstantBufferDesc,
				nullptr,
				&m_SBconstantBuffer
			)
		);

		CD3D11_BUFFER_DESC SkyconstantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&SkyconstantBufferDesc,
				nullptr,
				&m_SkyconstantBuffer
			)
		);
	});
	
	// Once both shaders are loaded, create the mesh.
	auto createSkyBoxTask = (createPSTask && createVSTask).then([this]() {

#pragma region SKYBOX
#if 0
		vector<VertexPositionColor> Skyverts;
		vector<unsigned int> Skyindices;
		ModelLoader("Tree.obj", Skyverts, Skyindices);

		D3D11_SUBRESOURCE_DATA spherevertexBufferData = { 0 };
		spherevertexBufferData.pSysMem = Skyverts.data();
		spherevertexBufferData.SysMemPitch = 0;
		spherevertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC spherevertexBufferDesc(Skyverts.size() * sizeof(VertexPositionColor), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&spherevertexBufferDesc,
				&spherevertexBufferData,
				&SkyBox_VB
			)
		);

		m_SkyindexCount = Skyindices.size();

		D3D11_SUBRESOURCE_DATA SkyindexBufferData = { 0 };
		SkyindexBufferData.pSysMem = Skyindices.data();
		SkyindexBufferData.SysMemPitch = 0;
		SkyindexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC SkyindexBufferDesc(Skyindices.size() * sizeof(unsigned int), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&SkyindexBufferDesc,
				&SkyindexBufferData,
				&SkyBox_IB
			)
		);

	/*	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"T_Tree_D.dds", NULL, &TModelTexture, NULL);

		D3D11_SAMPLER_DESC TsampDesc;
		ZeroMemory(&TsampDesc, sizeof(TsampDesc));
		TsampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		TsampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		TsampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		TsampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		TsampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		TsampDesc.MinLOD = -D3D11_FLOAT32_MAX;
		TsampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateSamplerState(
				&TsampDesc,
				&TModelTextureSampleState
			)
		);*/
#endif
#pragma endregion
	});

	auto createTreeTask = (createPSTask && createVSTask).then([this] () {

#pragma region TREE
		vector<VertexPositionColor> SBverts;
		vector<unsigned int> SBindices;
		ModelLoader("Tree.obj", SBverts, SBindices);

		D3D11_SUBRESOURCE_DATA SBvertexBufferData = { 0 };
		SBvertexBufferData.pSysMem = SBverts.data();
		SBvertexBufferData.SysMemPitch = 0;
		SBvertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC SBvertexBufferDesc(SBverts.size() * sizeof(VertexPositionColor), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&SBvertexBufferDesc,
				&SBvertexBufferData,
				&m_SBvertexBuffer
			)
		);

		m_SBindexCount = SBindices.size();

		D3D11_SUBRESOURCE_DATA SBindexBufferData = { 0 };
		SBindexBufferData.pSysMem = SBindices.data();
		SBindexBufferData.SysMemPitch = 0;
		SBindexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC SBindexBufferDesc(SBindices.size() * sizeof(unsigned int), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&SBindexBufferDesc,
				&SBindexBufferData,
				&m_SBindexBuffer
			)
		);

		CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"T_Tree_D.dds", NULL, &TModelTexture, NULL);

		D3D11_SAMPLER_DESC TsampDesc;
		ZeroMemory(&TsampDesc, sizeof(TsampDesc));
		TsampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		TsampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		TsampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		TsampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		TsampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		TsampDesc.MinLOD = -D3D11_FLOAT32_MAX;
		TsampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateSamplerState(
				&TsampDesc,
				&TModelTextureSampleState
			)
		);
#pragma endregion
});

	auto createGumbaTask = (createPSTask && createVSTask).then([this]() {

#pragma region GUMBAMODEL
		vector<VertexPositionColor> modelverts;
		vector<unsigned int> modelindices;
		ModelLoader("fuzzy.obj", modelverts, modelindices);

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = modelverts.data();
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(modelverts.size() * sizeof(VertexPositionColor), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				&m_vertexBuffer
			)
		);

		m_indexCount = modelindices.size();

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = modelindices.data();
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(modelindices.size() * sizeof(unsigned int), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc,
				&indexBufferData,
				&m_indexBuffer
			)
		);

		CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Diffuse_Fuzzy.dds", NULL, &ModelTexture, NULL);

		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = -D3D11_FLOAT32_MAX;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateSamplerState(
				&sampDesc,
				&ModelTextureSampleState
			)
		);
#pragma endregion
});

	auto createLightBulbTask = (createPSTask && createVSTask).then([this]() {

#pragma region LIGHTBULBMODEL
		vector<VertexPositionColor> SPverts;
		vector<unsigned int> SPindices;
		ModelLoader("SpotLight.obj", SPverts, SPindices);

		D3D11_SUBRESOURCE_DATA SPvertexBufferData = { 0 };
		SPvertexBufferData.pSysMem = SPverts.data();
		SPvertexBufferData.SysMemPitch = 0;
		SPvertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC SPvertexBufferDesc(SPverts.size() * sizeof(VertexPositionColor), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&SPvertexBufferDesc,
				&SPvertexBufferData,
				&m_SPvertexBuffer
			)
		);

		m_SPindexCount = SPindices.size();

		D3D11_SUBRESOURCE_DATA SPindexBufferData = { 0 };
		SPindexBufferData.pSysMem = SPindices.data();
		SPindexBufferData.SysMemPitch = 0;
		SPindexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC SPindexBufferDesc(SPverts.size() * sizeof(unsigned int), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&SPindexBufferDesc,
				&SPindexBufferData,
				&m_SPindexBuffer
			)
		);

		HRESULT SPhr = CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"AlarmLight_D.dds", NULL, &SPModelTexture, NULL);

		D3D11_SAMPLER_DESC SPsampDesc;
		ZeroMemory(&SPsampDesc, sizeof(SPsampDesc));
		SPsampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SPsampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SPsampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SPsampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		SPsampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		SPsampDesc.MinLOD = -D3D11_FLOAT32_MAX;
		SPsampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateSamplerState(
				&SPsampDesc,
				&SPModelTextureSampleState
			)
		);
#pragma endregion		
});

	auto createFloorTask = (createPSTask && createVSTask).then([this]() {

#pragma region FLOOR
		vector<VertexPositionColor> Fverts;
		vector<unsigned int> Findices;
		ModelLoader("ground.obj", Fverts, Findices);

		D3D11_SUBRESOURCE_DATA FvertexBufferData = { 0 };
		FvertexBufferData.pSysMem = Fverts.data();
		FvertexBufferData.SysMemPitch = 0;
		FvertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC FvertexBufferDesc(Fverts.size() * sizeof(VertexPositionColor), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&FvertexBufferDesc,
				&FvertexBufferData,
				&m_FvertexBuffer
			)
		);

		m_FindexCount = Findices.size();

		D3D11_SUBRESOURCE_DATA FindexBufferData = { 0 };
		FindexBufferData.pSysMem = Findices.data();
		FindexBufferData.SysMemPitch = 0;
		FindexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC FindexBufferDesc(Fverts.size() * sizeof(unsigned int), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&FindexBufferDesc,
				&FindexBufferData,
				&m_FindexBuffer
			)
		);

		HRESULT Fhr = CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"wall.dds", NULL, &FModelTexture, NULL);

		D3D11_SAMPLER_DESC FsampDesc;
		ZeroMemory(&FsampDesc, sizeof(FsampDesc));
		FsampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		FsampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		FsampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		FsampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		FsampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		FsampDesc.MinLOD = -D3D11_FLOAT32_MAX;
		FsampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		DX::ThrowIfFailed(
			Fhr = m_deviceResources->GetD3DDevice()->CreateSamplerState(
				&FsampDesc,
				&FModelTextureSampleState
			)
		);
#pragma endregion
});

	auto createSetupTask = (createPSTask && createVSTask).then([this]() {

#pragma region BLENDSETUP
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));

		D3D11_RENDER_TARGET_BLEND_DESC rtbDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));

		rtbDesc.BlendEnable = true;
		rtbDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rtbDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		rtbDesc.BlendOp = D3D11_BLEND_OP_ADD;
		rtbDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
		rtbDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
		rtbDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rtbDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.RenderTarget[0] = rtbDesc;
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBlendState(
				&blendDesc,
				&Transparency
			)
		);

		D3D11_RASTERIZER_DESC rasterDesc;
		ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
		//ADDED for MSAA
		rasterDesc.AntialiasedLineEnable = true;
		rasterDesc.MultisampleEnable = true;
		//////////////////////////////////////
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.CullMode = D3D11_CULL_BACK;

		rasterDesc.FrontCounterClockwise = true;
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateRasterizerState(
				&rasterDesc,
				&CCWcullMode
			)
		);

		rasterDesc.FrontCounterClockwise = false;
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateRasterizerState(
				&rasterDesc,
				&CCWcullMode
			)
		);
#pragma endregion
#pragma region LIGHTSETUP
		///////////////LIGHTS//////////////////
		dirLight.ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		dirLight.diffuse = XMFLOAT4(0.2f, 0.2f, 0.4f, 1.0f);
		dirLight.dir = XMFLOAT3(0.707f, -0.707f, 0.0f);

		light.pos = XMFLOAT3(0.0f, 1.5f, 0.0f);
		light.range = 100.0f;
		light.dir = XMFLOAT3(0.707f, -0.707f, 0.0f);
		light.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
		light.diffuse = XMFLOAT4(2.0f, 0.2f, 0.2f, 1.0f);

		_spotlight.pos = XMFLOAT3(0.0f, 1.0f, 0.0f);
		_spotlight.dir = XMFLOAT3(0.0f, 0.0f, 1.0f);
		_spotlight.range = 1000.0f;
		_spotlight.cone = 20.0f;
		_spotlight.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
		_spotlight.diffuse = XMFLOAT4(0.2f, 1.5f, 0.5f, 1.0f);

		D3D11_BUFFER_DESC pfBuffer;
		ZeroMemory(&pfBuffer, sizeof(D3D11_BUFFER_DESC));
		pfBuffer.Usage = D3D11_USAGE_DEFAULT;
		pfBuffer.ByteWidth = sizeof(ModelViewProjectionConstantBuffer);
		pfBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		pfBuffer.CPUAccessFlags = 0;
		pfBuffer.MiscFlags = 0;

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&pfBuffer,
				NULL,
				&m_constantBuffer
			)
		);

		ZeroMemory(&pfBuffer, sizeof(D3D11_BUFFER_DESC));
		pfBuffer.Usage = D3D11_USAGE_DEFAULT;
		pfBuffer.ByteWidth = sizeof(perFrame);
		pfBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		pfBuffer.CPUAccessFlags = 0;
		pfBuffer.MiscFlags = 0;

		HRESULT test;
		DX::ThrowIfFailed(
			test = m_deviceResources->GetD3DDevice()->CreateBuffer(
				&pfBuffer,
				NULL,
				&m_perframeBuffer
			)
		);
#pragma endregion

#pragma region RENDERTOTEXTURESETUP
#if 1
		D3D11_TEXTURE2D_DESC texDesc;
		D3D11_RENDER_TARGET_VIEW_DESC rtViewDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC srViewDesc;

		//Render to texture setup//
		ZeroMemory(&texDesc, sizeof(texDesc));
		texDesc.Width = 1511;
		texDesc.Height = 900;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.SampleDesc.Count = 1;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateTexture2D(
				&texDesc,
				NULL,
				&m_rtTextureMap
			)
		);

		ZeroMemory(&texDesc, sizeof(texDesc));
		texDesc.Width = 1511;
		texDesc.Height = 900;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texDesc.SampleDesc.Count = 1;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateTexture2D(
				&texDesc,
				NULL,
				&m_dsTextureMap
			)
		);
		///////////////////////////////////////
		CD3D11_TEXTURE2D_DESC1 depthStencilDesc(
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			1024,
			1024,
			1, // This depth stencil view has only one texture.
			1, // Use a single mipmap level.
			D3D11_BIND_DEPTH_STENCIL
		);

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateDepthStencilView(
				m_dsTextureMap,
				NULL,
				&m_dsViewMap
			)
		);

		rtViewDesc.Format = texDesc.Format;
		rtViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtViewDesc.Texture2D.MipSlice = 0;

		HRESULT check;
		DX::ThrowIfFailed(
			check = m_deviceResources->GetD3DDevice()->CreateRenderTargetView(
				m_rtTextureMap,
				NULL,
				&m_rtViewMap
			)
		);

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateShaderResourceView(
				m_rtTextureMap,
				NULL,
				&m_srvTextureMap
			)
		);
#endif
#pragma endregion

});

	// Once the cube is loaded, the object is ready to be rendered.
	createSkyBoxTask.then([this]() {
		m_loadingComplete = true;
	});

	createTreeTask.then([this] () {
		m_loadingComplete = true;
	});

	createGumbaTask.then([this]() {
		m_loadingComplete = true;
	});

	createLightBulbTask.then([this]() {
		m_loadingComplete = true;
	});

	createFloorTask.then([this]() {
		m_loadingComplete = true;
	});

	createSetupTask.then([this]() {
		m_loadingComplete = true;
	});
}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_constantBuffer.Reset();
	m_SPconstantBuffer.Reset();
	m_FconstantBuffer.Reset();
	m_SBconstantBuffer;
	m_vertexBuffer.Reset();
	m_SBvertexBuffer.Reset();
	m_FvertexBuffer.Reset();
	m_SPvertexBuffer.Reset();
	m_indexBuffer.Reset();
	m_SBindexBuffer.Reset();
	m_SPindexBuffer.Reset();
	m_FindexBuffer.Reset();
	ModelTexture.Reset();
	FModelTexture.Reset();
	SPModelTexture.Reset();
	ModelTextureSampleState.Reset();
	SPModelTextureSampleState.Reset();
	FModelTextureSampleState.Reset();
	Transparency->Release();
	CCWcullMode->Release();
	CWcullMode->Release();
	m_perframeBuffer->Release();
	m_SBindexBuffer.Reset();
	m_SBvertexBuffer.Reset();
	SKY_VS.Reset();
	SKY_PS.Reset();
	RS_CullNone.Reset();
	m_rtTextureMap->Release();
	m_rtViewMap->Release();
	m_srvTextureMap.Reset();
	m_dsTextureMap->Release();
	m_dsViewMap->Release();
	SKY_VS_Buffer->Release();
	SKY_PS_Buffer->Release();
	SkyBox_IB.Reset();
	SkyBox_VB.Reset();
	SkyBoxSRV->Release();
	SkyBox_DSS->Release();
}