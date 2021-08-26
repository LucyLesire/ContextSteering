/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// Behaviors.h: Implementation of certain reusable behaviors for the BT version of the Agario Game
/*=============================================================================*/
#ifndef ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
#define ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteMath/EMath.h"
#include "framework/EliteAI/EliteDecisionMaking/EliteBehaviorTree/EBehaviorTree.h"
#include "../Shared/Agario/AgarioAgent.h"
#include "../Shared/Agario/AgarioFood.h"
#include "../App_Steering/SteeringBehaviors.h"

//-----------------------------------------------------------------
// Behaviors
//-----------------------------------------------------------------
bool IsCloseToFood(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent = nullptr;
	std::vector<AgarioFood*>* foodVec = nullptr;

	auto dataAvailable = pBlackboard->GetData("Agent", pAgent) &&
		pBlackboard->GetData("FoodVec", foodVec);

	if (!pAgent || !foodVec)
		return false;

	//TODO: Check for food closeby and set target accordingly
	const float closeToRange{ 20.f };
	auto foodIT = std::find_if(foodVec->begin(), foodVec->end(), [&pAgent, &closeToRange](AgarioFood* f)
		{
			//true if close to agent
			return DistanceSquared(pAgent->GetPosition(), f->GetPosition()) < (closeToRange * closeToRange);
		});
	if (foodIT != foodVec->end())
	{
		pBlackboard->ChangeData("Target", (*foodIT)->GetPosition());
		std::cout << "Food found\n";
		return true;
	}
	return false;
}

bool IsCloseToEnemy(Elite::Blackboard* pB)
{
	AgarioAgent* pAgent = nullptr;
	std::vector<AgarioAgent*>* agentVec = nullptr;
	
	auto dataAvailable = pB->GetData("Agent", pAgent) && pB->GetData("AgentVec", agentVec);

	if (!pAgent || !agentVec)
		return false;

	const float closeToRange{ 20.f };
	auto agentIT = std::find_if(agentVec->begin(), agentVec->end(), [&pAgent, &closeToRange](AgarioAgent* f)
		{
			return DistanceSquared(pAgent->GetPosition(), f->GetPosition()) < (closeToRange * closeToRange) && (pAgent->GetRadius() < f->GetRadius());
		});
	if (agentIT != agentVec->end())
	{
		pB->ChangeData("Target", (*agentIT)->GetPosition());
		std::cout << "Target Found\n";
		return true;
	}
	return false;
}

bool IsCloseToAgentFood(Elite::Blackboard* pB)
{
	AgarioAgent* pAgent = nullptr;
	std::vector<AgarioAgent*>* agentVec = nullptr;

	auto dataAvailable = pB->GetData("Agent", pAgent) && pB->GetData("AgentVec", agentVec);

	if (!pAgent || !agentVec)
		return false;

	const float closeToRange{ 20.f };
	const float sizeDiffCoeff{ 1.05f };
	auto agentIT = std::find_if(agentVec->begin(), agentVec->end(), [&pAgent, &closeToRange, &sizeDiffCoeff](AgarioAgent* f)
		{
			return DistanceSquared(pAgent->GetPosition(), f->GetPosition()) < (closeToRange * closeToRange) && (pAgent->GetRadius() > f->GetRadius() * sizeDiffCoeff);
		});
	if (agentIT != agentVec->end())
	{
		pB->ChangeData("Target", (*agentIT)->GetPosition());
		std::cout << "Target Foood Found\n";
		return true;
	}
	return false;
}

BehaviorState ChangeToWander(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent = nullptr;
	auto dataAvailable = pBlackboard->GetData("Agent", pAgent);

	if (!pAgent)
		return Failure;

	pAgent->SetToWander();

	return Success;
}

BehaviorState ChangeToSeek(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent = nullptr;
	Vector2 seekTarget{};
	auto dataAvailable = pBlackboard->GetData("Agent", pAgent) &&
		pBlackboard->GetData("Target", seekTarget);

	if (!pAgent)
		return Failure;
	
	//TODO: Implement Change to seek (Target)
	pAgent->SetToSeek(seekTarget);
	return Success;
}

BehaviorState ChangeToFlee(Elite::Blackboard* pB)
{
	AgarioAgent* pAgent = nullptr;
	Vector2 fleeTarget{};
	auto dataAvailable = pB->GetData("Agent", pAgent) && pB->GetData("Target", fleeTarget);

	if (!pAgent)
		return Failure;

	pAgent->SetToFlee(fleeTarget);
	return Success;
}

#endif