#include "pch.h"
#include "Device.h"

Device::~Device()
{
	_debug->ReportLiveDeviceObjects(D3D11_RLDO_FLAGS::D3D11_RLDO_SUMMARY);
}

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

	_device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(_debug.GetAddressOf()));
	_debug->ReportLiveDeviceObjects(D3D11_RLDO_FLAGS::D3D11_RLDO_DETAIL);
}
