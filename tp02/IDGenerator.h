#pragma once

#include <assert.h>
#include <vector>

#include "ISingleton.h"

struct ID
{
	int generation;
	int index;

	ID(int _index, int _generation) : index(_index), generation(_generation)
	{}

	void reset()
	{
		index = -1;
		generation = -1;
	}

	bool operator==(const ID& other)
	{
		return generation == other.generation && index == other.index;
	}
};

class IDGenerator : public ISingleton<IDGenerator>
{
private:
	std::vector<bool> m_used;
	std::vector<int> m_generation;
	int m_lastIndex;
public:
	IDGenerator()
		: m_lastIndex(0)
	{
		m_used.resize(100, false);
		m_generation.resize(100, 0);
	}

	ID lockID()
	{
		auto& itFirstUnsused =  std::find(m_used.begin(), m_used.end(), false);
		if (itFirstUnsused != m_used.end())
		{
			int idFirstUnused = std::distance(m_used.begin(), itFirstUnsused);
			m_generation[idFirstUnused]++;
			*itFirstUnsused = true;
			return ID(idFirstUnused, m_generation[idFirstUnused]);
		}
		else
		{
			m_generation.push_back(0);
			m_used.push_back(true);
			m_lastIndex++;
			assert(m_lastIndex == m_generation.size() && m_lastIndex == m_used.size());
			return ID(m_lastIndex - 1, m_generation.back());
		}
	}

	void freeID(ID& id)
	{
		assert(m_used.size() < id.index && m_generation.size() < id.index);
		m_used[id.index] = false;
	}
};

