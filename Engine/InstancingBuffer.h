#pragma once
struct InstancingParams
{
	Vector4 Color;
	Vector4 Emission;

	Matrix matW;
	Matrix matWVP;

	Vector2 tile;
	Vector2 offset;
};

class InstancingBuffer
{
public:
	InstancingBuffer();
	~InstancingBuffer();

	void Init(UINT maxCount = 10);

	void Clear();
	void AddData(InstancingParams& params);
	void PushData();

public:
	UINT						GetCount() { return static_cast<UINT>(_data.size()); }
	ID3D11Buffer*		GetBuffer() { return _buffer.Get(); }

	void	SetID(UINT64 instanceId) { _instanceId = instanceId; }
	UINT64	GetID() { return _instanceId; }

private:
	UINT64						_instanceId = 0;
	ComPtr<ID3D11Buffer>		_buffer;

	UINT						_maxCount = 0;
	vector<InstancingParams>	_data;
};