#include "includes/cycleTimer.h"
#include "includes/world.hpp"
#include <unistd.h>
#include <stdio.h>


void step(World* world);
void print_usage();

int main(int argc, char* argv)
{
	World* awholenewworld = new World();
	bool loop = true;

	/* loop to read input */
	while (loop) 
	{
		char key;
		std::cin >> key;
		switch (key) {
			case 's':
				step(awholenewworld);
				break;
			case 'x':
				loop = false;
				break;
			default:
				print_usage();
				break;
		}
	}

	return 0;
}

void step(World* world) 
{
	float startTime;
	float endTime;
	startTime = CycleTimer::currentSeconds();

	/* update all the info of the world */
	world->step();

	endTime = CycleTimer::currentSeconds();
	std::cout << "Time in seconds " << endTime - startTime << " ms\n";
}

void print_usage()
{
	std::cout << "Options: press 's' to advance a step.\n	press 'x' to exit simulation\n";
}
