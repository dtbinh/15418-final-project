#ifndef VACCINE_H
#define VACCINE_H

#include <iostream>
#include <string>
#include <stdlib.h>
#include "person.hpp"
class Vaccine {

public:
	Vaccine(int id, std::string name, std::string virus, int strength);
	Vaccine();
	~Vaccine();

	void vaccinate(Person person);
	std::string get_name();
	std::string get_vaccinated_virus_name();
	int get_strength();
	int get_id();
	void set_id(int value);
	void set_strength(int value);
private:
	int id; // a higher id value is better
	std::string name;
	std::string vaccinated_virus;
	int strength; 
};

#endif /* VACCINE_H*/