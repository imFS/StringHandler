#include "StringHandler.h"
#include <chrono>

StringHandler::StringHandler() : task_executer(execute, this)
{
	
}

StringHandler::~StringHandler()
{
	task_executer.detach();

	// Wait until executer thread exits to avoid 0xC0000005, prob. there is a better way to handle this.
	thread_mutex.lock();
	thread_mutex.unlock();
}

void StringHandler::execute(StringHandler* i)
{
	i->thread_mutex.lock();
	while (i->task_executer.joinable())
	{
		if (i->task_count() > 0)
		{
			task t = i->get_task();
			t.o.f(t.packet);
			i->remove_task();
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	i->thread_mutex.unlock();
}

void StringHandler::search(std::string packet)
{
	for (ops& f : filters)
	{
		for (std::string prefix : f.filters)
		{
			if (packet._Starts_with(prefix))
			{
				if (f.f != NULL)
					add_task(packet, f);
			}
		}
	}
}

void StringHandler::add_filter(std::string name, std::string filter)
{
	for (ops& f : filters)
	{
		if (f.name.compare(name) == 0)
		{
			f.filters.push_back(filter);
			return;
		}
	}

	ops f;
	f.name = name;
	f.filters.push_back(filter);

	filters.push_back(f);
}

void StringHandler::on(std::string name, std::function<void(std::string packet)> callback)
{
	// Assign callback function to filter
	for (ops& f : filters)
	{
		if (f.name.compare(name) == 0)
		{
			f.f = callback;
			return;
		}
	}

	ops f;
	f.name = name;
	f.f = callback;

	filters.push_back(f);
}

int StringHandler::task_count()
{
	std::lock_guard<std::mutex> m(task_mutex);
	return tasks.size();
}

task StringHandler::get_task()
{
	std::lock_guard<std::mutex> m(task_mutex);
	return tasks.front();
}

void StringHandler::add_task(std::string packet, ops& o)
{
	task_mutex.lock();
	tasks.push_back(task(packet, o));
	task_mutex.unlock();
}

void StringHandler::remove_task()
{
	task_mutex.lock();
	tasks.pop_front();
	task_mutex.unlock();
}
