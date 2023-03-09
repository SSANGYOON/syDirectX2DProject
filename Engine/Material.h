#pragma once
#include "Resource.h"
class Shader;
class Texture;
class Material : public Resource
{
public :
	Material();
	virtual ~Material();

public:
	virtual HRESULT Load(const std::wstring& path) override;

	void SetShader(shared_ptr<Shader> shader) { _shader = shader; };
	void SetTexture(shared_ptr<Texture> texture) { _texture = texture; };
	void Bind();

private:
	shared_ptr<Shader> _shader;
	shared_ptr<Texture> _texture;
};

