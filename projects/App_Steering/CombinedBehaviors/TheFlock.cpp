#include "stdafx.h"
#include "TheFlock.h"

#include "../SteeringAgent.h"
#include "../SteeringBehaviors.h"
#include "CombinedSteeringBehaviors.h"

using namespace Elite;

//Constructor & Destructor
Flock::Flock(
	int flockSize /*= 50*/, 
	float worldSize /*= 100.f*/, 
	SteeringAgent* pAgentToEvade /*= nullptr*/, 
	bool trimWorld /*= false*/)

	: m_CellSpace{ worldSize, worldSize, 20, 20, flockSize - 1}
	, m_WorldSize{ worldSize }
	, m_FlockSize{ flockSize }
	, m_TrimWorld { trimWorld }
	, m_pAgentToEvade{pAgentToEvade}
	, m_NeighborhoodRadius{ 15 }
	, m_NrOfNeighbors{0}
{
	m_Agents.resize(m_FlockSize);

	m_pWanderEvader = new Wander();

	m_pAgentToEvade = new SteeringAgent{};
	m_pAgentToEvade->SetSteeringBehavior(m_pWanderEvader);
	m_pAgentToEvade->SetBodyColor({ 1.f,0.f,0.f });

	m_pWander = new Wander();
	m_pWander->SetWanderOffset(0);

	m_pSeek = new Seek();
	
	m_pAllignment = new Allignment();
	m_pAllignment->SetFlock(this);

	m_pCohesion = new Cohesion();
	m_pCohesion->SetFlock(this);

	m_pSeperation = new Seperation();
	m_pSeperation->SetFlock(this);

	m_pEvade = new Evade();
	m_pEvade->SetEvasionRadius(m_EvasionRadius);
	
	m_pBlendedSteering = new BlendedSteering({ { m_pCohesion, 0.5f }, {m_pSeperation, 0.5f}, { m_pAllignment, 0.5f }, { m_pSeek, 0.5f }, { m_pWander, 0.5f } });
	m_pPrioritySteering = new PrioritySteering({ {m_pEvade}, { m_pBlendedSteering } });

	m_Neighbors.resize(m_FlockSize);

	for (int i{}; i < m_FlockSize; ++i)
	{
		m_Agents[i] = new SteeringAgent();
		m_Agents[i]->SetSteeringBehavior(m_pPrioritySteering);
		m_Agents[i]->SetAutoOrient(true);
		m_Agents[i]->SetMaxLinearSpeed(150.f);
		m_Agents[i]->SetMass(1.f);
		m_Agents[i]->SetBodyColor({ 1.f,1.f,0.f });
		Elite::Vector2 pos = { randomFloat(-m_WorldSize, m_WorldSize), randomFloat(-m_WorldSize, m_WorldSize) };
		m_Agents[i]->SetPosition(pos);
		m_CellSpace.AddAgent(m_Agents[i]);
		m_OldPositions[m_Agents[i]] = pos;
	}
}

Flock::~Flock()
{
	for (int i{}; i < m_FlockSize; i++)
	{
		SAFE_DELETE(m_Agents[i]);
	}
	m_Agents.clear();
	SAFE_DELETE(m_pAgentToEvade);
	SAFE_DELETE(m_pWanderEvader);

	SAFE_DELETE(m_pBlendedSteering);
	SAFE_DELETE(m_pPrioritySteering);
	SAFE_DELETE(m_pEvade);
	SAFE_DELETE(m_pCohesion);
	SAFE_DELETE(m_pSeperation);
	SAFE_DELETE(m_pAllignment);
	SAFE_DELETE(m_pSeek);
	SAFE_DELETE(m_pWander);

}

void Flock::Update(float deltaT)
{
	// loop over all the boids
	// register its neighbors
	// update it
	// trim it to the world
	m_pAgentToEvade->Update(deltaT);
	m_pAgentToEvade->TrimToWorld(m_WorldSize);
	m_pEvade->SetTarget(m_pAgentToEvade->GetPosition());
	m_pSeek->SetTarget(m_MousePos);
	m_NrOfNeighbors = m_CellSpace.GetNrOfNeighbors();
	m_Neighbors = m_CellSpace.GetNeighbors();
	for (SteeringAgent* agent : m_Agents)
	{
		m_CellSpace.UpdateAgentCell(agent, m_OldPositions[agent]);
		m_CellSpace.RegisterNeighbors(agent->GetPosition(), m_NeighborhoodRadius);
		m_NrOfNeighbors = m_CellSpace.GetNrOfNeighbors();
		m_Neighbors = m_CellSpace.GetNeighbors();
		//RegisterNeighbors(agent);
		if (m_CanDebugNeighbourhood)
		{
			SetBodyColor(agent);
		}
		(agent)->Update(deltaT);
		if (m_TrimWorld)
		{
			(agent)->TrimToWorld(m_WorldSize);
		}
		m_OldPositions[agent] = agent->GetPosition();
	}
	UpdateAndRenderUI();
}

void Flock::SetBodyColor(SteeringAgent* pAgent)
{
	if ((pAgent->GetPosition() == m_Agents[0]->GetPosition()))
	{
		for (int i{}; i < m_FlockSize; i++)
		{
			m_Agents[i]->SetBodyColor({ 1.f,1.f,0.f });
		}
		for (int i{}; i < m_NrOfNeighbors; ++i)
		{
			m_Neighbors[i]->SetBodyColor({ 0.f, 1.f, 0.f });
		}
	}
}

void Flock::Render(float deltaT)
{
	m_CellSpace.RenderCells(m_CanDebugGrid, m_Agents[0]->GetPosition(), m_NeighborhoodRadius);
	m_pAgentToEvade->Render(deltaT);
	m_pAgentToEvade->SetRenderBehavior(m_CanDebugRender);
	for (SteeringAgent* agent : m_Agents)
	{
		agent->Render(deltaT);
		agent->SetRenderBehavior(m_CanDebugRender);
	}
	if (m_CanDebugNeighbourhood)
	{
		std::list<Elite::Vector2> squareRadius{};
		squareRadius.push_back(Elite::Vector2{ m_Agents[0]->GetPosition().x - m_NeighborhoodRadius, m_Agents[0]->GetPosition().y - m_NeighborhoodRadius });
		squareRadius.push_back(Elite::Vector2{ m_Agents[0]->GetPosition().x - m_NeighborhoodRadius, m_Agents[0]->GetPosition().y + m_NeighborhoodRadius });
		squareRadius.push_back(Elite::Vector2{ m_Agents[0]->GetPosition().x + m_NeighborhoodRadius, m_Agents[0]->GetPosition().y + m_NeighborhoodRadius });
		squareRadius.push_back(Elite::Vector2{ m_Agents[0]->GetPosition().x + m_NeighborhoodRadius, m_Agents[0]->GetPosition().y - m_NeighborhoodRadius });
		Elite::Polygon neighbourhoodRect{ squareRadius };
		DEBUGRENDERER2D->DrawCircle(Elite::Vector2{ m_Agents[0]->GetPosition() }, m_NeighborhoodRadius, Elite::Color(0, 1, 1, 1), 0);
		if (m_CanDebugGrid)
		{
			DEBUGRENDERER2D->DrawPolygon(&neighbourhoodRect, Elite::Color(0, 1, 1, 1));
		}
	}
}

void Flock::UpdateAndRenderUI()
{
	//Setup
	int menuWidth = 235;
	int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
	int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
	bool windowActive = true;
	ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
	ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 20));
	ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::PushAllowKeyboardFocus(false);

	//Elements
	ImGui::Text("CONTROLS");
	ImGui::Indent();
	ImGui::Text("LMB: place target");
	ImGui::Text("RMB: move cam.");
	ImGui::Text("Scrollwheel: zoom cam.");
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("STATS");
	ImGui::Indent();
	ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Checkbox("Trim World", &m_TrimWorld);
	if (m_TrimWorld)
	{
		ImGui::SliderFloat("Trim Size", &m_WorldSize, 0.f, 500.f, "%1.");
	}
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("Flocking");
	ImGui::Spacing();

	ImGui::Checkbox("Debug Rendering", &m_CanDebugRender);
	ImGui::Checkbox("Debug Grid", &m_CanDebugGrid);
	ImGui::Checkbox("Debug Neighbourhood", &m_CanDebugNeighbourhood);

	// Implement checkboxes and sliders here
	ImGui::SliderFloat("Cohesion", &m_pBlendedSteering->m_WeightedBehaviors[0].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Seperation", &m_pBlendedSteering->m_WeightedBehaviors[1].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Allignment", &m_pBlendedSteering->m_WeightedBehaviors[2].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Seek", &m_pBlendedSteering->m_WeightedBehaviors[3].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Wander", &m_pBlendedSteering->m_WeightedBehaviors[4].weight, 0.f, 1.f, "%.2");

	//End
	ImGui::PopAllowKeyboardFocus();
	ImGui::End();
	
}

void Flock::RegisterNeighbors(SteeringAgent* pAgent)
{
	// register the agents neighboring the currently evaluated agent
	// store how many they are, so you know which part of the vector to loop over
	float distance{ };
	m_NrOfNeighbors = 0;
	for (int i{}; i < m_FlockSize; i++)
	{
		distance = Distance(m_Agents[i]->GetPosition(), pAgent->GetPosition());
		if (distance < m_NeighborhoodRadius)
		{
			m_Neighbors[m_NrOfNeighbors] = m_Agents[i];
			if (pAgent->GetPosition() == m_Agents[0]->GetPosition())
			{
				m_Agents[i]->SetBodyColor({ 0.f, 1.f, 0.f });
			}
			m_NrOfNeighbors++;
		}
		else
		{
			if (pAgent->GetPosition() == m_Agents[0]->GetPosition())
			{
				m_Agents[i]->SetBodyColor({ 1.f, 1.f, 0.f });
			}
		}
	}
}

Elite::Vector2 Flock::GetAverageNeighborPos() const
{
	Elite::Vector2 pos{};

	if (m_NrOfNeighbors == 0)
	{
		return pos;
	}

	for (int i{}; i < m_NrOfNeighbors; i++)
	{
		pos += m_Neighbors[i]->GetPosition();
	}
	pos /= float(m_NrOfNeighbors);

	return pos;
}

Elite::Vector2 Flock::GetAverageNeighborVelocity() const
{
	Elite::Vector2 avarageVel{};
	if (m_NrOfNeighbors == 0)
	{
		return avarageVel;
	}
	
	for (int i{}; i < m_NrOfNeighbors; i++)
	{
		avarageVel += m_Neighbors[i]->GetLinearVelocity();
	}
	
	avarageVel /= float(m_NrOfNeighbors);
	
	return avarageVel;
}


float* Flock::GetWeight(ISteeringBehavior* pBehavior) 
{
	if (m_pBlendedSteering)
	{
		auto& weightedBehaviors = m_pBlendedSteering->m_WeightedBehaviors;
		auto it = find_if(weightedBehaviors.begin(),
			weightedBehaviors.end(),
			[pBehavior](BlendedSteering::WeightedBehavior el)
			{
				return el.pBehavior == pBehavior;
			}
		);

		if(it!= weightedBehaviors.end())
			return &it->weight;
	}

	return nullptr;
}
