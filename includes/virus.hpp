#include <iostream>
#include <string>
#include <stdlib.h>
#include "person.hpp"

class Virus {

public:
	Virus(int dna, std::string name, float strength);
	//~Virus();

	void infect(Person person);
	void evolve();
private:
	int dna;
	std::string name;

	float strength;
	bool dormant;
	bool evolved;

	int* infected; // list of array positions of all infected people not immune to virus
	int* immune; // list of array positions of all people who are infected but immune to virus
};
