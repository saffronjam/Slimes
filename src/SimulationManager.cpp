#include "SaffronPCH.h"

//#include <SFML/OpenGL.hpp>

#include "SimulationManager.h"

namespace Se
{
SimulationManager::SimulationManager() :
	_agents(50000)
{
	for (auto& agent : _agents)
	{
		agent.Position = sf::Vector2f(_texWidth, _texHeight) / 2.0f;
		agent.Angle = Random::Real() * Math::PI * 2.0f;
	}


	_drawProgram = CreateComputeProgram("res/shaders/draw.comp");
	_blendEvapProgram = CreateComputeProgram("res/shaders/blendEvap.comp");

	GLuint tex_output;
	glGenTextures(1, &tex_output);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_output);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _texWidth, _texHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	struct Ssbo_data
	{
		Agent agents[50000];
	};

	auto* ssbo_data = new Ssbo_data();

	std::memcpy(&ssbo_data->agents, _agents.data(), _agents.size() * sizeof(Agent));


	glGenBuffers(1, &_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Ssbo_data), ssbo_data, GL_DYNAMIC_DRAW);
	//sizeof(data) only works for statically sized C/C++ arrays.
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, _ssbo);


	delete ssbo_data;
}

void SimulationManager::OnUpdate(Scene& scene)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texOutput);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _texWidth, _texHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(0, _texOutput, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, _ssbo);

	{
		// launch compute shaders!
		glUseProgram(_drawProgram);
		glDispatchCompute((GLuint)_agents.size(), 1, 1);
	}
	{
		// launch compute shaders!
		glUseProgram(_blendEvapProgram);
		glDispatchCompute(_texWidth, _texHeight, 1);
	}

	// make sure writing to image has finished before read
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


	auto* pixels = new sf::Color[_texWidth * _texHeight];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	glGetBufferSubData(GL_TEXTURE_2D, 0, _texWidth * _texHeight * sizeof(sf::Color), pixels);

	sf::Image image;
	image.create(_texWidth, _texHeight, reinterpret_cast<sf::Uint8*>(pixels));

	_tex.loadFromImage(image);

	delete[] pixels;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

	Application::Get().GetWindow().GetNativeWindow().resetGLStates();
}

void SimulationManager::OnRender(Scene& scene)
{
	sf::Sprite sprite(_tex);
	sprite.setPosition(-_texWidth / 2.0f, -_texHeight / 2.0f);
	scene.Submit(sprite);
}

GLuint SimulationManager::CreateComputeProgram(const char* filepath)
{
	std::string content;
	std::ifstream fileStream(filepath, std::ios::in);

	if (!fileStream.is_open())
	{
		std::cerr << "Could not read file " << filepath << ". File does not exist." << std::endl;
	}

	std::string line;
	while (!fileStream.eof())
	{
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();

	const auto shader = glCreateShader(GL_COMPUTE_SHADER);
	const auto* contentCStr = content.c_str();
	glShaderSource(shader, 1, &contentCStr, nullptr);
	glCompileShader(shader);
	// check for compilation errors as per normal here

	const auto program = glCreateProgram();
	glAttachShader(program, shader);
	glLinkProgram(program);

	return program;
}
}
