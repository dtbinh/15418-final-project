#ifndef WORLD_H
#define WORLD_H


#include "zone.hpp"

#define NUM_ZONES 6;

class World {

public:
	World();
	//~World();

	void setup_zones();
	void setup_virus();
	void init_random_friendships();
	void distribute_vaccine_and_cure(Virus virus);
	void step();

	int get_zone_index(std::string name);
	int get_current_step();
	
private:

	Zone* zones;
	int steps;
};

#endif /* WORLD_H*/