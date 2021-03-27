#pragma once

#include <SFML/Graphics.hpp>
#include <glad/glad.h>
#include <Saffron.h>

#include "Agent.h"

namespace Se
{
class SimulationManager
{
public:
	SimulationManager();

	void OnUpdate(Scene& scene);
	void OnRender(Scene& scene);

private:
	static GLuint CreateComputeProgram(const char *filepath);

private:
	sf::Image _simulationImage;
	ArrayList<Agent> _agents;

	sf::Texture _tex;

	int _noAgents = 1000;
	float _bodySpeed = 150.0f;
	float _decayMult = 0.9998;

	GLuint _drawProgram;
	GLuint _blendEvapProgram;
	GLuint _texOutput;
	GLuint _ssbo;
	
	static constexpr int _texWidth = 1024, _texHeight = 512;
};
}
