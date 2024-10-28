#pragma once
#include <stack>
#include <map>
#include <vector>

/*-----------------------
* DeadLockProfiler
-----------------------*/

class DeadLockProfiler
{
public:
	void PushLock(const char* name);
	void PopLock(const char* name);
	void CheckCycle();

private:
	void DFS(int32 here);

private:
	std::unordered_map<const char*, int32>	_nameToId;
	std::unordered_map<int32, const char*>	_IdeToName;
	std::stack<int32>						_lockStack;
	std::map<int32, set<int32>>				_lockHistory;

	Mutex									_lock;

private:
	std::vector<int32>						_discoveredOrder; // ��尡 �߰ߵ� ������ ����ϴ� �迭
	int32									_discoveredCount = 0; // ��尡 �߰ߵ� ����
	vector<bool>							_finished; // dfs(i) ���� �Ǿ����� ����
	vector<int32>							_parent;
};
