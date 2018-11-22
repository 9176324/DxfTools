#include "stdafx.h"
#include "TaskManager.h"
#include "baseaddr.h"
#include "utils.h"
#include "GameCall.h"
#include "GameManager.h"

std::unique_ptr<TaskManager> TaskManager::m_pInstance(new TaskManager());
TaskManager::TaskManager()
{
}


TaskManager::~TaskManager()
{
}

TaskManager * TaskManager::GetInstance()
{
	return m_pInstance.get();
}

DWORD TaskManager::GetTaskItem()
{
	DWORD TaskTravseBase = utils::ReadValue<DWORD>(Base_TaskTravseAddr);
	if (!TaskTravseBase) {
		return 0;
	}

	DWORD StartAddr = utils::ReadValue<DWORD>(TaskTravseBase + Base_TaskTravseStartAddr);
	DWORD EndAddr = utils::ReadValue<DWORD>(TaskTravseBase + Base_TaskTravseEndAddr);
	if (!StartAddr || !EndAddr) {
		return 0;
	}

	DWORD TaskCount = (EndAddr - StartAddr) >> 2;

	for (DWORD i = 0; i < TaskCount; i++)
	{
		DWORD TaskObject = utils::ReadValue<DWORD>(StartAddr + i * 4);
		if (!TaskObject) {
			continue;
		}

		TaskItem item(TaskObject);
		if (item.GetTaskType() == 0)
		{
			return TaskObject;
		}
	}

	return 0;
}

DWORD g_nRet2SelectRole = 0;
void TaskManager::AutoTask()
{

	while (true)
	{
		GameCall::RandomSleep();

	/*	if (GameCall::IsTask())
		{
			GameCall::MemKeyBorad(326, true);
			Sleep(500);
			continue;
		}*/

		//��ȡ�������,���û������������ж��ǿ�������û������
		DWORD TaskObject = GetTaskItem();
		if (0 == TaskObject)
		{
			if (g_nRet2SelectRole > 0)
			{
				//GameCall::Bulletin(L"û���ҵ��������񣬿�ʼ��ǰ��ߵȼ�����");
				DWORD MapId = GameCall::GetUpLevelMapId();
				GameCall::RandomSleep();
				GameCall::EreaCall(MapId);
				GameCall::RandomSleep();
				GameCall::SelectMap();
				GameCall::RandomSleep();
				DWORD Easy = GetMapCurDiff(MapId);
				GameCall::EnterMap(MapId, Easy);
				GameCall::RandomSleep();
			
			}
			else {
				g_nRet2SelectRole++;

				GameCall::ReturnToSelectRole();
				//GameCall::Bulletin(L"û���ҵ��������񣬷���ѡ���ɫ");
				GameCall::RandomSleep();
			}
			return;
		}

		g_nRet2SelectRole = 0;
		TaskItem item(TaskObject);

		//{
		//	std::wstringstream ss;
		//	ss << L"<����> - �������� [ " << item.GetTaskName() << L" ] ������ " << item.GetTaskId() << L" �������� "<< item.GetSeek();
		//	GameCall::Bulletin(ss.str());
		//	Sleep(600);
		//}

		if (item.GetTaskId() > 0)
		{

			//61��������
			std::wstring name = item.GetTaskName();
			if (name.find(L"���غն����") != std::wstring::npos)
			{
				if (GameCall::GetPlayerLevel() < 63)
				{
					DWORD MapId = GameCall::GetUpLevelMapId();
					GameCall::RandomSleep();
					GameCall::EreaCall(MapId);
					GameCall::RandomSleep();
					GameCall::SelectMap();
					GameCall::RandomSleep();
					DWORD Easy = GetMapCurDiff(MapId);
					GameCall::EnterMap(MapId, Easy);
					GameCall::RandomSleep();
				}
				else {
					GameCall::Bulletin(L"�ֶ���������");
				}

				return;
			}

			DelCommitTask();
			GameCall::RandomSleep();
			GameCall::TaskCommit(item.GetTaskId());
			GameCall::RandomSleep();

			//������������ϸ����񻺴��ύ
			if (!IsSameTask(item.GetTaskId())) 
			{

				/*{
					std::wstringstream ss;
					ss << L"<����> - �������ֲ�ͬ���л���һ������";
					GameCall::Bulletin(ss.str());
					Sleep(600);
				}*/

				TaskObject = GetTaskItem();
				if (TaskObject) {
					TaskItem itm(TaskObject);
					GameCall::TaskAccept(itm.GetTaskId());
					GameCall::RandomSleep();
				}
				
				continue;
			}
		


			std::wstring Text = std::wstring(item.GetSeek()/*, 25*/);
			if (Text.find(L"hunt enemy") != std::wstring::npos ||
				Text.find(L"meet npc") != std::wstring::npos ||
				Text.find(L"hunt monster") != std::wstring::npos ||
				Text.find(L"seek n meet npc") != std::wstring::npos ||
				Text.find(L"question") != std::wstring::npos ||
				Text.find(L"quest clear") != std::wstring::npos ||
				Text.find(L"reach the range") != std::wstring::npos ||
				Text.find(L"look cinematic") != std::wstring::npos)
			{

				DWORD nTimes = GetNTimes();
				if (nTimes == 512)
				{
					nTimes = 1;
				}

				std::vector<DWORD>  list;

				auto first = nTimes % 512;
				auto second = nTimes / 512;
				auto three = nTimes / 262144;

				list.push_back(first > 100 ? 1 : first);
				list.push_back(second > 100 ? 1 : second);
				list.push_back(three > 100 ? 1 : three);

				DWORD elm = *(std::max_element(list.begin(), list.end()));

				/*DelCommitTask();
				Sleep(500);
				GameCall::TaskCommit(item.GetTaskId());
				Sleep(600);*/

				if (elm == 0)
				{
					elm = 1;
				}

				if(elm > 100){

					elm = 1;
					{
						std::wstringstream ss;
						ss << L"<����> - nTimes " << nTimes << L" ����[0] " << list[0] << L" ����[1] " << list[1] << L" ����[2] " << list[2] << L" ������ " << elm;
						GameCall::Bulletin(ss.str());
						Sleep(500);
					}
				}
				
				for (auto i = 0; i != elm; i++)
				{
					GameCall::TaskFinish(item.GetTaskId(), 1);
					GameCall::RandomSleep();
				}

				{
					std::wstringstream ss;
					ss << L"<����> - ��ɶԻ� [ " << item.GetTaskName() << L" ]";
					GameCall::Bulletin(ss.str());
					GameCall::RandomSleep();
				}

				GameCall::TaskCommit(item.GetTaskId());
				GameCall::RandomSleep();
				return;
			}

			if (item.GetMapId() > 0)
			{
				if (Text.find(L"condition under clear") != std::wstring::npos ||
					Text.find(L"seeking") != std::wstring::npos ||
					Text.find(L"clear map") != std::wstring::npos)
				{
					{
						std::wstringstream ss;
						ss << L"<����> - �ƶ������� [ " << item.GetMapId() << L" ]";
						GameCall::Bulletin(ss.str());
						GameCall::RandomSleep();
					}
					GameCall::RandomSleep();
					GameCall::EreaCall(item.GetMapId());
					GameCall::RandomSleep();
					GameCall::SelectMap();
					GameCall::RandomSleep();
					GameCall::EnterMapForTask(item.GetMapId(), 0, item.GetTaskId());
					GameCall::RandomSleep();

					return;
				}
			}


			if (Text.find(L"seeking") != std::wstring::npos)
			{
				GameCall::Bulletin(L"<����> - ��ǰ������Ҫ�������");
				GameCall::RandomSleep();
				return;
			}

		}
	}
}

void TaskManager::DelCommitTask()
{

	DWORD TaskTravseBase = utils::ReadValue<DWORD>(Base_TaskTravseAddr);
	if (!TaskTravseBase) {
		return ;
	}

	DWORD StartAddr = utils::ReadValue<DWORD>(TaskTravseBase + 8);
	DWORD EndAddr = utils::ReadValue<DWORD>(TaskTravseBase + 12);
	if (!StartAddr || !EndAddr) {
		return ;
	}


	DWORD count = (EndAddr - StartAddr) / 12;

	for (auto i = 0; i != count; i++)
	{
		DWORD object = utils::ReadValue<DWORD>(StartAddr + i * 12);
		TaskItem item(object);
		auto skr = GameCall::Dectypt(StartAddr + i * 12 + 4);
		if (item.GetTaskType() == 0 && skr == 0) {
			GameCall::TaskCommit(item.GetTaskId());
			GameCall::RandomSleep();
		}
	}
}


DWORD TaskManager::GetNTimes()
{

	DWORD TaskTravseBase = utils::ReadValue<DWORD>(Base_TaskTravseAddr);
	if (!TaskTravseBase) {
		return 0;
	}

	DWORD StartAddr = utils::ReadValue<DWORD>(TaskTravseBase + 8);
	DWORD EndAddr = utils::ReadValue<DWORD>(TaskTravseBase + 12);
	if (!StartAddr || !EndAddr) {
		return 0;
	}


	DWORD count = (EndAddr - StartAddr) / 12;

	for (auto i = 0; i != count; i++)
	{
		DWORD object = utils::ReadValue<DWORD>(StartAddr + i * 12);
		TaskItem item(object);

		if (item.GetTaskType() == 0) {
			return GameCall::Dectypt(StartAddr + i * 12 + 4);
		}
	}

	return 0;
}

bool TaskManager::IsSameTask(DWORD TaskId)
{
	DWORD TaskTravseBase = utils::ReadValue<DWORD>(Base_TaskTravseAddr);
	if (!TaskTravseBase) {
		return false;
	}

	DWORD StartAddr = utils::ReadValue<DWORD>(TaskTravseBase + 8);
	DWORD EndAddr = utils::ReadValue<DWORD>(TaskTravseBase + 12);
	if (!StartAddr || !EndAddr) {
		return false;
	}


	DWORD count = (EndAddr - StartAddr) / 12;

	for (auto i = 0; i != count; i++)
	{
		DWORD object = utils::ReadValue<DWORD>(StartAddr + i * 12);
		TaskItem item(object);

		if (item.GetTaskType() == 0) {
			return (item.GetTaskId() == TaskId);
		}
	}

	return false;
}
