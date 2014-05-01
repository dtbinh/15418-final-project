#include <iostream>
#include <string>
#include <stdlib.h>
#include "person.hpp"

class Virus {

public:
	Virus(int dna, std::string name, float strength);
	Virus();
	~Virus();

	void infect(Person person);
	void evolve();
private:
	int dna;
	std::string name;

	float strength;
	bool dormant;
	bool evolved;

	Person* infected; // list of array positions of all infected people not immune to virus
	Person* immune; // list of array positions of all people who are infected but immune to virus
};
