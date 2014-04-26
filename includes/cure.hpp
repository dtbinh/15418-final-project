#include "vaccine.hpp"
#include <iostream>
#include <string>
#include <stdlib.h>

class Cure {

public:
	Cure(int dna, std::string name, std::string virus, float strength, double cost);
	//~cure();

	void cure(Person person);
private:
	int dna;
	std::string name;
	std::string cured_virus;
	float strength;

	double cost;

	int* cured; // list of all array positions of people cured
};
