#include "virus.hpp"
#include <iostream>
#include <string>
#include <stdlib.h>

class Vaccine {

public:
	Vaccine(int dna, string name, string virus, float strength, double cost);
	//~Vaccine();

	void vaccinate(Person person);
private:
	int dna;
	string name;
	string vaccinated_virus;
	float strength;

	double cost;
}
