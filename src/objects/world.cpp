#include "world.hpp"


World::World() {
	setup_virus();
	setup_zones();
	steps = 0;
}

/*
 * construct the 6 different zones with all their attributes and people here
 */
void World::setup_zones(){
	// set up 6 different locations
	// singapore, new york city, tokyo, mumbai, zurich and cairo
/*	Zone::Location singapore_loc, nyc_loc, tokyo_loc, mumbai_loc, zurich_loc, cairo_loc;
	singapore_loc.pollution = 0.55;
	singapore_loc.government = 0.83;
	singapore_loc.healthcare = 0.88;
	tokyo_loc.pollution = 0.45;
	tokyo_loc.government = 0.61;
	tokyo_loc.healthcare = 0.85;
	nyc_loc.pollution = 0.67;
	nyc_loc.government = 0.68;
	nyc_loc.healthcare = 0.75;
	mumbai_loc.pollution = 0.75;
	mumbai_loc.government = 0.35;
	mumbai_loc.healthcare = 0.52;
	zurich_loc.pollution = 0.25;
	zurich_loc.government = 0.75;
	zurich_loc.healthcare = 0.95;
	cairo_loc.pollution = 0.81;
	cairo_loc.government = 0.27;
	cairo_loc.healthcare = 0.48;
	zones = new Zone[6];
	// intialize zones
	Zone singapore = Zone(singapore_loc, "Singapore", 5400000, 716.1,true);
	Zone nyc = Zone(nyc_loc, "New York City", 8405837, 1213.4,true);
	Zone tokyo = Zone(tokyo_loc, "Tokyo", 13185502, 2187.66,true);
	Zone mumbai = Zone(mumbai_loc, "Mumbai", 12478447, 603,false);
	Zone zurich = Zone(zurich_loc, "Zurich", 383708, 87.88,false);
	Zone cairo = Zone(cairo_loc, "Cairo", 10230520, 528,false);
	zones[0] = singapore;
	zones[1] = nyc;
	zones[2] = tokyo;
	zones[3] = mumbai;
	zones[4] = zurich;
	zones[5] = cairo; 
*/
}

/*
 * contruct the 3 virus classes with their attributes here
 */
void World::setup_virus() {

}

void introduce_virus(Virus* v) {

}


/*
 * this function makes 1 time step in the world. Each step, new events happen. We can have a vaccine/ cure made if certain conditions are met, 
 * we can introduce new viruses to a zone, we propogate all active viruses, propogate vaccine and cures, evolve a virus or make it dormant, sell viruses and cures
 * from one zone to another zone, update the time that each person has left (each person has a natural time left calculated from their age and the value decreases
 * every step, but being infected drops this value faster). Once their time left reaches 0, the person dies and is remvoed from the zone. 
 */
void World::step() {
	if (steps == 0) {
		introduce_virus(NULL);
	}
	else if (steps == 100) {
		introduce_virus(NULL);
	}
	else if (steps == 200) {
		introduce_virus(NULL);
	}

	// TO DO, implement the various helper functions both here and the other classes
}
/* 
 * returns the zone with name we are looking for, or a NULL if that zone does not exist
 */
Zone* World::get_zone(std::string name) {
	for (int i = 0; i < 5; i++) {
		if (zones[i].getName().compare(name) == 0) {
			return &zones[i];
		}
	}

	return NULL;
}

/* 
 * returns the virus with name we are looking for, or a NULL if that zone does not exist
 */
Virus* World::get_virus(std::string name) {
	if (name.compare("hiv") == 0) {
		return &hiv;
	}
	else if (name.compare("flu") == 0) {
		return &flu;
	}
	else if (name.compare("hepatitis") == 0) {
		return &hepatitis;
	}
	else {
		return NULL;
	}
}

int World::get_current_step() {
	return steps;
}
