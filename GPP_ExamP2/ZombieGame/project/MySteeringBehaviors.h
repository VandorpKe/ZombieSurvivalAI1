#pragma once
#include "Exam_HelperStructs.h"
#include "SteeringBehaviors.h"
#include "IExamInterface.h"

using namespace Elite;

class MySteeringBehaviors
{
public:
	MySteeringBehaviors(SteeringPlugin_Output* pSteeringOutput, IExamInterface* pInterface);
	~MySteeringBehaviors() = default;

	//Seek
	void SetTarget(const Vector2& target) { m_Target = target; }

	void Seek() const;
	void Flee(float fleeRadius) const;
	void FaceFlee(float fleeRadius) const;
private:
	SteeringPlugin_Output* m_pSteeringOutput = nullptr;
	IExamInterface* m_pInterface = nullptr;
	Vector2 m_Target;
};
