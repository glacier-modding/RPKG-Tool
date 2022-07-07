#include "gpudevice.h"
#include "global.h"

gpudevice::gpudevice()
{

}

ID3D11Device* gpudevice::get_device_d3d11()
{
    if (!device) {
        constexpr D3D_FEATURE_LEVEL d3d_feature_level = D3D_FEATURE_LEVEL_11_0;

        const HRESULT hresult = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, NULL, &d3d_feature_level, 1, D3D11_SDK_VERSION, &device, nullptr, &context);

        if (FAILED(hresult))
        {
            LOG("Error: Failed to initialize DirectX 11 GPU Device.");
        }
    }
    return device;
}
