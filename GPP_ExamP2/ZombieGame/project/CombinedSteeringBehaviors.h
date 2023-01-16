#pragma once
#include "MySteeringBehaviors.h"
#include "Exam_HelperStructs.h"

class Flock;

//****************
//BLENDED STEERING
class BlendedSteering final: public MySteeringBehaviors
{
public:
	struct WeightedBehavior
	{
		MySteeringBehaviors* pBehavior = nullptr;
		float weight = 0.f;

		WeightedBehavior(MySteeringBehaviors* pBehavior, float weight) :
			pBehavior(pBehavior),
			weight(weight)
		{};
	};

	BlendedSteering(std::vector<WeightedBehavior> weightedBehaviors, SteeringPlugin_Output* pSteeringOutput);

	void AddBehaviour(WeightedBehavior weightedBehavior) { m_WeightedBehaviors.push_back(weightedBehavior); }
	void DoSteering();

	// returns a reference to the weighted behaviors, can be used to adjust weighting. Is not intended to alter the behaviors themselves.
	std::vector<WeightedBehavior>& GetWeightedBehaviorsRef() { return m_WeightedBehaviors; }

private:
	std::vector<WeightedBehavior> m_WeightedBehaviors = {};

	using MySteeringBehaviors::SetTarget; // made private because targets need to be set on the individual behaviors, not the combined behavior
};