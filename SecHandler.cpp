#include "SecHandler.h"
#include <chrono>

SecHandler::SecHandler() : task_executer(execute, this)
{
	
}

SecHandler::~SecHandler()
{
	task_executer.detach();

	// Wait until executer thread exits to avoid 0xC0000005, prob. there is a better way to handle this.
	thread_mutex.lock();
	thread_mutex.unlock();
}

void SecHandler::execute(SecHandler* i)
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

void SecHandler::search(std::string packet)
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

void SecHandler::add_filter(std::string name, std::string filter)
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

void SecHandler::on(std::string name, std::function<void(std::string packet)> callback)
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

int SecHandler::task_count()
{
	std::lock_guard<std::mutex> m(task_mutex);
	return tasks.size();
}

task SecHandler::get_task()
{
	std::lock_guard<std::mutex> m(task_mutex);
	return tasks.front();
}

void SecHandler::add_task(std::string packet, ops& o)
{
	task_mutex.lock();
	tasks.push_back(task(packet, o));
	task_mutex.unlock();
}

void SecHandler::remove_task()
{
	task_mutex.lock();
	tasks.pop_front();
	task_mutex.unlock();
}
