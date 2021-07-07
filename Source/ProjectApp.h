#pragma once

#include <Saffron.h>

#include "Layers/ProjectLayer.h"

namespace Se
{
class ProjectApp : public App
{
public:
	explicit ProjectApp(const AppProperties& properties);

	void OnInit() override;

	void OnUpdate() override;

private:
	Shared<ProjectLayer> _projectLayer;
};
}