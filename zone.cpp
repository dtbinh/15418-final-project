#include "includes/zone.hpp"

Zone::Zone(Location loc, std::string name, int population, double area, bool wealthy) {
	name = name;
	loc = loc;
	population = population;
	area = area;
	/* TODO reinitialize the virus stats */
	viruses.push_back(Virus(1, "HIV", 1.0));
	viruses.push_back(Virus(2, "Influenza", 2.0));
	viruses.push_back(Virus(3, "Hepatatis", 3.0));

	// add in a newly create bunch of person structs
	people = new Person [population];

	int use = 0;
	float wealth = 0.0; 
	// give each person random attributes 
	for (int i = 0; i < population; i++) {
		// set person's age randomly between 1-100
		people[i].age = rand() * 100 + 1;
		people[i].infected = false;
		people[i].time_left = (100 - people[i].age)*100;
		// now find a random percentile value for this person's wealth
		use = rand() % 4 + 1;
		// if this is meant to be a wealthy zone, then more people are in the upper 50% of world's wealth with a 3/4 chance
		if (wealthy) {
			if (use < 4) {
				// return a fraction between 0.5 and 1.0
				wealth = (rand() % 50 + 1.0) / 100.0 + 0.5;
			}
			else {
				// return a fraction between 0.0 and 0.5
				wealth = (rand() % 50 + 1.0) / 100.0;
			}
		}
		// else people are in the lower 50% of world's wealth with a 3/4 chance
		else {
			if (use < 4) {
				// return a fraction between 0.0 and 0.5
				wealth = (rand() % 50 + 1.0) / 100.0;
			}
			else {
				// return a fraction between 0.5 and 1.0
				wealth = (rand() % 50 + 1.0) / 100.0 + 0.5;
			}
		}

		// now we use the person's percentile wealth value to determine their net worth. 
		// the higher the percentile, their net worth becomes exponentially greater
		people[i].net_worth = exp(2.0*wealth);
	}
}

/*
 * returns the sum of the net_worth of everyone in the zone
 */
double Zone::calculate_wealth() {
	double sum = 0.0;
	for (int i = 0; i < population; i++) {
		// if person is not dead
		if (people[i].dead != true) {	
			sum += people[i].net_worth;
		}
	}

	return sum;
}

/*
 * returns the population density for this zone
 */
double Zone::calculate_population_density() {
	return population/area;
}

/*
 * sets the population to a certain number
 */
void Zone::set_population(int size) {
	population = size;
}

/*
 * gets the current population of the zone
 */
int Zone::get_population() {
	return population;
}

/*
 * function to call when a person dies
 */
void Zone::person_dies(int i) {
	people[i].dead = true;
	population = population-1;
}

/*
 * returns the person at position i in people
 */ 
Person Zone::get_person(int i) {
	return people[i];
}
 
