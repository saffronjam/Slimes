#pragma once

#include <SFML/Graphics.hpp>
#include <glad/glad.h>
#include <Saffron.h>

#include "Agent.h"

namespace Se
{
class SimulationManager
{
	enum class ShapeType
	{
		Circle,
		Square,
		Random
	};

	enum class AngleType
	{
		CenterIn,
		CenterOut,
		Random
	};

	enum class PaletteType
	{
		Slime,
		Fiery,
		Greyscale,
		Rainbow,
		UV,
		Count
	};

	enum class QualityType
	{
		Low,
		Medium,
		High
	};

	enum class StateType
	{
		Paused,
		Running
	};

public:
	explicit SimulationManager(QualityType initialQuality = QualityType::Medium);

	void OnUpdate(Scene& scene);
	void OnRender(Scene& scene);
	void OnGuiRender();

private:
	void UpdatePaletteTransition();
	void UpdatePaletteData();

	Function<sf::Vector2f(int)> GetPositionGenerator(ShapeType shapeType);
	Function<float(int)> GetAngleGenerator(AngleType angleType);

	void SetShape(const Function<sf::Vector2f(int)>& generator);
	void SetAngles(const Function<float(int)>& generator);
	void SetPalette(PaletteType desired);
	void SetQuality(QualityType quality);
	void SetTexSize(Uint32 width, Uint32 height);
	void SetAgentDimensionSize(Uint32 size);

	void Reset(ShapeType shapeType, AngleType angleType);
	void Transition(ShapeType shapeTypeTo, AngleType angleTypeTo);

	void RunDrawFrame();

private:
	static constexpr Uint32 _paletteWidth = 256;

	StateType _stateType = StateType::Paused;
	
	Uint32 _agentDim;
	Uint32 _texWidth;
	Uint32 _texHeight;

	ArrayList<Agent> _agentBuffer;

	sf::Image _simulationImage;

	sf::Texture _outputTexture;
	sf::Texture _dataTexture;

	Shared<ComputeShader> _drawCS;
	Shared<ComputeShader> _blendEvapCS;
	Shared<ComputeShader> _painterCS;
	GLuint _ssbo;

	ShapeType _shapeType = ShapeType::Circle;
	AngleType _angleType = AngleType::CenterIn;

	Array<Shared<sf::Image>, static_cast<size_t>(PaletteType::Count)> _paletteImages;
	Array<sf::Texture, static_cast<size_t>(PaletteType::Count)> _palettes;
	sf::Image _currentPaletteImage;
	sf::Texture _currentPaletteTexture;
	PaletteType _desiredPalette = PaletteType::Slime;
	Array<sf::Vector4f, _paletteWidth> _colorsStart;
	Array<sf::Vector4f, _paletteWidth> _colorsCurrent;

	sf::Time _colorTransitionTimer;
	sf::Time _colorTransitionDuration = sf::seconds(0.3f);
	bool _inTransition = false;

	// Gui cache
	float _movementSpeed = 250.0f;
	float _trailAttraction = 250.0f;
	float _diffuseSpeed = 5.0f;
	float _evaporateSpeed = 6.5f;
	float _colorScale = 5.0f;

	ArrayList<const char*> _shapeTypeNames;
	ArrayList<const char*> _angleTypeNames;
	ArrayList<const char*> _paletteTypeNames;
	ArrayList<const char*> _qualityTypeNames;
	int _shapeTypeIndex = static_cast<int>(_shapeType);
	int _angleTypeIndex = static_cast<int>(_angleType);
	int _paletteTypeIndex = static_cast<int>(_desiredPalette);
	int _qualityTypeIndex;
};
}
