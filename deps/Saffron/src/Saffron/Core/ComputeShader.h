#pragma once

#include "Saffron/Base.h"

namespace Se
{
class ComputeShader
{
public:
	void Bind();
	void Unbind();

	void Dispatch(Uint32 xGroup, Uint32 yGroup, Uint32 zGroup);

	bool LoadFromFile(const Filepath& filepath);

	void SetFloat(const String& name, float value);
	void SetInt(const String& name, int value);;
	void SetVec4(const String& name, const sf::Vector4f& value);

	static void AwaitFinish();

private:
	void BindThenUnbind(const Function<void()>& fn);

private:
	Uint32 _id{};
};
}
