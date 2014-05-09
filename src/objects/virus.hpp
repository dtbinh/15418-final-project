#ifndef VIRUS_H
#define VIRUS_H

#include <iostream>
#include <string>
#include <stdlib.h>
#include <cmath>
#include <map>
#include <vector>

#include "person.hpp"
#include "zone.hpp"



class Zone; // forward declaration


class Virus {

public:
	Virus(int id, std::string name, double strength, double mutation);
	Virus();
	~Virus();

	bool infect(Zone zone, Person infector, Person infecting, bool start);
	bool evolve_virus(Zone zone);

	// functions to access virus attributes
	std::string get_name();
	int get_id();
	int get_dormant();
	int get_evolved();
	double get_strength();
	double get_mutation_prob();
	int get_current_infected_num();
	std::vector<Person> get_infected_people();
	std::vector<Person> get_immune_people();

	void set_name(std::string name);
	void set_dormant(int value);
	void set_evolved(int value);
	void set_current_infected_num(int value);
	void set_mutation_prob(int value);

	std::map <int, int> get_infected_people_map();
	void add_infected_person(int i, int id);
private:
	int id;
	std::string name;
	double mutation_prob;

	// strength of a virus is as such:
	// an infected person can possibly infect all people within +- strength units of the infected person's x,y coordinates
	double strength;
	int dormant; // the number of steps for which a virus is dormant 
	int evolved; // the number of times that a virus has evolved

	// list of vector of all infected people not immune to virus 
	// infected can be everyone
	std::vector<Person> infected_people; 
	int current_infected;

	// a map mapping the position of a person in the people array of the zone to the id of the virus infecting them
	std::map<int, int> infected_people_map;

	// list of vector of all people who are infected but naturally immune to virus 
	// we'll go by the law that for every virus, 1% of the population in the world is immune to it
	std::vector<Person> immune_people;

};

#endif /* VIRUS_H */