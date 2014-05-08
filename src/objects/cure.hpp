#ifndef CURE_H
#define CURE_H

#include <iostream>
#include <string>
#include <stdlib.h>
#include "person.hpp"
class Cure {

public:
	Cure(int id, std::string name, std::string virus, int strength);
	Cure();
	~Cure();

	void cure(Person person);
	std::string get_name();
	std::string get_cured_virus_name();
	int get_strength();
	int get_id();
	void set_id(int value);
	void set_strength(int value);

private:
	int id; // a higher id value is better
	std::string name;
	std::string cured_virus;
	int strength; 

	int* cured; // list of all array positions of people cured
};

#endif /* CURE_H*/