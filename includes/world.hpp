#include "zone.hpp"
#include "virus.hpp"
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

	Zone*  get_zone(string name);
	Virus* get_virus(string name);
	int    get_current_step();
	
private:
	Virus hiv;
	Virus flu;
	Virus hepatitis;

	Zone* zones;

	int step;

};
