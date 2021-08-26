/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere, Thomas Goussaert
/*=============================================================================*/
// SteeringBehaviors.h: SteeringBehaviors interface and different implementations
/*=============================================================================*/
#ifndef ELITE_STEERINGBEHAVIORS
#define ELITE_STEERINGBEHAVIORS

//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "SteeringHelpers.h"
class Obstacle;
class SteeringAgent;
using namespace Elite;

#pragma region **ISTEERINGBEHAVIOR** (BASE)
class ISteeringBehavior
{
public:
	ISteeringBehavior() = default;
	virtual ~ISteeringBehavior() = default;

	virtual SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) = 0;

	//Seek Functions
	void SetTarget(const TargetData& target) { m_Target = target; }

	template<class T, typename std::enable_if<std::is_base_of<ISteeringBehavior, T>::value>::type* = nullptr>
	T* As()
	{ return static_cast<T*>(this); }

protected:
	TargetData m_Target;
};
#pragma endregion

///////////////////////////////////////
//SEEK
//****
class Seek : public ISteeringBehavior
{
public:
	Seek() = default;
	virtual ~Seek() = default;

	//Seek Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
};

//////////////////////////
//WANDER
//******
class Wander : public Seek
{
public:
	Wander() = default;
	virtual ~Wander() = default;

	//Wander Behavior
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
	
	void SetWanderOffset(float offset) { m_Offset = offset; }
	void SetWanerRadius(float radius) { m_Radius = radius; }
	void SetMaxAngleChange(float rad) { m_AngleChange = rad; }
protected:
	float m_Offset = 6.f;
	float m_Radius = 4.f;
	float m_AngleChange = ToRadians(45);
	float m_WanderAngle = 0.f;

private:
	//void SetTarget(const TargetData* pTarget);
};

/////////////////////////
//Flee
//******
class Flee : public Seek
{
public:
	Flee() = default;
	virtual ~Flee() = default;

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
};

class Arrive : public Seek
{
public:
	Arrive() = default;
	virtual ~Arrive() = default;

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
};

class Face : public Seek
{
public:
	Face() = default;
	virtual ~Face() = default;

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
};

class Pursuit : public Seek
{
public:
	Pursuit() = default;
	virtual ~Pursuit() = default;

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
};

class Evade :public Flee
{
public:
	Evade() = default;
	virtual ~Evade() = default;

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
	void SetEvasionRadius(float radius) { m_EvasionRadius = radius; };

private:
	float m_EvasionRadius = 15.f;
};

class Context : public Seek
{
public:
	Context() = default;
	virtual ~Context() = default;

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
	bool isSegementInCircle(Vector2 start, Vector2 end, Vector2 circleCenter, float radius);
	void AddObstacle(std::pair<Vector2, float> obstacle) { m_Obstacles.push_back(obstacle); };
	void SetArraySize(int size = 8) { m_ArraySize = size; } ;

private:
	std::vector<std::pair<Vector2, float>> m_Obstacles{};
	std::vector<Vector2> m_Directions{};
	std::vector<float> m_Interests{};
	std::vector<float> m_Dangers{};
	Elite::Polygon m_Obstacle;
	size_t m_ArraySize = 16;
};
#endif


