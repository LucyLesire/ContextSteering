#pragma once
#include "../SteeringHelpers.h"
#include "SpacePartitioning/SpacePartitioning.h"
#include "FlockingSteeringBehaviors.h"

class ISteeringBehavior;
class SteeringAgent;
class BlendedSteering;
class PrioritySteering;

class Flock
{
public:
	Flock(
		int flockSize = 500, 
		float worldSize = 100.f, 
		SteeringAgent* pAgentToEvade = nullptr, 
		bool trimWorld = false);

	~Flock();

	void Update(float deltaT);
	void UpdateAndRenderUI();
	void Render(float deltaT);

	void RegisterNeighbors(SteeringAgent* pAgent);
	int GetNrOfNeighbors() const { return m_NrOfNeighbors; }
	const vector<SteeringAgent*>& GetNeighbors() const { return m_Neighbors; }

	Elite::Vector2 GetAverageNeighborPos() const;
	Elite::Vector2 GetAverageNeighborVelocity() const;

	void SetMousePos(const TargetData& mouseTarget) { m_MousePos = mouseTarget; };

private:
	//Cell
	CellSpace m_CellSpace;

	// flock agents
	int m_FlockSize = 0;
	vector<SteeringAgent*> m_Agents;

	// neighborhood agents
	vector<SteeringAgent*> m_Neighbors;
	float m_NeighborhoodRadius = 10.f;
	int m_NrOfNeighbors = 0;

	// evade target
	SteeringAgent* m_pAgentToEvade = nullptr;
	float m_EvasionRadius = 15.f;

	// world info
	bool m_TrimWorld = false;
	float m_WorldSize = 0.f;
	
	// steering Behaviors
	BlendedSteering* m_pBlendedSteering = nullptr;
	PrioritySteering* m_pPrioritySteering = nullptr;
	Cohesion* m_pCohesion = nullptr;
	Wander* m_pWander = nullptr;
	Seek* m_pSeek = nullptr;
	Allignment* m_pAllignment = nullptr;
	Seperation* m_pSeperation = nullptr;
	Evade* m_pEvade = nullptr;
	Wander* m_pWanderEvader = nullptr;
	//Debug
	bool m_CanDebugRender = false;
	bool m_CanDebugGrid = false;
	bool m_CanDebugNeighbourhood = false;

	//Mouse
	TargetData m_MousePos;
	
	//
	std::unordered_map<SteeringAgent*, Elite::Vector2> m_OldPositions;
	// private functions
	float* GetWeight(ISteeringBehavior* pBehaviour);

	void SetBodyColor(SteeringAgent* pAgent);

private:
	Flock(const Flock& other);
	Flock& operator=(const Flock& other);
};