#include "stdafx.h"
#include "Plugin.h"
#include "IExamInterface.h"

using namespace std;
using namespace Elite;

//Called only once, during initialization
void Plugin::Initialize(IBaseInterface* pInterface, PluginInfo& info)
{
	//Retrieving the interface
	//This interface gives you access to certain actions the AI_Framework can perform for you
	m_pInterface = static_cast<IExamInterface*>(pInterface);

	//Bit information about the plugin
	//Please fill this in!!
	info.BotName = "Survivor";
	info.Student_FirstName = "Jentl";
	info.Student_LastName = "Vandorpe";
	info.Student_Class = "2DAE08";

	// BEHAVIOR TREE
	// Create and add the necessary blackboard data
	m_pSteeringOutput = new SteeringPlugin_Output();
	m_pSteeringBehavior = new MySteeringBehaviors(m_pSteeringOutput, m_pInterface);
	m_pEntitiesInFov = new std::vector<EntityInfo>();
	m_pHousesInFov = new std::vector<HouseInfo>();
	m_pExplorationHelpers = new HelperStructs::Exploration();
	m_pHouseHelpers = new HelperStructs::House();
	// 1. Create Blackboard
	Blackboard* pBlackboard = new Blackboard();
	pBlackboard->AddData("Interface", m_pInterface);
	pBlackboard->AddData("SteeringBehavior", m_pSteeringBehavior);
	pBlackboard->AddData("EntitiesInFov", m_pEntitiesInFov);
	pBlackboard->AddData("HousesInFov", m_pHousesInFov);
	pBlackboard->AddData("ExplorationHelpers", m_pExplorationHelpers);
	pBlackboard->AddData("HouseHelpers", m_pHouseHelpers);

	// 2. Create BehaviorTree (make more conditions/actions and create a more advanced tree than the simple agents
	m_pBehaviorTree = new BehaviorTree(pBlackboard,
		new BehaviorSelector(
			{
				// Flee from PurgeZone
				new BehaviorSequence({
					new BehaviorConditional(BT_Conditions::IsInPurgeZone),
					new BehaviorAction(BT_Actions::ChangeToFleePurge)
				}),
				// Enemy in fov
				new BehaviorSequence({
					new BehaviorConditional(BT_Conditions::EnemyInFov),
					new BehaviorAction(BT_Actions::ShootEnemy)
				}),
				// Loot items if in FOV
				new BehaviorSequence({
					new BehaviorConditional(BT_Conditions::IsLootInFov),
					new BehaviorAction(BT_Actions::LootItems)
				}),
				// Low energy
				new BehaviorSequence({
					new BehaviorConditional(BT_Conditions::LowEnergy),
					new BehaviorConditional(BT_Conditions::HasFood),
					new BehaviorAction(BT_Actions::EatFood)
				}),
				// Low Health
				new BehaviorSequence({
					new BehaviorConditional(BT_Conditions::LowHealh),
					new BehaviorConditional(BT_Conditions::HasMedkit),
					new BehaviorAction(BT_Actions::UseMedkit)
				}),
				// House logic
				new BehaviorSequence({
					// Go in to new house
					new BehaviorSequence({
						new BehaviorConditional(BT_Conditions::FoundNewHouse),
						new BehaviorAction(BT_Actions::GoInsideHouse)
					}),
					// Check whole house
					new BehaviorSequence({
						new BehaviorConditional(BT_Conditions::IsInHouse),
						new BehaviorAction(BT_Actions::CheckInsideHouse)
					}),
					// Has the house been checked
					new BehaviorSequence({
						new BehaviorConditional(BT_Conditions::HouseChecked),
						new BehaviorAction(BT_Actions::GoOutsideHouse)
					})
				}),
				// Fallback
				new BehaviorAction(BT_Actions::ChangeToSeek)
			}
	));

}
//Called only once
void Plugin::DllInit()
{
	//Called when the plugin is loaded
}
//Called only once
void Plugin::DllShutdown()
{
	//Called wheb the plugin gets unloaded
}

//Called only once, during initialization
void Plugin::InitGameDebugParams(GameDebugParams& params)
{
	params.AutoFollowCam = true; //Automatically follow the AI? (Default = true)
	params.RenderUI = true; //Render the IMGUI Panel? (Default = true)
	params.SpawnEnemies = true; //Do you want to spawn enemies? (Default = true)
	params.EnemyCount = 20; //How many enemies? (Default = 20)
	params.GodMode = false; //GodMode > You can't die, can be useful to inspect certain behaviors (Default = false)
	params.LevelFile = "GameLevel.gppl";
	params.AutoGrabClosestItem = true; //A call to Item_Grab(...) returns the closest item that can be grabbed. (EntityInfo argument is ignored)
	params.StartingDifficultyStage = 1;
	params.InfiniteStamina = false;
	params.SpawnDebugPistol = true;
	params.SpawnDebugShotgun = true;
	params.SpawnPurgeZonesOnMiddleClick = true;
	params.PrintDebugMessages = true;
	params.ShowDebugItemNames = true;
	params.Seed = 36;
}

//Only Active in DEBUG Mode
//(=Use only for Debug Purposes)
void Plugin::Update(float dt)
{
	//Demo Event Code
	//In the end your AI should be able to walk around without external input
	if (m_pInterface->Input_IsMouseButtonUp(Elite::InputMouseButton::eLeft))
	{
		//Update target based on input
		Elite::MouseData mouseData = m_pInterface->Input_GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft);
		const Elite::Vector2 pos = Elite::Vector2(static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y));
		m_Target = m_pInterface->Debug_ConvertScreenToWorld(pos);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Space))
	{
		m_CanRun = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Left))
	{
		m_AngSpeed -= Elite::ToRadians(10);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Right))
	{
		m_AngSpeed += Elite::ToRadians(10);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_G))
	{
		m_GrabItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_U))
	{
		m_UseItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_R))
	{
		m_RemoveItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyUp(Elite::eScancode_Space))
	{
		m_CanRun = false;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Delete))
	{
		m_pInterface->RequestShutdown();
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_KP_Minus))
	{
		if (m_InventorySlot > 0)
			--m_InventorySlot;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_KP_Plus))
	{
		//if (m_InventorySlot < 4)
		//	++m_InventorySlot;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Q))
	{
		ItemInfo info = {};
		m_pInterface->Inventory_GetItem(*m_InventorySlot, info);
		std::cout << (int)info.Type << std::endl;
	}
}

//Update
//This function calculates the new SteeringOutput, called once per frame
SteeringPlugin_Output Plugin::UpdateSteering(float dt)
{

	// ************************
	// STEERING OUTPUT
	auto steering = new SteeringPlugin_Output();
	
	//Use the Interface (IAssignmentInterface) to 'interface' with the AI_Framework
	auto agentInfo = m_pInterface->Agent_GetInfo();


	//Use the navmesh to calculate the next navmesh point
	Vector2 checkpointLocation{};

	//OR, Use the mouse target
	auto nextTargetPos = m_pInterface->NavMesh_GetClosestPathPoint(m_Target); //Uncomment this to use mouse position as guidance
	m_pSteeringBehavior->SetTarget(nextTargetPos);
	m_pBehaviorTree->Update(dt);

	*m_pHousesInFov = GetHousesInFOV();//uses m_pInterface->Fov_GetHouseByIndex(...)
	*m_pEntitiesInFov = GetEntitiesInFOV(); //uses m_pInterface->Fov_GetEntityByIndex(...)

	//steering.AngularVelocity = m_AngSpeed; //Rotate your character to inspect the world while walking
	steering->AutoOrient = true; //Setting AutoOrient to TRue overrides the AngularVelocity

	steering->RunMode = m_CanRun; //If RunMode is True > MaxLinSpd is increased for a limited time (till your stamina runs out)

	//SteeringPlugin_Output is works the exact same way a SteeringBehaviour output
	//auto nextTargetPos = m_pInterface->NavMesh_GetClosestPathPoint(checkpointLocation);
//@End (Demo Purposes)
	m_GrabItem = false; //Reset State
	m_UseItem = false;
	m_RemoveItem = false;

	steering = m_pSteeringOutput;
	return *steering;
}

//This function should only be used for rendering debug elements
void Plugin::Render(float dt) const
{
	//This Render function should only contain calls to Interface->Draw_... functions
	m_pInterface->Draw_SolidCircle(m_Target, .7f, { 0,0 }, { 1, 0, 0 });
}

vector<HouseInfo> Plugin::GetHousesInFOV() const
{
	vector<HouseInfo> vHousesInFOV = {};

	HouseInfo hi = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetHouseByIndex(i, hi))
		{
			vHousesInFOV.push_back(hi);
			continue;
		}

		break;
	}

	return vHousesInFOV;
}

vector<EntityInfo> Plugin::GetEntitiesInFOV() const
{
	vector<EntityInfo> vEntitiesInFOV = {};

	EntityInfo ei = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetEntityByIndex(i, ei))
		{
			vEntitiesInFOV.push_back(ei);
			continue;
		}

		break;
	}

	return vEntitiesInFOV;
}