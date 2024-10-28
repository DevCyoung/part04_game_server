#include "pch.h"
#include "DeadLockProfiler.h"

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_lock);

	// 아이디를 찾거나 발급한다.
	int32 lockID = 0;

	auto findIt = _nameToId.find(name);

	if (findIt == _nameToId.end())
	{
		lockID = static_cast<int32>(_nameToId.size());
		_nameToId[name] = lockID;
		_IdeToName[lockID] = name;
	}
	else
	{
		lockID = findIt->second;
	}

	// 잡고있는 락이 있었다면
	if (false == _lockStack.empty())
	{
		//기존에 발견되지 않은 케이스라면 데드락 여부 다시 확인한다.
		const int32 prevId = _lockStack.top();
		if (lockID != prevId)
		{
			set<int32>& history = _lockHistory[prevId];
			if (history.find(lockID) == history.end())
			{
				history.insert(lockID);
				CheckCycle();
			}
		}
	}

	_lockStack.push(lockID);
}

void DeadLockProfiler::PopLock(const char* name)
{
	LockGuard guard(_lock);

	if (_lockStack.empty())
	{
		CRASH("MULTIPLE_UNLOCK");
	}

	int32 lockID = _nameToId[name];
	if (_lockStack.top() != lockID)
	{
		CRASH("MULTIPLE_UNLOCK");
	}

	_lockStack.pop();
}

void DeadLockProfiler::CheckCycle()
{
	const int32 lockCount = static_cast<int32>(_nameToId.size());
	_discoveredOrder = std::vector<int32>(lockCount, -1);
	_discoveredCount = 0;
	_finished = std::vector<bool>(lockCount, false);
	_parent = std::vector<int32>(lockCount, -1);

	for (int32 lockID = 0; lockID < lockCount; ++lockID)
	{
		DFS(lockID);
	}

	// 연산이 끝났으면 정리한다.
	_discoveredOrder.clear();
	_finished.clear();
	_parent.clear();
}

void DeadLockProfiler::DFS(int32 here)
{
	if (_discoveredOrder[here] != -1)
	{
		return;
	}

	_discoveredOrder[here] = _discoveredCount++;

	// 모든 인접한 정점을순회 한다.
	auto findIt = _lockHistory.find(here);
	if (findIt == _lockHistory.end())
	{
		_finished[here] == true;
		return;
	}

	set<int32>& nextSet = findIt->second;

	for (int32 there : nextSet)
	{
		//아직 방문한적이 없다면 방문한다.
		if (_discoveredOrder[there] == -1)
		{
			_parent[there] = here;
			DFS(there);
			continue;
		}

		// here가 there보다 먼저 발견되었다면, there는 here의 후손이다. (순방향 간선)
		if (_discoveredOrder[here] < _discoveredOrder[there])
		{
			continue;
		}

		// 순방향이 아니고, Dfs(there)가 아직 종료하지 않ㄹ았다면  there는 here의 선조이다. (역방향 간선)
		if (false == _finished[there])
		{
			printf("%s-> %s\n", _IdeToName[here], _IdeToName[there]);

			int32 now = here;
			while (true)
			{
				printf("%s -> %s \n", _IdeToName[_parent[now]], _IdeToName[now]);
				now = _parent[now];
				if (now == there)
				{
					break;
				}
			}

			CRASH("DEADLOCK_DETECTED");
		}
	}

	_finished[here] = true;
}
