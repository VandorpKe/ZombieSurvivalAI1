#include "stdafx.h"
#include "MySteeringBehaviors.h"

MySteeringBehaviors::MySteeringBehaviors(SteeringPlugin_Output* pSteeringOutput, IExamInterface* pInterface)
	: m_pSteeringOutput(pSteeringOutput)
	, m_pInterface(pInterface) 
{
}

// *****************************
// SEEK
void MySteeringBehaviors::Seek() const
{
	const AgentInfo agentInfo = m_pInterface->Agent_GetInfo();

	m_pSteeringOutput->AutoOrient = true;
	m_pSteeringOutput->LinearVelocity = m_Target - agentInfo.Position;
	m_pSteeringOutput->LinearVelocity.Normalize();
	m_pSteeringOutput->LinearVelocity *= agentInfo.MaxLinearSpeed;
}

// *****************************
// FLEE
void MySteeringBehaviors::Flee(float fleeRadius) const
{
	const AgentInfo agentInfo = m_pInterface->Agent_GetInfo();

	const Vector2 toTarget = agentInfo.Position - m_Target;
	float distanceSqrd = toTarget.MagnitudeSquared();

	if(distanceSqrd < fleeRadius * fleeRadius)
	{
		m_pSteeringOutput->LinearVelocity = m_Target - agentInfo.Position;
		m_pSteeringOutput->LinearVelocity.Normalize();
		m_pSteeringOutput->LinearVelocity *= -agentInfo.MaxLinearSpeed;
	}
	else
	{
		m_pSteeringOutput->LinearVelocity = ZeroVector2;
	}
}

// *****************************
// FACE AND FLEE
void MySteeringBehaviors::FaceFlee(float fleeRadius) const
{
	AgentInfo agentInfo = { m_pInterface->Agent_GetInfo() };

	// FACE
	m_pSteeringOutput->AutoOrient = false;
	const Elite::Vector2 agentDirection{ cosf(agentInfo.Orientation), sinf(agentInfo.Orientation) };
	const Elite::Vector2 targetDirection{ (m_Target - agentInfo.Position.GetNormalized()) };

	const float dotProduct{ agentDirection.Dot(targetDirection) };            // - Projection of normalized vectors to check "overlap" (through projection)

	float turnDirection{};
	constexpr float epsilon{ 0.01f };                                        // - Chosen by iterative testing
	if (dotProduct > 1 + epsilon || dotProduct < 1 - epsilon)                // - If agent & target vector overlap, result of dot will be 1
	{
		const float crossResult{ agentDirection.Cross(targetDirection) };    // - Determines turning direction
		turnDirection = crossResult / abs(crossResult);                      // - "Unitize" turnDirection so speed of turning is not 
	}                                                                        //	  determined by angular distance to target

	m_pSteeringOutput->AngularVelocity = turnDirection * agentInfo.MaxAngularSpeed;

	// FLEE
	const Vector2 toTarget = agentInfo.Position - m_Target;
	float distanceSqrd = toTarget.MagnitudeSquared();

	if (distanceSqrd < fleeRadius * fleeRadius)
	{
		m_pSteeringOutput->LinearVelocity = m_Target - agentInfo.Position;
		m_pSteeringOutput->LinearVelocity.Normalize();
		m_pSteeringOutput->LinearVelocity *= -agentInfo.MaxLinearSpeed;
	}
	else
	{
		m_pSteeringOutput->LinearVelocity = ZeroVector2;
	}
}