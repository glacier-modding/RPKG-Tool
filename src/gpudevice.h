#pragma once
#include <d3d11.h>
#pragma comment(lib,"d3d11.lib")

class gpudevice
{
public:
	gpudevice();

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	ID3D11Device* get_device_d3d11();
};