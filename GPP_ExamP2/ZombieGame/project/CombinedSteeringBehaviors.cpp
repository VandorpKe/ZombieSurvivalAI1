#include "stdafx.h"
#include "CombinedSteeringBehaviors.h"
#include <algorithm>

BlendedSteering::BlendedSteering(std::vector<WeightedBehavior> weightedBehaviors, SteeringPlugin_Output* pSteeringOutput)
	:m_WeightedBehaviors(weightedBehaviors)
{
};

//****************
//BLENDED STEERING
void BlendedSteering::DoSteering()
{
	SteeringPlugin_Output blendedSteering = {};
	auto totalWeight = 0.f;

	for (auto weightedBehavior : m_WeightedBehaviors)
	{
		auto steering = weightedBehavior.pBehavior->CalculateSteering(deltaT, pAgent);
		blendedSteering.LinearVelocity += weightedBehavior.weight * steering.LinearVelocity;
		blendedSteering.AngularVelocity += weightedBehavior.weight * steering.AngularVelocity;

		totalWeight += weightedBehavior.weight;
	}

	if (totalWeight > 0.f)
	{
		auto scale = 1.f / totalWeight;
		blendedSteering. *= scale;
	}

	return blendedSteering;
}