#include "zone.hpp"

Zone::Zone() {}
Zone::~Zone() {}
Zone::Zone(Location location, std::string name, int population, double area, bool wealthy) {
	this->name = name;
	this->loc = location;
	this->initial_population = population;
	this->population = population;
	this->area = area;
	this->num_viruses = 0;
	// add in a newly create bunch of person structs
	this->people = new Person [population];

	int use = 0;
	float wealth = 0.0; 
	// give each person random attributes 
	for (int i = 0; i < population; i++) {
		// set person's age randomly between 1-100
		this->people[i].age = rand() * 100 + 1;
		// set virus infected and immune to false
		this->people[i].infected["HIV"] = std::make_pair(false,-1);
		this->people[i].infected["Flu"] = std::make_pair(false,-1);
		this->people[i].infected["Ebola"] = std::make_pair(false,-1);

		this->people[i].immune["HIV"] = false;
		this->people[i].immune["Flu"] = false;
		this->people[i].immune["Ebola"] = false;
		// number of days left to live their natural life
		this->people[i].time_left = (101 - people[i].age)*365;

		// also set person's x and y coordinates here
		// TO DO get right positions
		this->people[i].position_x = 0.0;
		this->people[i].position_y = 0.0;

		// now find a random percentile value for this person's wealth
		use = fmod(double(rand()),4) + 1;
		// if this is meant to be a wealthy zone, then more people are in the upper 50% of world's wealth with a 3/4 chance
		if (wealthy) {
			if (use < 4) {
				// return a fraction between 0.5 and 1.0
				wealth = (fmod(double(rand()),50) + 1.0) / 100.0 + 0.5;
			}
			else {
				// return a fraction between 0.0 and 0.5
				wealth = (fmod(double(rand()),50)+ 1.0) / 100.0;
			}
		}
		// else people are in the lower 50% of world's wealth with a 3/4 chance
		else {
			if (use < 4) {
				// return a fraction between 0.0 and 0.5
				wealth = (fmod(double(rand()),50) + 1.0) / 100.0;
			}
			else {
				// return a fraction between 0.5 and 1.0
				wealth = (fmod(double(rand()),50) + 1.0) / 100.0 + 0.5;
			}
		}

		// now we use the person's percentile wealth value to determine their net worth. 
		// the higher the percentile, their net worth becomes exponentially greater
		this->people[i].net_worth = exp(2.0*wealth);
	}
}

/*
 * returns the sum of the net_worth of everyone in the zone
 */
double Zone::calculate_wealth() {
	double sum = 0.0;
	for (int i = 0; i < this->initial_population; i++) {
		// if person is not dead
		if (this->people[i].dead != true) {	
			sum += this->people[i].net_worth;
		}
	}

	return sum;
}

/*
 * returns the population density for this zone
 */
double Zone::calculate_population_density() {
	return this->population/this->area;
}


/*
 * returns the name of the city this zone represents
 */
std::string Zone::get_name() {
	return this->name;
}


/*
 * sets the population to a certain number
 */
void Zone::set_population(int size) {
	this->population = size;
}

/*
 * gets the initial population of the zone
 */
int Zone::get_initial_population() {
	return this->initial_population;
}


/*
 * gets the current population of the zone
 */
int Zone::get_population() {
	return this->population;
}


/*
 * gets the location of  this zone
 */
Zone::Location Zone::get_location() {
	return this->loc;
}


/*
 * returns the total number of dead people in the zone since start of simulation
 */
int Zone::get_total_dead() {
	return this->initial_population - this->population;
}

/*
 * returns the person at position i in people
 */ 
Person Zone::get_person(int i) {
	return this->people[i];
}

/*
 * function to call when a person dies
 */
void Zone::person_dies(int i) {
	this->people[i].dead = true;
	this->population = this->population-1;
}

/*
 * gets the number of viruses in the zone
 */
int Zone::get_num_viruses() {
	return this->num_viruses;
}

/*
 * returns the vector of all viruses infecting this zone
 */
std::vector<Virus> Zone::get_viruses() {
	return this->viruses;
}

/*
 * returns the map mapping viruses to the number of people they have killed
 */
std::map<std::string, int> Zone::get_virus_killed(){
	return this->virus_killed;
}

/*
 * returns the index of the virus in vector 'viruses' if it is vector, else 
 * returns -1
 */
int Zone::has_virus(std::string name) {
	std::string current;
	for (int i = 0; i < this->num_viruses; i++) {
		current = this->viruses[i].get_name();
		if (current.compare(name) == 0) {
			return i;
		}
	}

	return -1;
}

/*
 * returns the friendship level of this zone with another zone
 */
int Zone::get_friendship(std::string name){
	return this->friendship[name];
}

/*
 * changes the friendship level  of this zone with another zone to value
 */
void Zone::change_friendship(std::string name, int value){
	this->friendship[name] = value;
}


/*
 * returns the vector of vaccines for viruses in this zone
 */ 
std::vector<Vaccine> Zone:: get_vaccines() {
	return this->vaccines;
}

/*
 * returns the vector of cures for viruses in this zone
 */ 
std::vector<Cure> Zone::get_cures() {
	return this->cures;
}

/*
 * This is a method to introduce a new virus into a zone
 * Starts by infecting INITIAL_INFECTED number of people in each zone
 */
void Zone::introduce_virus(Virus virus) {
	this->virus_active_time[virus.get_name()] = 0;
	this->virus_killed[virus.get_name()] = 0;
	this->viruses.push_back(virus);
	virus.set_dormant(0);

	this->num_viruses += 1;
	int pop = this->initial_population;
	Person current; 
	int infected = 0;
	int i = 0;

	int initial = INITIAL_INFECTED;
	// infect random INITIAL_INFECTED number of people
	while (infected < initial) {
		i = fmod(double(rand()),double(pop));
		current = get_person(i);
		if (!current.dead) {
			// infect person only if not dead yet
			if (virus.infect(*this,current,current, true)) {
				infected += 1;
				// add new infected person to map with original virus id
				virus.add_infected_person(i, virus.get_id());
				current.infected[virus.get_name()] = std::make_pair(true,virus.get_id());
			}
		}
	}
}

/*
 * returns an array of all people within +- strength units of infected person's x,y coordinates
 */
std::list<int> Zone::get_nearest_people(Person infected, double strength) {
	// go through all people in the zone and add them to a list of infected people
	std::list<int> nearest;
	double x = infected.position_x;
	double y = infected.position_y;
	Person current;
	for (int i = 0; i < this->initial_population; i++) {
		current = this->people[i];
		// if current person is near enough and not dead, add him
		if (current.dead) {
			continue;
		}
		if (abs(current.position_x-x) <= strength || abs(current.position_y-y) <= strength) {
			nearest.push_front(i);
		}
	}
	return nearest;
}
/*
 * this is a method to propogate a virus in a zone. It goes through 
 * all infected people in the zone and tries to infect the people nearest to them
 * propogates all viruses in the system
 */
void Zone::propogate_virus() {
	Virus current;
	Person infector;
	Person infecting; 
	std::list<int> nearest;

	for (int i = 0; i < this->num_viruses; i++) {
		current = this->viruses[i];
		if (current.get_dormant() == 0) {
			this->virus_active_time[current.get_name()]++;
		}
		// go through each alive infected person of the current virus
		for (int j = 0; j < current.get_current_infected_num(); j++) {
			infector = current.get_infected_people()[j];
			if (infector.dead) {
				continue;
			}
			nearest = get_nearest_people(infector, current.get_strength());
			// go through each of the nearest people and infect them
			std::list<int>::iterator it;
			for (it = nearest.begin(); it != nearest.end(); it++) {
				infecting = people[*it];
				if (infecting.dead) {
					continue;
				}
				if (current.infect(*this, infector, infecting,false)) {
					if ((fmod(double(rand()),50)/100.0) <= current.get_mutation_prob()) {
						// add new infected person to map of infected person with new mutated id
						int id = rand();
						current.add_infected_person(*it, id);
						infecting.infected[current.get_name()] = std::make_pair(true,id);
					}
					else {
						// add new infected person to map with old id 
						current.add_infected_person(*it, current.get_id());
						infecting.infected[current.get_name()] = std::make_pair(true,current.get_id());
					}
				}
			}
		}
	}
}



/*
 * updates the time left of healthy people by decreasing it by 1 (1 day)
 * for infected people who are not immune to the virus, they get their time left 
 * decremented by a multiplier. For infected people who are immune to virus, 
 * same as healthly people
 */ 
void Zone::update_time_left() {
	Person current;
	bool is_healthy = true;
	for (int i = 0; i < this->population; i++) {
		current = this->people[i];
		if (!current.dead) {
			continue;
		}
		// if virus is dormant, then people time left isn't affected.
		if (current.infected["Flu"].first && !current.immune["Flu"]) {
			// loses 5 days of their life for each day infected with the flu
			if (this->viruses[0].get_dormant() == 0) {
				current.time_left -= 5;
				is_healthy = false;
				if (current.time_left <= 0) {
					this->virus_killed["Flu"]++;
				}
			}
		}

		if (current.infected["HIV"].first && !current.immune["HIV"]) {
			// loses 10 days of their life for each day infected with the hiv
			if (this->viruses[1].get_dormant() == 0) {
				current.time_left -= 10;
				is_healthy = false;
				if (current.time_left <= 0) {
					this->virus_killed["HIV"]++;
				}
			}
		}

		if (current.infected["Ebola"].first && !current.immune["Ebola"]) {	
			// loses 150 days of their life for each day infected with the flu
			if (this->viruses[2].get_dormant() == 0) {
				current.time_left -= 150;
				is_healthy = false;
				if (current.time_left <= 0) {
					this->virus_killed["Ebola"]++;
				}
			}
		}

		if (is_healthy) {
			current.time_left -= 1;
		}

		// now call person dead function
		if (current.time_left <= 0) {
			person_dies(i);
		}
	}
}



/*
 * whenever a virus has evolved, we call this to update the virus's name and 
 * introduce it into the zone again
 */ 
void Zone::update_virus(Virus virus) {
	int evolved = virus.get_evolved();
    std::stringstream ss;
    ss << evolved;
    std::string str = ss.str();
	virus.set_name(virus.get_name() + this->name + str);

	// add it active time map and killed map but don't push it to list of viruses infecting the zone
	// this is because it is just an evolved virus, 
	this->virus_active_time[virus.get_name()] = 0;
	this->virus_killed[virus.get_name()] = 0;
	virus.set_dormant(0);

	int pop = this->initial_population;
	Person current; 
	int infected = 0;
	int i = 0;

	int initial = INITIAL_INFECTED;
	// infect random INITIAL_INFECTED number of people
	while (infected < initial) {
		i = fmod(double(rand()),double(pop));
		current = get_person(i);
		if (!current.dead) {
			// infect person only if not dead yet
			if (virus.infect(*this,current,current, true)) {
				infected += 1;
				// add new infected person to map with original virus id
				virus.add_infected_person(i, virus.get_id());
				current.infected[virus.get_name()] = std::make_pair(true,virus.get_id());
			}
		}
	}
}


/*
 * checks if enough time has elapsed to make a vaccine and cure or if so many people have died such that
 * the time taken to need the vaccine has been sped up
 * if virus is a dormant virus, then return false too
 */
bool Zone::found_vaccine_and_cure(Virus virus) {
	if (virus.get_dormant() > 0) {
		return false;
	}
	int interval = VIRUS_INTERVAL;
	if (this->virus_active_time[virus.get_name()] >= interval/2) {
		return true;
	}

	if (this->virus_killed[virus.get_name()] > initial_population/50.0) {
		return true;
	}

	return false;
}

/*
 * This is a method to introduce a vaccine into the zone
 */
void Zone::introduce_vaccine(Vaccine vac) {
	// if virus that this vaccine vaccinates is an evolved virus, we replace the vaccine in vector rather than add new one
	int i = has_virus(vac.get_vaccinated_virus_name());
	if (viruses[i].get_evolved() > 0) {
		vaccines[i] = vac;
	}
	else {
		this->vaccines.push_back(vac);
	}
}

/*
 * This is a method to introduce a cure into the zone
 */
void Zone::introduce_cure(Cure cure) {
	// if virus that this cure cures is an evolved virus, we replace the cure in vector rather than add new one
	int i = has_virus(cure.get_cured_virus_name());
	if (viruses[i].get_evolved() > 0) {
		cures[i] = cure;
	}
	else {
		this->cures.push_back(cure);
	}
}


/*
 * this is a method to propogate a vaccine into a zone. It vaccines everyone
 * in the zone 
 */
void Zone::propogate_vaccine(Vaccine vaccine) {
	// go through everyone in the zone
	for (int i = 0; i < this->population; i++) {
		vaccine.vaccinate(people[i]);
	}
}


/*
 * this is a method to propogate a cure into a zone. It cures as many people as it can
 * It then returns the total number of cures left over
 */
int Zone::propogate_cure(int total_cures, Cure cure) {
	int current_cures = total_cures;

	// go through each infected person of this virus
	Virus virus = this->viruses[has_virus(cure.get_cured_virus_name())];
	std::vector<Person> infected = virus.get_infected_people();

	// cure the people who contracted the virus first
	for (int i = 0; i < infected.size(); i++) {
		cure.cure(infected[i]);
		current_cures--;
		if (current_cures == 0) {
			break;
		}
	}

	return current_cures;
}
