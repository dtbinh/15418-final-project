#include <iostream>
#include <string>
#include <stdlib.h>
#include <cmath>
#include <map>
#include <vector>
#include "virus.hpp"

#define MAX_POPULATION 10000

class Zone {

// add in attributes for person and location struct. These attributes determine the virus and vaccine transmission rates, amongst other stuff
struct Location{
	float pollution; // percentile value, higher means more percentile
	float government; // percentile value, lower means more corrupt, slow, useless government
	float healthcare; // percentile value, level of healthcare system (insurance, doctors, medical technology etc)
};

public: 
	Zone(Location loc, std::string name, int population, double area, bool wealthy);
	//~Zone();

	double calculate_wealth();
	double calculate_population_density();
	void set_population(int size);
	int get_population();
	void person_dies(int i);
	Person get_person(int i);
 

private:
	std::string name;
	Location loc;
	int	population; 
	double area; // in km square

	std::vector<Virus> viruses;
	Person* people;
};
