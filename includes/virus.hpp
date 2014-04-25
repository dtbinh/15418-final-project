#include "zone.hpp"
#include <iostream>
#include <string>
#include <stdlib.h>

class Virus {

public:
	Virus(int dna, string name, float strength);
	//~Virus();

	void infect(Person person);
	void evolve();
private:
	int dna;
	string name;

	float strength;
	bool dormant;
	bool evolved;

	int* infected; // list of array positions of all infected people not immune to virus
	int* immune; // list of array positions of all people who are infected but immune to virus
}
