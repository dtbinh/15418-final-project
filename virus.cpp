#include "includes/virus.hpp"

Virus::Virus(int dna, string name, float strength) {
	dna = dna;
	name = name;
	strength = strength;
	dormant = false;
	evolved = false;
	// infected can be everyone
	infected = new Person [MAX_POPULATION];
	// we'll go by the law that for every virus, 1% of the population in the world is immune to it
	immune = new Person [MAX_POPULATION/50];
}

void Virus::infect(Person person) {
	person.infected = true;
	// set person to immune with 1% probability
	int is_immune = rand() % 100 + 1;
	if (is_immunue == 1) {
		person.immune[name] = true;
	}
}



