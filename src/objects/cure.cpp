#include "cure.hpp"

Cure::Cure(int dna, std::string name, std::string virus, float strength, int cost) {
	dna = dna;
	name = name;
	strength = strength;
	cost = cost;
	cured_virus = virus;
}

/*
 * cures a person by making him not infected.
 */
void Cure::cure(Person person) {
	person.infected = false;
}



