#include "stdafx.h"
#include "FlockingSteeringBehaviors.h"
#include "TheFlock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"
#include "projects/App_Research/Army.h"

//*********************
//SEPARATION (FLOCKING)
void Seperation::SetFlock(Flock* flock)
{
	m_pFlock = flock;
}

void Seperation::SetArmy(Army* army)
{
	m_pArmy = army;
}

SteeringOutput Seperation::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	float distance{};
	float magInvVector{};
	Elite::Vector2 vectorNeighbourAgent{};
	Elite::Vector2 sumVector{};
	SteeringOutput steering{};

	//if (m_pFlock->GetNrOfNeighbors() == 0)
	//{
	//	return steering;
	//}

	if (m_pArmy->GetNrOfNeighbors() == 0)
	{
		return steering;
	}

	for (int i{}; i < m_pArmy->GetNrOfNeighbors(); i++)
	{
		distance = Distance(pAgent->GetPosition(), m_pArmy->GetNeighbors()[i]->GetPosition());
		magInvVector = 20.f - distance;
		vectorNeighbourAgent = pAgent->GetPosition() - m_pArmy->GetNeighbors()[i]->GetPosition();
		vectorNeighbourAgent.Normalize();
		vectorNeighbourAgent *= magInvVector;
		sumVector += vectorNeighbourAgent;
	}
	sumVector /= m_pArmy->GetNrOfNeighbors();
	sumVector *= pAgent->GetMaxLinearSpeed();

	steering.LinearVelocity = sumVector;

	return steering;
}

//*******************
//COHESION (FLOCKING)
void Cohesion::SetFlock(Flock* flock)
{
	m_pFlock = flock;
}

void Cohesion::SetArmy(Army* army)
{
	m_pArmy = army;
}

SteeringOutput Cohesion::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};
	//if (m_pFlock->GetNrOfNeighbors() == 0)
	//{
	//	return steering;
	//}

	if (m_pArmy->GetNrOfNeighbors() == 0)
	{
		return steering;
	}

	m_Target.Position = m_pArmy->GetAverageNeighborPos();
	return Seek::CalculateSteering(deltaT, pAgent);
}

//*************************
//VELOCITY MATCH (FLOCKING)
void Allignment::SetFlock(Flock* flock)
{
	m_pFlock = flock;
}

void Allignment::SetArmy(Army* army)
{
	m_pArmy = army;
}

SteeringOutput Allignment::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	Elite::Vector2 avarageVel = m_pArmy->GetAverageNeighborVelocity();
	pAgent->SetLinearVelocity(avarageVel);
	SteeringOutput steering{ avarageVel };
	return steering;
}





