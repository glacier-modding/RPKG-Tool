#include "gpudevice.h"
#include <iostream>

gpudevice::gpudevice()
{

}

ID3D11Device* gpudevice::get_device_d3d11()
{
    if (!device) {
        D3D_FEATURE_LEVEL d3d_feature_level = D3D_FEATURE_LEVEL_11_0;

        HRESULT hresult = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &d3d_feature_level, 1, D3D11_SDK_VERSION, &device, NULL, &context);

        if (FAILED(hresult))
        {
            LOG("Error: Failed to initialize DirectX 11 GPU Device.");
        }
    }
    return device;
}
