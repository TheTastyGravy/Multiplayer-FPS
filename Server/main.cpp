#include "CustomServer.h"
#include <thread>


int main()
{
	CustomServer server;
	server.startup("192.168.56.1", 5456);

	while (true)
	{
		server.update();
		// Run at 30 hertz
		std::this_thread::sleep_for(std::chrono::milliseconds(33));
	}


	return 0;
}