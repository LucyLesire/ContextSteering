#pragma once
#include "../SteeringBehaviors.h"

class Flock;
class Army;

//SEPARATION - FLOCKING
//*********************
class Seperation : public Seek
{
public:
	Seperation() = default;
	virtual ~Seperation() = default;

	void SetFlock(Flock * flock);
	void SetArmy(Army* army);
	// Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent * pAgent) override;
private:
	Flock* m_pFlock = nullptr;
	Army* m_pArmy = nullptr;
};


//COHESION - FLOCKING
//*******************
class Cohesion : public Seek
{
public:
	Cohesion() = default;
	virtual ~Cohesion() = default;

	void SetFlock(Flock* flock);
	void SetArmy(Army* army);

	// Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
private:
	Flock* m_pFlock = nullptr;
	Army* m_pArmy = nullptr;
};

//VELOCITY MATCH - FLOCKING
//************************

class Allignment : public Seek
{
public:
	Allignment() = default;
	virtual ~Allignment() = default;

	void SetFlock(Flock * flock);
	void SetArmy(Army* army);
	// Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent * pAgent) override;

private:
	Flock* m_pFlock = nullptr;
	Army* m_pArmy = nullptr;
};
