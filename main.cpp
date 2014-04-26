#include "includes/cycleTimer.h"
#include "includes/world.hpp"
#include <unistd.h>
#include <stdio.h>


void step();
void print_usage();

main(int argc, char* argv)
{
	World awholenewworld = new World();
	bool loop = true;

	/* loop to read input */
	while (loop) 
	{
		char key;
		std::cin >> key;
		switch (key) {
			case 's':
				step();
				break;
			case 'x':
				loop = false;
				break;
			case default:
				print_usage();
				break;
		}
	}

}

void step() 
{
	float startTime;
	float endTime;
	startTime = cycleTimer::currentSeconds();

	/* update all the info of the world */

	endTime = cycleTimer::currentSeconds();
	std::cout << "Time in seconds " << endTime - startTime << " ms\n";
}

void print_usage()
{
	std::cout << "Options: press 's' to advance a step.\n	press 'x' to exit simulation\n";
}
