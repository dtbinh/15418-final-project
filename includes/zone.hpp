#include <iostream>
#include <string>
#include <stdlib.h>
#include <cmath>

#define MAX_POPULATION 10000
struct Person;
struct Location;

class Zone {

public: 
	Zone(Location loc, string name, int population, double area);
	//~Zone();

	double calculate_wealth();
	double calculate_population_density();
	void set_population(int size);
	int get_population();
	void person_dies(int i);
	Person get_person(int i);
 

private:
	string name;
	Location loc;
	int	population; 
	double area; // in km square

	Virus* viruses;
	Person* people;
}
