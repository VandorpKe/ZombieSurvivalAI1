//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"

//SEEK
//****
SteeringPlugin_Output Seek::CalculateSteering(AgentInfo* pAgentInfo)
{
	SteeringPlugin_Output steering = {};

	steering.LinearVelocity = m_Target - pAgentInfo->Position;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgentInfo->MaxLinearSpeed;
	
	return steering;
}

////FLEE
////****
SteeringOutput Flee::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	Elite::Vector2 toTarget = pAgent->GetPosition() - m_Target.Position;
	float distanceSqrd = toTarget.MagnitudeSquared();

	SteeringOutput steering = {};
	if(distanceSqrd > m_FleeRadius*m_FleeRadius)
	{
		steering.IsValid = false;
		return steering;
	}

	steering.LinearVelocity = m_Target.Position - pAgent->GetPosition();
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= -pAgent->GetMaxLinearSpeed();

	return steering;
}
//
////ARRIVE
////****
//SteeringOutput Arrive::CalculateSteering(float deltaT, SteeringAgent* pAgent)
//{
//	SteeringOutput steering = {};
//	const float slowRadius = 15.f;
//	const float arrivalRadius = 1.f;
//
//	steering.LinearVelocity = m_Target.Position - pAgent->GetPosition();
//	const float distanceSquared = steering.LinearVelocity.MagnitudeSquared();
//	steering.LinearVelocity.Normalize();
//
//	if(distanceSquared < (arrivalRadius*arrivalRadius))
//	{
//		steering.LinearVelocity.x = 0.f;
//		steering.LinearVelocity.y = 0.f;
//		return steering;
//	}
//
//	if (distanceSquared < (slowRadius*slowRadius))
//	{
//		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed() * distanceSquared / (slowRadius * slowRadius);
//	}
//	else
//	{
//		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();
//	}
//
//	return steering;
//}
//
////FACE
////****
//SteeringOutput Face::CalculateSteering(float deltaT, SteeringAgent* pAgent)
//{
//	pAgent->SetAutoOrient(false);
//
//	//SteeringOutput steering{};
//	//Elite::Vector2 directionVector{ m_Target.Position - pAgent->GetPosition() };
//	//directionVector.Normalize();
//	//const float orientation{ Elite::GetOrientationFromVelocity(directionVector)};
//	//steering.AngularVelocity = orientation - pAgent->GetRotation() - M_PI / 2.f;
//
//	const Elite::Vector2 agentDirection{ cosf(pAgent->GetRotation()), sinf(pAgent->GetRotation()) };
//	const Elite::Vector2 targetDirection{ (m_Target.Position - pAgent->GetPosition()).GetNormalized() };
//
//	const float dotProduct{ agentDirection.Dot(targetDirection) };            // - Projection of normalized vectors to check "overlap" (through projection)
//
//	float turnDirection{};
//	constexpr float epsilon{ 0.01f };                                        // - Chosen by iterative testing
//	if (dotProduct > 1 + epsilon || dotProduct < 1 - epsilon)                // - If agent & target vector overlap, result of dot will be 1
//	{
//		const float crossResult{ agentDirection.Cross(targetDirection) };    // - Determines turning direction
//		turnDirection = crossResult / abs(crossResult);                      // - "Unitize" turnDirection so speed of turning is not 
//	}                                                                        //	  determined by angular distance to target
//
//	SteeringOutput steering = {};
//	steering.AngularVelocity = turnDirection * pAgent->GetMaxAngularSpeed();
//
//	return steering;
//}
//
//WANDER
//****
//SteeringPlugin_Output Wander::CalculateSteering(AgentInfo* pAgentInfo)
//{
//	//Get the angle of the agent
//	//Find the direction where it's looking at
//	//Set a circle at the correct distance of the agent at the correct direction
//	const float agentAngle{ pAgent->GetRotation() };
//	const Elite::Vector2 agentDirection{ cosf(agentAngle), sinf(agentAngle) };
//	const Elite::Vector2 wanderCenter{ m_OffsetDistance * agentDirection + pAgent->GetPosition()};
//
//	m_Timer += deltaT;
//	if(m_Timer > 1.f)
//	{
//		m_Timer = 0.f;
//		const float minAngle{ m_WanderAngle - m_MaxAngleChange };
//		m_WanderAngle = float(rand() % int((minAngle + 90) + 1) + minAngle);
//	}
//	const Elite::Vector2 targetPointOnWander{ cosf(m_WanderAngle) * m_Radius + wanderCenter.x
//											, sinf(m_WanderAngle) * m_Radius + wanderCenter.y };
//
//	SteeringPlugin_Output steering{};
//	steering.LinearVelocity = targetPointOnWander - pAgent->GetPosition();
//	steering.LinearVelocity.GetNormalized() *= pAgent->GetMaxLinearSpeed();
//
//	return steering;
//}
//
////EVADE
////****
//SteeringOutput Evade::CalculateSteering(float deltaT, SteeringAgent* pAgent)
//{
//	SteeringOutput steering{};
//	Elite::Vector2 direction{};
//	const Elite::Vector2 evadeMagnitude = m_Target.Position - pAgent->GetPosition();
//
//	if(evadeMagnitude.MagnitudeSquared() <= 15 * 15)
//	{
//		//			'-' because we want to get away and not towards the target
//		direction = -(evadeMagnitude)+m_Target.LinearVelocity;
//	}
//
//	steering.LinearVelocity = direction;
//	steering.LinearVelocity.Normalize();
//	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();
//	return steering;
//}
