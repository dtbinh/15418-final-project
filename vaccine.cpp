#include "includes/vaccine.hpp"

Vaccine::Vaccine(int dna, std::string name, std::string virus, float strength, int cost) {
	dna = dna;
	name = name;
	strength = strength;
	cost = cost;
	vaccinated_virus = virus;
}

/*
 * vaccinate a person by making him immune to the virus
 */
void Vaccine::vaccinate(Person person) {
	if (person.infected == false) { 
		person.immune[vaccinated_virus] = true;
	}
}



