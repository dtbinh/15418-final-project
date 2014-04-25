

// add in attributes for person and location struct. These attributes determine the virus and vaccine transmission rates, amongst other stuff
struct Person{
	double net_worth; // how much this person has in monetary terms
	std::map <string, bool> immune; // set true or false depending on whether person immune to a virus in them
	int age; // age of the person, between 1-100
	bool infected;
	int time_left;
};

struct Location{
	float pollution; // percentile value, higher means more percentile
	float government; // percentile value, lower means more corrupt, slow, useless government
	float healthcare; // percentile value, level of healthcare system (insurance, doctors, medical technology etc)
};


Zone::Zone(Location loc, string name, int population, double area, bool wealthy) {
	name = name;
	loc = loc;
	population = population;
	area = area;
	viruses = new Virus[3];
	// add in a newly create bunch of person structs
	people = new Person [population];

	int use = 0;
	float wealth = 0.0; 
	// give each person random attributes 
	for (int i = 0; i < population; i++) {
		people[i].immune = new std::map<string,bool>;
		// set person's age randomly between 1-100
		people[i].age = rand() % 100 + 1;
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
		if (people[i] != nullptr) {	
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
	people[i] = nullptr;
	population = population-1;
}

/*
 * returns the person at position i in people
 */ 
Person Zone::get_person(int i) {
	return people[i];
}
 