/*=============================================================================*/
// Copyright 2021-2022 Elite Engine
// Authors: Matthieu Delaere, Thomas Goussaert
/*=============================================================================*/
// SteeringBehaviors.h: SteeringBehaviors interface and different implementations
/*=============================================================================*/
#ifndef ELITE_STEERINGBEHAVIORS
#define ELITE_STEERINGBEHAVIORS

//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "Exam_HelperStructs.h"
#include "IExamInterface.h"
class SteeringAgent;

#pragma region **ISTEERINGBEHAVIOR** (BASE)
class ISteeringBehavior
{
public:
	ISteeringBehavior() = default;
	virtual ~ISteeringBehavior() = default;

	virtual SteeringPlugin_Output CalculateSteering(AgentInfo* pAgentInfo) = 0;

	//Seek Functions
	void SetTarget(const Elite::Vector2& target) { m_Target = target; }

	template<class T, typename std::enable_if<std::is_base_of<ISteeringBehavior, T>::value>::type* = nullptr>
	T* As()
	{ return static_cast<T*>(this); }

protected:
	Elite::Vector2 m_Target;
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
	SteeringPlugin_Output CalculateSteering(AgentInfo* pAgentInfo) override;
};

/////////////////////////////////////////
////FLEE
////****
//class Flee : public ISteeringBehavior
//{
//public:
//	Flee() = default;
//	virtual ~Flee() = default;
//
//	//Seek behavior
//	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
//	void SetFleeRadius(float fleeRadius) { m_FleeRadius = fleeRadius; }
//private:
//	float m_FleeRadius = 20.f;
//};
//
/////////////////////////////////////////
////ARRIVE
////****
//class Arrive : public ISteeringBehavior
//{
//public:
//	Arrive() = default;
//	virtual ~Arrive() = default;
//
//	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
//};
//
/////////////////////////////////////////
////FACE
////****
//class Face : public ISteeringBehavior
//{
//public:
//	Face() = default;
//	virtual ~Face() = default;
//
//	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
//};
//
/////////////////////////////////////
//WANDER
//****
class Wander : public Seek
{
public:
	Wander() = default;
	virtual ~Wander() = default;

	//Wander Behaviour
	SteeringPlugin_Output CalculateSteering(AgentInfo* pAgentInfo)override;

	void SetWanderOffset(float offset) { m_OffsetDistance = offset; }
	void SetWanderRadius(float radius) { m_Radius = radius; }
	void SetMaxAngleChange(float rad) { m_MaxAngleChange = rad; }
protected:
	float m_Timer = 0.f;
	float m_OffsetDistance = 6.f; //Offset (Agent Direction)
	float m_Radius = 4.f; //WanderRadius
	float m_MaxAngleChange = Elite::ToRadians(45); //Max WanderAngle change per frame
	float m_WanderAngle = 0.f; //Internal
};

/////////////////////////////////////////
////EVADE
////****
//class Evade : public ISteeringBehavior
//{
//public:
//	Evade() = default;
//	virtual ~Evade() = default;
//
//	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
//};
#endif