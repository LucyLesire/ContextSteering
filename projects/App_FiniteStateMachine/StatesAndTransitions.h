/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// StatesAndTransitions.h: Implementation of the state/transition classes
/*=============================================================================*/
#ifndef ELITE_APPLICATION_FSM_STATES_TRANSITIONS
#define ELITE_APPLICATION_FSM_STATES_TRANSITIONS

#include "../Shared/Agario/AgarioAgent.h"
#include "../Shared/Agario/AgarioFood.h"
#include "../App_Steering/SteeringBehaviors.h"

//AGARIO AGENT STATES
//-------------------
class WanderState : public Elite::FSMState
{
public:
	WanderState() : FSMState() {};
	virtual void OnEnter(Blackboard* pB)
	{
		//Agent opvragen uit blackboard
		AgarioAgent* pAgent = nullptr;
		bool dataAvailable = pB->GetData("Agent", pAgent);
		
		if (!dataAvailable)
			return;
		
		if (!pAgent)
			return;
		//Agent wandering steering behavior actief zetten
		pAgent->SetToWander();
	}
};

class SeekState : public Elite::FSMState
{
public:
	SeekState() : FSMState() {};
	virtual void OnEnter(Blackboard* pB)
	{
		AgarioAgent* pAgent = nullptr;
		bool dataAvailable = pB->GetData("Agent", pAgent);
		if (!dataAvailable)
			return;

		if (!pAgent)
			return;

		Elite::Vector2 foodPos;
		dataAvailable = pB->GetData("FoodPosition", foodPos);
		if (!dataAvailable)
			return;

		//Agent seeking steering behavior actief zetten
		pAgent->SetToSeek(foodPos);
	}
};

//AGARIO AGENT TRANSITIONS
//------------------------
class CloseToFood : public Elite::FSMTransition
{
public:
	CloseToFood() : FSMTransition() {}
	virtual bool ToTransition(Blackboard* pB) const override
	{
		//Is food Close by?
		const float acceptenceRadius{ 50.f };
		AgarioAgent* pAgent = nullptr;
		bool dataAvailable = pB->GetData("CustomAgent", pAgent);

		if (!dataAvailable)
			return false;

		if (!pAgent)
			return false;

		std::vector<AgarioFood*> pFoodVec{};
		dataAvailable = pB->GetData("FoodVec", pFoodVec);
		
		if (!dataAvailable)
			return false;

		for (AgarioFood* food : pFoodVec)
		{
			if (Elite::Distance(food->GetPosition(), pAgent->GetPosition()) < acceptenceRadius)
			{
				food->MarkForDestroy();
				return true;
			}
		}
		return false;
	}
};

class EvadeBigAgent : public Elite::FSMTransition
{
public:
	EvadeBigAgent() : FSMTransition() {}
	virtual bool ToTransition(Blackboard* pB) const override
	{
		
	}
};

#endif