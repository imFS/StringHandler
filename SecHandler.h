#ifndef SH_H
#define SH_H
#include <string>
#include <list>
#include <functional>
#include <vector>
#include <mutex>
#include <thread>

struct ops {
	std::string name;
	std::vector<std::string> filters;
	std::function<void(std::string packet)> f = NULL;
};

struct task {
	std::string packet;
	ops& o;

public:
	task(std::string p, ops& op) : packet(p), o(op) {
	}
};

class SecHandler
{
public:
	SecHandler();
	~SecHandler();
	
	static void execute(SecHandler* i);
	void search(std::string packet);
	void add_filter(std::string name, std::string filter);
	void on(std::string name, std::function<void(std::string packet)> callback);
private:
	std::thread task_executer;
	int task_count();
	task get_task();
	void add_task(std::string packet, ops& o);
	void remove_task();

	std::list<ops> filters;
	std::list<task> tasks;
	std::mutex task_mutex;
	std::mutex thread_mutex;
};

#endif