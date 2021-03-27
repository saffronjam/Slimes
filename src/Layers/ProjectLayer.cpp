﻿#include "ProjectLayer.h"

namespace Se
{
void ProjectLayer::OnAttach(std::shared_ptr<BatchLoader> &loader)
{
	BaseLayer::OnAttach(loader);
	loader->Submit([this]
		{
		}, "Create Simulation Manager");
	_simMgr = CreateShared<SimulationManager>();
}

void ProjectLayer::OnDetach()
{
	BaseLayer::OnDetach();
}

void ProjectLayer::OnUpdate()
{
	BaseLayer::OnUpdate();
	_simMgr->OnUpdate(_scene);
	_simMgr->OnRender(_scene);
}

void ProjectLayer::OnGuiRender()
{
	BaseLayer::OnGuiRender();

	if ( ImGui::BeginMenuBar() )
	{
		if ( ImGui::BeginMenu("Gui") )
		{
			ImGui::MenuItem("View System", nullptr, &_viewSystem);
			ImGui::MenuItem("View Demo", nullptr, &_viewDemo);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if ( ImGui::Begin("Project") )
	{
	}
	ImGui::End();

	if ( _viewDemo )
	{
		ImGui::ShowDemoWindow();
	}
}

void ProjectLayer::OnRenderTargetResize(const sf::Vector2f &newSize)
{
	BaseLayer::OnRenderTargetResize(newSize);
	_scene.OnRenderTargetResize(newSize);
}
}
