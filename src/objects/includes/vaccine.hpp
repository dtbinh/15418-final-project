#include "virus.hpp"
#include <iostream>
#include <string>
#include <stdlib.h>

class Vaccine {

public:
	Vaccine(int dna, std::string name, std::string virus, float strength, int cost);
	//~Vaccine();

	void vaccinate(Person person);
private:
	int dna;
	std::string name;
	std::string vaccinated_virus;
	float strength;

	double cost;
};
