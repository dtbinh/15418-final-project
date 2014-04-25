#include "vaccine.hpp"
#include <iostream>
#include <string>
#include <stdlib.h>

class cure {

public:
	cure(int dna, string name, string virus, float strength, double cost);
	//~cure();

	void cure(Person person);
private:
	int dna;
	string name;
	string cured_virus;
	float strength;

	double cost;

	int* cured; // list of all array positions of people cured
}
