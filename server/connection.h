#pragma once

#include <atomic>
#include <chrono>

using connectionID = uint32_t;

struct ConnectionIDGenerator
{
	connectionID get_new_ID()
	{
		return nextID++;
	}
private:
	connectionID nextID = 0;

};

struct Connection
{
	connectionID id;
	std::atomic_bool confirmed;
	std::chrono::steady_clock::time_point tod;

	Connection(connectionID id, bool confirmed, std::chrono::steady_clock::time_point tod)
	:	id {id},
		confirmed {confirmed},
		tod {tod}
	{
	}
};
