#include "virus.hpp"

Virus::Virus() {}
Virus::~Virus() {}
Virus::Virus(int id, std::string name, double strength, double mutation) {
	this->id = id;
	this->name = name;
	this->strength = strength;
	this->mutation_prob = mutation;
	this->dormant = 0;
	this->evolved = 0;
	this->current_infected = 0;
	
}

/*
 * if start is set to true, then it means this method is being called to initiate a virus. else
 * if a person is infecting another person, this function uses various probability functions to 
 * check whether the latter is successfully infected.
 * returns true or false depending on whether infection succeeded
 */
bool Virus::infect(Zone zone, Person infector, Person infecting, bool start) {
	// only infect the person if he is not vaccinated against the virus
	if (infecting.vaccined[name].first) {
		if (fmod(id,infecting.vaccined[name].second) != 0) {
			// infecting is vaccined against this mutation of virus 
			return false;
		}
	}

	if (start) {
		this->infected_people.push_back(infecting);
		this->current_infected ++;
		// set person to immune with 1% probability
		int is_immune = rand()%100 + 1;
		if (is_immune == 1) {
			infecting.immune[this->name] = true;
			this->immune_people.push_back(infecting);
		}
		return true;
	}
	else {
		
		// for all viruses, a higher zone population density makes prob of infection higher
		// population density in real world can be anywhere from 1 - 50000 (people per km^2)
		// take this density over 100000
		double prob = zone.calculate_population_density()/100000.0;
		// also if infector is a carrier (naturally immune)  probability is higher to infect
		if (infector.immune["name"]) {
			prob *= 1.5;
		}
		if (this->name.compare("HIV") == 0) {
			// assume that children (12 and under) are much less likely to be infected
			// changes of infection are higher if the age differential between the two people is lower
			// also, the level of healthcare in the zone makes a difference
			if (infecting.age > 12) {
				prob *= (abs(infector.age - infecting.age)*0.25)/100.0; 
				prob *= (1.0 - zone.get_location().healthcare);
			}
			else {
				prob = 0.05;
			}
		}
		else if (this->name.compare("Flu") == 0) {
			// has a higher probability of infecting young children and the elderly (above 60)
			// younger people who are infected also have a higher change of infecting others
			// the level of pollution in the zone makes a difference
			double chance = 0.25;
			if (infecting.age > 60|| infecting.age < 15) {
				chance = 0.5;
			}
			prob *= (100 - infector.age)*chance/100.0;
			prob *= zone.get_location().pollution;
		}
		else if (this->name.compare("Ebola") == 0) {
			// everyone gets equal chance of getting infected with ebola
			// chances of infection depends on zone
			// the level of pollution in the zone makes a difference
			prob *= 0.9;
			prob *= zone.get_location().pollution;
		}


		// use prob to decide whether or not to infect person
		if ((rand() % 100 + 1.0)/100.0 < prob) {
			this->infected_people.push_back(infecting);
			this->current_infected ++;
			// set person to immune with 1% probability
			int is_immune = rand() % 100 + 1;
			if (is_immune == 1) {
				infecting.immune[this->name] = true;
				this->immune_people.push_back(infecting);
			}
			return true;
		}

		return false;
	}
}

/*
 * modifies the attribute of this virus 
 */ 
bool Virus::evolve_virus(Zone zone){
	// we first check if we want to evolve the virus or not
	// uses randomness, the total number of infected people, level of pollution and healthcare 
	double prob; 
	prob = ((rand() % 100 + 1.0)/100.0) * this->current_infected/zone.get_initial_population() 
		* zone.get_location().pollution * (1.0 - zone.get_location().healthcare);

	if ((rand() % 100 + 1.0)/100.0 > prob) {
		return false;
	}
	
	else { // evolve virus and return true
		// firstly, we want to make this virus immune to past cures and vaccines
		int i = zone.has_virus(this->name);
		// use i to access previous vaccine and cure for this virus, modify them
		zone.get_vaccines()[i].set_id(1); 
		zone.get_cures()[i].set_id(1);
		this->evolved++;
		return true;
	}	
}

std::string Virus::get_name() {
	return this->name;
}

int Virus::get_id() {
	return this->id;
}

int Virus::get_dormant() {
	return this->dormant;
}

int Virus::get_evolved() {
	return this->evolved;
}

double Virus::get_strength() {
	return this->strength;
}

double Virus::get_mutation_prob() {
	return this->mutation_prob;
}

int Virus::get_current_infected_num() {
	return this->current_infected;
}

std::vector<Person> Virus::get_infected_people() {
	return this->infected_people;
}

std::vector<Person> Virus::get_immune_people() {
	return this->immune_people;
}

void Virus:: set_name(std::string name) {
	this->name = name;
}

void Virus::set_dormant(int value) {
	this->dormant = value;
}

void Virus::set_evolved(int value) {
	this->evolved = value;
}

void Virus::set_current_infected_num(int value) {
	this->current_infected = value;
}

void Virus::set_mutation_prob(int value) {
	this->mutation_prob = value;
}

std::map <int, int> Virus::get_infected_people_map() {
	return this->infected_people_map;
}

void Virus::add_infected_person(int i, int id) {
	this->infected_people_map[i] = id;
}

