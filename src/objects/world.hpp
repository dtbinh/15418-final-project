#include "zone.hpp"
#include <iostream>
#include <string>
#include <stdlib.h>



class World {

public:
	World();
	//~World();

	void setup_zones();
	void setup_virus();
	
	void step();

	Zone*  get_zone(std::string name);
	Virus* get_virus(std::string name);
	int    get_current_step();
	
private:
	Virus hiv;
	Virus flu;
	Virus hepatitis;

	Zone* zones;

	int steps;
};
