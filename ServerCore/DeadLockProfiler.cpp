#include "pch.h"
#include "DeadLockProfiler.h"

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_lock);

	// ���̵� ã�ų� �߱��Ѵ�.
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

	// ����ִ� ���� �־��ٸ�
	if (false == LLockStack.empty())
	{
		//������ �߰ߵ��� ���� ���̽���� ����� ���� �ٽ� Ȯ���Ѵ�.
		const int32 prevId = LLockStack.top();
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

	LLockStack.push(lockID);
}

void DeadLockProfiler::PopLock(const char* name)
{
	LockGuard guard(_lock);

	if (LLockStack.empty())
	{
		CRASH("MULTIPLE_UNLOCK");
	}

	int32 lockID = _nameToId[name];
	if (LLockStack.top() != lockID)
	{
		CRASH("MULTIPLE_UNLOCK");
	}

	LLockStack.pop();
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

	// ������ �������� �����Ѵ�.
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

	// ��� ������ ��������ȸ �Ѵ�.
	auto findIt = _lockHistory.find(here);
	if (findIt == _lockHistory.end())
	{
		_finished[here] == true;
		return;
	}

	set<int32>& nextSet = findIt->second;

	for (int32 there : nextSet)
	{
		//���� �湮������ ���ٸ� �湮�Ѵ�.
		if (_discoveredOrder[there] == -1)
		{
			_parent[there] = here;
			DFS(there);
			continue;
		}

		// here�� there���� ���� �߰ߵǾ��ٸ�, there�� here�� �ļ��̴�. (������ ����)
		if (_discoveredOrder[here] < _discoveredOrder[there])
		{
			continue;
		}

		// �������� �ƴϰ�, Dfs(there)�� ���� �������� �ʤ��Ҵٸ�  there�� here�� �����̴�. (������ ����)
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
