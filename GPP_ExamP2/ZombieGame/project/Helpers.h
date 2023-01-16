#pragma once
#include "stdafx.h"
#include "EBlackboard.h"
#include "Exam_HelperStructs.h"
#include "IExamInterface.h"

using namespace Elite;

namespace HelperFunction
{
	bool HasItem(Blackboard* pBlackboard, const eItemType& itemType, ItemInfo& itemOwned)
	{
		// Get info
		IExamInterface* pInterface;
		if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
			return false;

		// Go over all inventory slots
		for (int i = 0; i < pInterface->Inventory_GetCapacity(); ++i)
		{
			// When no item in slot continue
			if (!pInterface->Inventory_GetItem(i, itemOwned))
				continue;
			// if item in slot is same as desired item => return true
			if (itemOwned.Type == itemType)
				return true;
		}

		return false;
	}

	bool FreeSlotAvailable(Blackboard* pBlackboard)
	{
		// Get info
		IExamInterface* pInterface;
		if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
			return false;

		// Go over all inventory slots
		for (int i = 0; i < pInterface->Inventory_GetCapacity(); ++i)
		{
			ItemInfo itemInfo;
			// When no item in slot return true
			if (!pInterface->Inventory_GetItem(i, itemInfo))
				return true;
		}
		return false;
	}

	int GetFreeInventorySlot(Blackboard* pBlackboard)
	{
		// Get info
		IExamInterface* pInterface;
		if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
			return false;

		// Go over all inventory slots
		for (int i = 0; i < pInterface->Inventory_GetCapacity(); ++i)
		{
			ItemInfo itemInfo;
			// When no item in slot return slot
			if (!pInterface->Inventory_GetItem(i, itemInfo))
				return i;
		}
		return -1;
	}

	int GetSlotOfItem(Blackboard* pBlackboard, const eItemType& itemType)
	{
		// Get info
		IExamInterface* pInterface;
		if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
			return false;

		// Go over all inventory slots
		for (int i = 0; i < pInterface->Inventory_GetCapacity(); ++i)
		{
			ItemInfo itemInfo;
			// When no item in slot continue
			if (!pInterface->Inventory_GetItem(i, itemInfo))
				continue;
			// if item in slot is same as desired item => return true
			if (itemInfo.Type == itemType)
				return i;
		}

		return 10;
	}
}

namespace HelperStructs
{
	struct Exploration
	{
		// Exploration in spiral form
		bool center = { false };			// 0
		bool innerRight = { false };		// 1
		bool innerCornerRU = { false };		// 2
		bool innerCornerLU = { false };		// 3
		bool innerCornerLB = { false };		// 4
		bool outerCornerRB = { false };		// 5
		bool outerCornerRU = { false };		// 6
		bool outerCornerLU = { false };		// 7
		bool outerCornerLB = { false };		// 8
		bool rightBorder = { false };		// 9
	};

	struct House
	{
		// Zigzag pattern
		bool rightU = { false };
		bool rightB = { false };
		bool leftU = { false };
		bool leftB = { false };

		// Visited houses
		std::vector<HouseInfo> vHousesVisited = {};

		// Current house, houses will never spawn on {0, 0}
		HouseInfo houseInfo = {{ZeroVector2}, {ZeroVector2}};
	};
}