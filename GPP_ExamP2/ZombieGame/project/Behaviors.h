/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// Behaviors.h: Implementation of certain reusable behaviors for the BT version of the Agario Game
/*=============================================================================*/
#pragma once
#ifndef ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
#define ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "EliteMath/EMath.h"
#include "EBehaviorTree.h"
#include "MySteeringBehaviors.h"
#include "Helpers.h"

//-----------------------------------------------------------------
// Behaviors
//-----------------------------------------------------------------

namespace  BT_Actions
{
	// SEEK
	Elite::BehaviorState ChangeToSeek(Blackboard* pBlackboard)
	{
		MySteeringBehaviors* pSteeringBehavior;
		if (!pBlackboard->GetData("SteeringBehavior", pSteeringBehavior) || pSteeringBehavior == nullptr)
			return Elite::BehaviorState::Failure;

		pSteeringBehavior->Seek();
		return Elite::BehaviorState::Success;
	}

	// EXPLORE
	Elite::BehaviorState ChangeToExplore(Blackboard* pBlackboard)
	{
		// Get info
		MySteeringBehaviors* pSteeringBehavior;
		if (!pBlackboard->GetData("SteeringBehavior", pSteeringBehavior) || pSteeringBehavior == nullptr)
			return Elite::BehaviorState::Failure;
		HelperStructs::Exploration* pExplorationHelpers;
		if (!pBlackboard->GetData("ExplorationHelpers", pExplorationHelpers) || pExplorationHelpers == nullptr)
			return Elite::BehaviorState::Failure;
		IExamInterface* pInterface;
		if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
			return Elite::BehaviorState::Failure;

		AgentInfo agentInfo = pInterface->Agent_GetInfo();
		WorldInfo worldInfo = pInterface->World_GetInfo();

		// Do logic

	}

	// FLEE
	Elite::BehaviorState ChangeToFlee(Blackboard* pBlackboard)
	{
		MySteeringBehaviors* pSteeringBehavior;
		if (!pBlackboard->GetData("SteeringBehavior", pSteeringBehavior) || pSteeringBehavior == nullptr)
			return Elite::BehaviorState::Failure;

		pSteeringBehavior->Flee(15); // TEST
		return Elite::BehaviorState::Success;
	}
	// FLEE - PURGE
	Elite::BehaviorState ChangeToFleePurge(Blackboard* pBlackboard)
	{
		// Get info
		MySteeringBehaviors* pSteeringBehavior;
		if (!pBlackboard->GetData("SteeringBehavior", pSteeringBehavior) || pSteeringBehavior == nullptr)
			return Elite::BehaviorState::Failure;
		std::vector<EntityInfo>* pEntitiesInFov;
		if (!pBlackboard->GetData("EntitiesInFov", pEntitiesInFov) || pEntitiesInFov == nullptr)
			return Elite::BehaviorState::Failure;
		IExamInterface* pInterface;
		if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
			return Elite::BehaviorState::Failure;

		// Do logic
		PurgeZoneInfo zoneInfo;
		for(auto& e : *pEntitiesInFov )
		{
			if(e.Type == eEntityType::PURGEZONE)
			{
				pInterface->PurgeZone_GetInfo(e, zoneInfo);
			}
		}

		// Set target to desired target to flee
		pSteeringBehavior->SetTarget(zoneInfo.Center);
		// Flee the PurgeZone with an offset so body isn't in purge
		pSteeringBehavior->Flee(zoneInfo.Radius + 5);
		return Elite::BehaviorState::Success;
	}

	// LOOT
	Elite::BehaviorState LootItems(Blackboard* pBlackboard)
	{
		// Get info
		std::vector<EntityInfo>* pEntitiesInFov;
		if (!pBlackboard->GetData("EntitiesInFov", pEntitiesInFov) || pEntitiesInFov == nullptr)
			return Elite::BehaviorState::Failure;
		IExamInterface* pInterface;
		if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
			return Elite::BehaviorState::Failure;
		MySteeringBehaviors* pSteeringBehavior;
		if (!pBlackboard->GetData("SteeringBehavior", pSteeringBehavior) || pSteeringBehavior == nullptr)
			return Elite::BehaviorState::Failure;

		// Do logic
		const AgentInfo agentInfo = pInterface->Agent_GetInfo();

		if (pEntitiesInFov->empty())
			return Elite::BehaviorState::Failure;

		for(auto& e : *pEntitiesInFov)
		{
			if (e.Type != eEntityType::ITEM)
				continue;

			// Go to item if agent not in grab range
			if(agentInfo.Position.DistanceSquared(e.Location) >= agentInfo.GrabRange * agentInfo.GrabRange)
			{
				pSteeringBehavior->SetTarget(e.Location);
				pSteeringBehavior->Seek();
			}

			// Get item info
			ItemInfo itemInfo;
			pInterface->Item_GetInfo(e, itemInfo);

			// Do double check if item in grab range
			if (!(agentInfo.Position.DistanceSquared(itemInfo.Location) > agentInfo.GrabRange * agentInfo.GrabRange))
			{
				pSteeringBehavior->SetTarget(itemInfo.Location);
				pSteeringBehavior->Seek();
			}

			int currentSlot;
			int inventorySlot;
			ItemInfo inventoryItem;
			switch (itemInfo.Type)
			{
			case eItemType::PISTOL:
				// Is item in the inventory
				if (!HelperFunction::HasItem(pBlackboard, eItemType::PISTOL, inventoryItem))
				{
					// Is there a free slot
					inventorySlot = HelperFunction::GetFreeInventorySlot(pBlackboard);
					if (inventorySlot == -1)
						return Elite::BehaviorState::Failure;

					pInterface->Item_Grab(e, itemInfo);
					pInterface->Inventory_AddItem(inventorySlot, itemInfo);
					return Elite::BehaviorState::Success;
				}

				// Item already in inventory
				// Check if ammo is greater then ammo in inventory slot
				if (pInterface->Weapon_GetAmmo(itemInfo) <= pInterface->Weapon_GetAmmo(inventoryItem))
				{
					// Ammo not greater, destroy item
					pInterface->Item_Destroy(e);
					return Elite::BehaviorState::Success;
				}

				// Get the slot of the item that's already in the inventory
				currentSlot = HelperFunction::GetSlotOfItem(pBlackboard, eItemType::PISTOL);
				// Remove said item
				pInterface->Inventory_RemoveItem(currentSlot);
				// Add item with bigger stats
				pInterface->Item_Grab(e, itemInfo);
				pInterface->Inventory_AddItem(currentSlot, itemInfo);
				return Elite::BehaviorState::Success;
			case eItemType::SHOTGUN:
				// Is item in the inventory
				if (!HelperFunction::HasItem(pBlackboard, eItemType::SHOTGUN, inventoryItem))
				{
					// Is there a free slot
					inventorySlot = HelperFunction::GetFreeInventorySlot(pBlackboard);
					if (inventorySlot == -1)
						return Elite::BehaviorState::Failure;

					pInterface->Item_Grab(e, itemInfo);
					pInterface->Inventory_AddItem(inventorySlot, itemInfo);
					return Elite::BehaviorState::Success;
				}

				// Item already in inventory
				// Check if ammo is greater then ammo in inventory slot
				if (pInterface->Weapon_GetAmmo(itemInfo) <= pInterface->Weapon_GetAmmo(inventoryItem))
				{
					// Ammo not greater, destroy item
					pInterface->Item_Destroy(e);
					return Elite::BehaviorState::Success;
				}

				// Get the slot of the item that's already in the inventory
				currentSlot = HelperFunction::GetSlotOfItem(pBlackboard, eItemType::SHOTGUN);
				// Remove said item
				pInterface->Inventory_RemoveItem(currentSlot);
				// Add item with bigger stats
				pInterface->Item_Grab(e, itemInfo);
				pInterface->Inventory_AddItem(currentSlot, itemInfo);
				return Elite::BehaviorState::Success;
			case eItemType::MEDKIT:
				// Is item in the inventory
				if (!HelperFunction::HasItem(pBlackboard, eItemType::MEDKIT, inventoryItem))
				{
					// Is there a free slot
					inventorySlot = HelperFunction::GetFreeInventorySlot(pBlackboard);
					if (inventorySlot == -1)
						return Elite::BehaviorState::Failure;

					pInterface->Item_Grab(e, itemInfo);
					pInterface->Inventory_AddItem(inventorySlot, itemInfo);
					return Elite::BehaviorState::Success;
				}

				// Item already in inventory
				// Check if medkit healing is greater then the other in inventory slot
				if (!(pInterface->Medkit_GetHealth(itemInfo) >= pInterface->Medkit_GetHealth(inventoryItem)))
				{
					// healing not greater, destroy item
					pInterface->Item_Destroy(e);
					return Elite::BehaviorState::Success;
				}

				// Get the slot of the item that's already in the inventory
				currentSlot = HelperFunction::GetSlotOfItem(pBlackboard, eItemType::MEDKIT);
				// Remove said item
				pInterface->Inventory_RemoveItem(currentSlot);
				// Add item with bigger stats
				pInterface->Item_Grab(e, itemInfo);
				pInterface->Inventory_AddItem(currentSlot, itemInfo);
				return Elite::BehaviorState::Success;
			case eItemType::FOOD:
				// Is item in the inventory
				if (!HelperFunction::HasItem(pBlackboard, eItemType::FOOD, inventoryItem))
				{
					// Is there a free slot
					inventorySlot = HelperFunction::GetFreeInventorySlot(pBlackboard);
					if (inventorySlot == -1)
						return Elite::BehaviorState::Failure;
					pInterface->Item_Grab(e, itemInfo);
					pInterface->Inventory_AddItem(inventorySlot, itemInfo);
					return Elite::BehaviorState::Success;
				}

				// Item already in inventory
				// Check if energy is greater then the other in inventory slot
				if (pInterface->Food_GetEnergy(itemInfo) <= pInterface->Food_GetEnergy(inventoryItem))
				{
					// See if there is still a slot available
					if(HelperFunction::FreeSlotAvailable(pBlackboard))
					{
						// Get the free slot and add item
						currentSlot = HelperFunction::GetFreeInventorySlot(pBlackboard);
						pInterface->Item_Grab(e, itemInfo);
						pInterface->Inventory_AddItem(currentSlot, itemInfo);
						return Elite::BehaviorState::Success;
					}

					// Energy not greater, destroy item
					pInterface->Item_Destroy(e);
					return Elite::BehaviorState::Success;
				}

				// Get the slot of the item that's already in the inventory
				currentSlot = HelperFunction::GetSlotOfItem(pBlackboard, eItemType::FOOD);
				// Remove said item
				pInterface->Inventory_RemoveItem(currentSlot);
				// Add item with bigger stats
				pInterface->Item_Grab(e, itemInfo);
				pInterface->Inventory_AddItem(currentSlot, itemInfo);
				return Elite::BehaviorState::Success;
			case eItemType::GARBAGE:
				pInterface->Item_Destroy(e);
				return Elite::BehaviorState::Success;
			}
			return Elite::BehaviorState::Failure;
		}
		return Elite::BehaviorState::Failure;
	}

	// HOUSE
	Elite::BehaviorState GoInsideHouse(Blackboard* pBlackboard)
	{
		// Get info
		MySteeringBehaviors* pSteeringBehavior;
		if (!pBlackboard->GetData("SteeringBehavior", pSteeringBehavior) || pSteeringBehavior == nullptr)
			return Elite::BehaviorState::Failure;
		HelperStructs::House* pHouseHelpers;
		if (!pBlackboard->GetData("HouseHelpers", pHouseHelpers))
			return Elite::BehaviorState::Failure;
		IExamInterface* pInterface;
		if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
			return Elite::BehaviorState::Failure;

		// Do logic
		const Vector2 target = pInterface->NavMesh_GetClosestPathPoint(pHouseHelpers->houseInfo.Center);
		pSteeringBehavior->SetTarget(target);
		pSteeringBehavior->Seek();
		return Elite::BehaviorState::Success;
	}
	Elite::BehaviorState CheckInsideHouse(Blackboard* pBlackboard)
	{
		// Get info
		MySteeringBehaviors* pSteeringBehavior;
		if (!pBlackboard->GetData("SteeringBehavior", pSteeringBehavior) || pSteeringBehavior == nullptr)
			return Elite::BehaviorState::Failure;
		HelperStructs::House* pHouseHelpers;
		if (!pBlackboard->GetData("HouseHelpers", pHouseHelpers))
			return Elite::BehaviorState::Failure;
		IExamInterface* pInterface;
		if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
			return Elite::BehaviorState::Failure;

		const AgentInfo agentInfo = pInterface->Agent_GetInfo();

		// Do logic
		const float playerOffset = { 2 };
		const float houseOffset = { 5 };
		const Vector2 houseCenter = { pHouseHelpers->houseInfo.Center };
		const Vector2 houseSize = { pHouseHelpers->houseInfo.Size };

		const Vector2 goalLeftU = { houseCenter.x - (houseSize.x / 2) + houseOffset, houseCenter.y + (houseSize.y / 2) - houseOffset};
		const Vector2 goalLeftB = { houseCenter.x - (houseSize.x / 2) + houseOffset, houseCenter.y - (houseSize.y / 2) + houseOffset};
		const Vector2 goalRightU = { houseCenter.x + (houseSize.x / 2) - houseOffset, houseCenter.y + (houseSize.y / 2) - houseOffset };
		const Vector2 goalRightB = { houseCenter.x + (houseSize.x / 2) - houseOffset, houseCenter.y - (houseSize.y / 2) + houseOffset };

		// Check if he went to Left Top corner
		if (agentInfo.Position.DistanceSquared(goalLeftU) <= playerOffset * playerOffset)
			pHouseHelpers->leftU = true;
		else
		{
			const Vector2 target = pInterface->NavMesh_GetClosestPathPoint(goalLeftU);
			pSteeringBehavior->SetTarget(target);
			pSteeringBehavior->Seek();
		}

		// Check if he went to Left Bottom corner
		if(pHouseHelpers->leftU && agentInfo.Position.DistanceSquared(goalLeftB) <= playerOffset * playerOffset)
			pHouseHelpers->leftB = true;
		else if (pHouseHelpers->leftU)
		{
			const Vector2 target = pInterface->NavMesh_GetClosestPathPoint(goalLeftB);
			pSteeringBehavior->SetTarget(target);
			pSteeringBehavior->Seek();
		}

		// Check if he went to Right Top corner
		if (pHouseHelpers->leftU && 
			pHouseHelpers->leftB && agentInfo.Position.DistanceSquared(goalRightU) <= playerOffset * playerOffset)
			pHouseHelpers->rightU = true;
		else if (pHouseHelpers->leftB)
		{
			const Vector2 target = pInterface->NavMesh_GetClosestPathPoint(goalRightU);
			pSteeringBehavior->SetTarget(target);
			pSteeringBehavior->Seek();
		}

		// Check if he went to Right Bottom corner
		if (pHouseHelpers->leftU && 
			pHouseHelpers->leftB &&
			pHouseHelpers->rightU && agentInfo.Position.DistanceSquared(goalRightB) <= playerOffset * playerOffset)
			pHouseHelpers->rightB = true;
		else if (pHouseHelpers->rightU)
		{
			const Vector2 target = pInterface->NavMesh_GetClosestPathPoint(goalRightB);
			pSteeringBehavior->SetTarget(target);
			pSteeringBehavior->Seek();
			return Elite::BehaviorState::Success;
		}

		return Elite::BehaviorState::Failure;
	}
	Elite::BehaviorState GoOutsideHouse(Blackboard* pBlackboard)
	{
		// Get info
		MySteeringBehaviors* pSteeringBehavior;
		if (!pBlackboard->GetData("SteeringBehavior", pSteeringBehavior) || pSteeringBehavior == nullptr)
			return Elite::BehaviorState::Failure;
		HelperStructs::House* pHouseHelpers;
		if (!pBlackboard->GetData("HouseHelpers", pHouseHelpers))
			return Elite::BehaviorState::Failure;
		IExamInterface* pInterface;
		if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
			return Elite::BehaviorState::Failure;

		const AgentInfo agentInfo = pInterface->Agent_GetInfo();

		// Do logic
		const Vector2 houseCenter = { pHouseHelpers->houseInfo.Center };
		const Vector2 houseSize = { pHouseHelpers->houseInfo.Size };
		const float offset = { 5 };

		// Set target outside of house to leave
		const Vector2 goal = { houseCenter.x + houseSize.x + offset,  houseCenter.y + houseSize.y + offset };
		const Vector2 target = pInterface->NavMesh_GetClosestPathPoint(goal);
		pSteeringBehavior->SetTarget(target);
		pSteeringBehavior->Seek();

		// Check if he left the house
		if (agentInfo.IsInHouse)
			return Elite::BehaviorState::Failure;

		// Add the current house to all the houses that have been visited
		pHouseHelpers->vHousesVisited.push_back(pHouseHelpers->houseInfo);

		// Reset the house pattern
		pHouseHelpers->leftU = false;
		pHouseHelpers->leftB = false;
		pHouseHelpers->rightU = false;
		pHouseHelpers->rightB = false;

		// Reset the current house position to the non existent position
		pHouseHelpers->houseInfo.Center = ZeroVector2;

		return Elite::BehaviorState::Success;
	}

	// INVENTORY
	// INVENTORY - FOOD / ENERGY
	Elite::BehaviorState EatFood(Blackboard* pBlackboard)
	{
		// Get info
		IExamInterface* pInterface;
		if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
			return Elite::BehaviorState::Failure;

		// Do logic
		ItemInfo itemInfo;
		if (!HelperFunction::HasItem(pBlackboard, eItemType::FOOD, itemInfo))
			return Elite::BehaviorState::Failure;

		if (pInterface->Inventory_UseItem(HelperFunction::GetSlotOfItem(pBlackboard, eItemType::FOOD)))
			return Elite::BehaviorState::Success;

		return Elite::BehaviorState::Failure;
	}
	// INVENTORY - MEDKIT / HEALTH
	Elite::BehaviorState UseMedkit(Blackboard* pBlackboard)
	{
		// Get info
		IExamInterface* pInterface;
		if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
			return Elite::BehaviorState::Failure;

		// Do logic
		ItemInfo itemInfo;
		if (!HelperFunction::HasItem(pBlackboard, eItemType::MEDKIT, itemInfo))
			return Elite::BehaviorState::Failure;

		if (pInterface->Inventory_UseItem(HelperFunction::GetSlotOfItem(pBlackboard, eItemType::MEDKIT)))
			return Elite::BehaviorState::Success;

		return Elite::BehaviorState::Failure;
	}
	// INVENTORY - ENEMY / PISTOL & SHOTGUN
	Elite::BehaviorState ShootEnemy(Blackboard* pBlackboard)
	{
		// Get info
		std::vector<EntityInfo>* pEntitiesInFov;
		if (!pBlackboard->GetData("EntitiesInFov", pEntitiesInFov) || pEntitiesInFov == nullptr)
			return Elite::BehaviorState::Failure;
		IExamInterface* pInterface;
		if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
			return Elite::BehaviorState::Failure;
		MySteeringBehaviors* pSteeringBehavior;
		if (!pBlackboard->GetData("SteeringBehavior", pSteeringBehavior) || pSteeringBehavior == nullptr)
			return Elite::BehaviorState::Failure;

		const AgentInfo agentInfo = { pInterface->Agent_GetInfo() };

		// Do logic
		EntityInfo enemy;
		for (const auto& e : *pEntitiesInFov)
		{
			if (e.Type != eEntityType::ENEMY)
				continue;

			// First enemy in fov
			enemy = e;
			break;
		}

		// Set the target to flee and face
		pSteeringBehavior->SetTarget(enemy.Location);
		pSteeringBehavior->FaceFlee(10);

		// Accuracy logic
		// Vector from agent to enemy
		const Vector2 vEnemy = { enemy.Location - agentInfo.Position };
		// Get the angle of that vector
		const float angleEnemy = { std::atan2(vEnemy.y, vEnemy.x) };
		// Difference between the facing of the agent and the angle of the enemy vector
		const float angleDifference = { std::abs(agentInfo.Orientation - angleEnemy) };
		// Return failure when angle accuracy is too big
		if (angleDifference > .1f)
			return Elite::BehaviorState::Failure;

		ItemInfo pistolInfo;
		ItemInfo shotgunInfo;
		// No items so can't shoot
		if (!HelperFunction::HasItem(pBlackboard, eItemType::PISTOL, pistolInfo) ||
			!HelperFunction::HasItem(pBlackboard, eItemType::SHOTGUN, shotgunInfo))
				return Elite::BehaviorState::Failure;

		// When pistol or shotgun is empty
		if (pInterface->Weapon_GetAmmo(pistolInfo) <= 0)
			pInterface->Inventory_RemoveItem(HelperFunction::GetSlotOfItem(pBlackboard, eItemType::PISTOL));
		if (pInterface->Weapon_GetAmmo(shotgunInfo) <= 0)
			pInterface->Inventory_RemoveItem(HelperFunction::GetSlotOfItem(pBlackboard, eItemType::SHOTGUN));

		// When the enemy is still far
		if (agentInfo.Position.DistanceSquared(enemy.Location) >= agentInfo.FOV_Range / 3 &&
			HelperFunction::HasItem(pBlackboard, eItemType::PISTOL, pistolInfo))
		{
			if(pInterface->Inventory_UseItem(HelperFunction::GetSlotOfItem(pBlackboard, eItemType::PISTOL)))
				return Elite::BehaviorState::Success;
		}
		// When the enemy is getting close
		if (HelperFunction::HasItem(pBlackboard, eItemType::SHOTGUN, shotgunInfo))
		{
			if (pInterface->Inventory_UseItem(HelperFunction::GetSlotOfItem(pBlackboard, eItemType::SHOTGUN)))
				return Elite::BehaviorState::Success;
		}

		return Elite::BehaviorState::Failure;
	}
}

namespace BT_Conditions
{
	// PURGEZONE
	bool IsInPurgeZone(Blackboard* pBlackboard)
	{
		// Get info
		std::vector<EntityInfo>* pEntitiesInFov;
		if (!pBlackboard->GetData("EntitiesInFov", pEntitiesInFov) || pEntitiesInFov == nullptr)
			return false;
		IExamInterface* pInterface;
		if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
			return false;

		// Do logic
		const AgentInfo agentInfo = pInterface->Agent_GetInfo();
		for(auto& e : *pEntitiesInFov)
		{
			// If type PURGEZONE
			if(e.Type == eEntityType::PURGEZONE)
			{
				// Get PurgeZone info
				PurgeZoneInfo zoneInfo;
				pInterface->PurgeZone_GetInfo(e, zoneInfo);
				// When inside purge, return true, else false
				if (zoneInfo.Radius * zoneInfo.Radius <	agentInfo.Position.DistanceSquared(e.Location))
					return false;
				else
					return true;
			}
		}
		return false;
	}

	// LOOT
	bool IsLootInFov(Blackboard* pBlackboard)
	{
		// Get info
		std::vector<EntityInfo>* pEntitiesInFov;
		if (!pBlackboard->GetData("EntitiesInFov", pEntitiesInFov) || pEntitiesInFov == nullptr)
			return false;

		// Do logic
		if (pEntitiesInFov->empty())
			return false;

		for(auto& e : *pEntitiesInFov)
		{
			if (e.Type == eEntityType::ITEM)
				return true;

			return false;
		}
		return true;
	}

	// HOUSES
	bool FoundNewHouse(Blackboard* pBlackboard)
	{
		// Get info
		std::vector<HouseInfo>* pHousesInFov;
		if (!pBlackboard->GetData("HousesInFov", pHousesInFov) || pHousesInFov == nullptr)
			return false;
		HelperStructs::House* pHouseHelpers;
		if (!pBlackboard->GetData("HouseHelpers", pHouseHelpers))
			return false;

		// Do logic

		// No houses in fov
		if (pHousesInFov->empty())
			return false;

		// Check if we already have a current house
		// Houses will never spawn on {0, 0}
		if (pHouseHelpers->houseInfo.Center != ZeroVector2)
			return true;

		// Check if house had been visited before
		for(const auto& visitedHouse : pHouseHelpers->vHousesVisited)
		{
			if (pHousesInFov->begin()->Center == visitedHouse.Center)
				return false;
		}

		// Set new house as the current house
		pHouseHelpers->houseInfo = *pHousesInFov->begin();
		return true;
	}
	bool IsInHouse(Blackboard* pBlackboard)
	{
		// Get info
		IExamInterface* pInterface;
		if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
			return false;
		HelperStructs::House* pHouseHelpers;
		if (!pBlackboard->GetData("HouseHelpers", pHouseHelpers))
			return false;

		// Do logic

		// Do we have a current house
		// Fail safe if pushed out of house by purge zone
		if (pHouseHelpers->houseInfo.Center == ZeroVector2)
			return false;

		// Return if we are in a house
		return pInterface->Agent_GetInfo().IsInHouse;
	}
	bool HouseChecked(Blackboard* pBlackboard)
	{
		// Get info
		HelperStructs::House* pHouseHelpers;
		if (!pBlackboard->GetData("HouseHelpers", pHouseHelpers))
			return false;

		// Do logic
		// The last corner of the house has been checked
		if (pHouseHelpers->rightB)
			return true;

		return false;
	}

	// INVENTORY
	// INVENTORY - FOOD / ENERGY
	bool LowEnergy(Blackboard* pBlackboard)
	{
		// Get info
		IExamInterface* pInterface;
		if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
			return false;

		const AgentInfo agentInfo = { pInterface->Agent_GetInfo() };

		// Do logic
		if (agentInfo.Energy >= 4)
			return false;

		return true;
	}
	bool HasFood(Blackboard* pBlackboard)
	{
		// Get info
		IExamInterface* pInterface;
		if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
			return false;

		// Do logic
		ItemInfo itemInfo;
		if (HelperFunction::HasItem(pBlackboard, eItemType::FOOD, itemInfo))
			return true;

		return false;
	}
	// INVENTORY - MEDKIT / HEALTH
	bool LowHealh(Blackboard* pBlackboard)
	{
		// Get info
		IExamInterface* pInterface;
		if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
			return false;

		const AgentInfo agentInfo = { pInterface->Agent_GetInfo() };

		// Do logic
		if (agentInfo.Health >= 6)
			return false;

		return true;
	}
	bool HasMedkit(Blackboard* pBlackboard)
	{
		// Get info
		IExamInterface* pInterface;
		if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
			return false;

		// Do logic
		ItemInfo itemInfo;
		if (HelperFunction::HasItem(pBlackboard, eItemType::MEDKIT, itemInfo))
			return true;

		return false;
	}
	// INVENTORY - ENEMY / PISTOL & SHOTGUN
	bool EnemyInFov(Blackboard* pBlackboard)
	{
		// Get info
		std::vector<EntityInfo>* pEntitiesInFov;
		if (!pBlackboard->GetData("EntitiesInFov", pEntitiesInFov) || pEntitiesInFov == nullptr)
			return false;

		// Do logic
		for(const auto& e : *pEntitiesInFov)
		{
			if (e.Type == eEntityType::ENEMY)
				return true;
		}
		return false;
	}
}
#endif