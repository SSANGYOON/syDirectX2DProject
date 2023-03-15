#pragma once
class Device
{
public:
	Device() = default;
	~Device();
	void Init();

	ComPtr<ID3D11Device> GetDevice() { return _device; }
	ComPtr<ID3D11DeviceContext> GetContext() { return _context; }

private:
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11Debug> _debug;
	ComPtr<ID3D11DeviceContext> _context;
};

