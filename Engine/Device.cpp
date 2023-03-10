#include "pch.h"
#include "Device.h"

void Device::Init()
{
	UINT DeviceFlag = D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL FeatureLevel = (D3D_FEATURE_LEVEL)0;

	HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr
		, DeviceFlag, nullptr, 0
		, D3D11_SDK_VERSION
		, _device.GetAddressOf()
		, &FeatureLevel
		, _context.GetAddressOf());
}
