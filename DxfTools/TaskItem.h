#pragma once
class TaskItem
{
public:
	TaskItem(DWORD TaskObject);
	~TaskItem();
	DWORD GetTaskId();
	DWORD GetTaskType();
	DWORD GetTaskGetLevel();
	DWORD GetTaskFinishLevel();
	wchar_t* GetTaskName();
	DWORD GetMapId();
	wchar_t* GetSeek(); //��ȡ����

	DWORD GetTaskMaterialCount();
private:
	DWORD m_TaskObject;
};

