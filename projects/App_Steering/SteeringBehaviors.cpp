//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"
#include "SteeringAgent.h"
//#include "Obstacle.h"

//SEEK
//****
SteeringOutput Seek::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	steering.LinearVelocity = (m_Target).Position - pAgent->GetPosition(); //Desired Velocity
	steering.LinearVelocity.Normalize(); //Normalize Desired Velocity
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed(); //Rescale to Max Speed

	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0,1,0 }, 0.4f);

	return steering;
}

//WANDER (base> SEEK)
//******
SteeringOutput Wander::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};
	Elite::Vector2 randomPosInCircle{};

	Elite::Vector2 circlePos{ pAgent->GetDirection() * (m_Offset)+pAgent->GetPosition() };

	float rand = randomFloat(-m_AngleChange, m_AngleChange);

	m_WanderAngle += rand;
	//std::cout << ToDegrees(m_WanderAngle) << std::endl;

	Elite::Vector2 randomOffset{ cos(m_WanderAngle) * m_Radius ,sin(m_WanderAngle) * m_Radius };
	//std::cout << randomOffset.x << " " << randomOffset.y << std::endl;
	randomOffset += circlePos;

	m_Target.Position = randomOffset;

	steering = { Seek::CalculateSteering(deltaT, pAgent) };

	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
	{
		//DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0,1,0 }, 0.4f);
		DEBUGRENDERER2D->DrawCircle(Elite::Vector2{ circlePos }, m_Radius, Elite::Color(0, 1, 1, 1), 0);
	}

	return steering;
}

SteeringOutput Flee::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};


	steering = Seek::CalculateSteering(deltaT, pAgent);
	steering.LinearVelocity *= -1;

	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0,1,0 }, 0.4f);

	return steering;
}

SteeringOutput Arrive::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};
	const float arrivalRadius = 25.f;
	float distance{};

	distance = (m_Target.Position - pAgent->GetPosition()).Magnitude();
	steering.LinearVelocity = (m_Target).Position - pAgent->GetPosition(); //Desired Velocity
	steering.LinearVelocity.Normalize(); //Normalize Desired Velocity

	if (distance <= arrivalRadius)
	{
		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed() * (distance / arrivalRadius);
	}
	else
	{
		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();
	}
	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0,1,0 }, 0.4f);

	return steering;
}

SteeringOutput Face::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	const float speed{ 5.f };
	const float offset{ 0.1f };

	SteeringOutput steering = {};
	Elite::Vector2 direction{};
	float angle{};

	pAgent->SetAutoOrient(false);

	direction = (m_Target).Position;
	angle = atan2(direction.x, -direction.y);

	if (pAgent->GetRotation() < angle - offset)
	{
		steering.AngularVelocity = speed;
	}
	else if (pAgent->GetRotation() > angle + offset)
	{
		steering.AngularVelocity = -speed;
	}
	else
	{
		steering.AngularVelocity = 0.f;
	}

	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0,1,0 }, 0.4f);

	return steering;
}

SteeringOutput Pursuit::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};
	Elite::Vector2 targetPosition{};
	Elite::Vector2 targetVelocity{};
	Elite::Vector2 pursuitPosition{};
	int T{};

	T = 5;

	targetPosition = m_Target.Position;
	targetVelocity = m_Target.LinearVelocity;

	pursuitPosition = targetPosition + targetVelocity * T;

	steering.LinearVelocity = pursuitPosition - pAgent->GetPosition(); //Desired Velocity
	steering.LinearVelocity.Normalize(); //Normalize Desired Velocity
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed(); //Rescale to Max Speed

	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0,1,0 }, 0.4f);

	return steering;
}

SteeringOutput Evade::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	auto distanceToTarget = Distance(pAgent->GetPosition(), m_Target.Position);
	SteeringOutput steering{};

	if (distanceToTarget > m_EvasionRadius)
	{
		steering.IsValid = false;
		return steering;
	}


	Elite::Vector2 targetPosition{};
	Elite::Vector2 targetVelocity{};
	Elite::Vector2 evadePosition{};
	int T{};

	T = 5;

	targetPosition = m_Target.Position;
	targetVelocity = m_Target.LinearVelocity;

	evadePosition = targetPosition + targetVelocity * T;

	steering.LinearVelocity = pAgent->GetPosition() - evadePosition; //Desired Velocity
	steering.LinearVelocity.Normalize(); //Normalize Desired Velocity
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed(); //Rescale to Max Speed

	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0,1,0 }, 0.4f);

	return steering;
}

SteeringOutput Context::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	Elite::Color greenColor{0.f,1.f,0.f,1.f};
	Elite::Color redColor{ 1.f,0.f,0.f,1.f };
	std::vector<Color> colors{};

	steering.LinearVelocity = (m_Target).Position - pAgent->GetPosition(); //Desired Velocity
	steering.LinearVelocity.Normalize(); //Normalize Desired Velocity
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed(); //Rescale to Max Speed

	m_Directions.resize(m_ArraySize);
	m_Dangers.resize(m_ArraySize);
	m_Interests.resize(m_ArraySize);
	colors.resize(m_ArraySize);

	Vector2 pos = pAgent->GetPosition();
	Vector2 dir{ 1,0 };

	float angleAgent = pAgent->GetOrientation();

	for (int i{}; i < m_ArraySize; i++)
	{
		float angle = i * 2 * M_PI / m_ArraySize;
		Vector2 RotatedVector{};
		Vector2 rightVector{ dir };
		//Vector2 rightVector{ steering.LinearVelocity.GetNormalized() };

		//RotatedVector.x = rightVector.x * cos(angle) - rightVector.y * sin(angle);
		//RotatedVector.y = rightVector.x * sin(angle) - rightVector.y * cos(angle);
		// 
		RotatedVector.x = rightVector.x * cos(angleAgent + angle) - rightVector.y * sin(angleAgent + angle);
		RotatedVector.y = rightVector.x * sin(angleAgent + angle) - rightVector.y * cos(angleAgent + angle);
		
		RotatedVector = RotatedVector.GetNormalized();

		RotatedVector *= 5.f;
		m_Directions[i] = RotatedVector;
	}


	Vector2 desired{};
	
	desired = steering.LinearVelocity.GetNormalized();

	for (int i{}; i < m_ArraySize; i++)
	{
		float dot = m_Directions[i].GetNormalized().Dot(desired);
		if (dot <= 0.f)
		{
			m_Interests[i] = 0;
		}
		else
		{
			m_Interests[i] = dot;
		}
	}

	for (int i{}; i < m_ArraySize; i++)
	{
		bool active{};
		for (int j{}; j < m_Obstacles.size(); j++)
		{
			if (isSegementInCircle(pos, m_Directions[i], m_Obstacles[j].first, m_Obstacles[j].second))
			{
				m_Dangers[i] = 1.f;
				active = true;
				colors[i] = redColor;
			}
			else if(!active)
			{
				m_Dangers[i] = 0.f;
				colors[i] = greenColor;
			}
		}

	}

	if (pAgent->CanRenderBehavior())
	{
		for (int i{}; i < m_ArraySize; i++)
		{
			DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), m_Directions[i] * m_Interests[i], (m_Directions[i] * m_Interests[i]).Magnitude(), colors[i], 0.4f);
		}
	}


	for (int i{}; i < m_ArraySize; i++)
	{
		m_Interests[i] -= m_Dangers[i];

	}

	Elite::Vector2 chosenDirection{};

	for (int i{}; i < m_ArraySize; i++)
	{
		chosenDirection += m_Directions[i] * m_Interests[i];
	}

	chosenDirection = chosenDirection.GetNormalized();
	chosenDirection *= pAgent->GetMaxLinearSpeed();

	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), chosenDirection, chosenDirection.Magnitude(), { 0,0,1, 1.f }, 0.4f);
	
	return chosenDirection;
}

bool Context::isSegementInCircle(Vector2 start, Vector2 end, Vector2 circleCenter, float radius)
{
	Vector2 closestPoint = ProjectOnLineSegment(start, start + end, circleCenter);
	Vector2 distanceToCircleCenter = circleCenter - closestPoint;

	if (distanceToCircleCenter.MagnitudeSquared() < (radius*radius))
	{
		return true;
	}

	return false;
}
