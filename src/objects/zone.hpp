#ifndef ZONE_H
#define ZONE_H

#include "virus.hpp"
#include "cure.hpp"
#include "vaccine.hpp"
#include <list>

#include <iostream>
#include <string>
#include <sstream>
#include "cycleTimer.h"
#include <stdio.h>

class Virus; // forward declaration

#define MAX_POPULATION 15000000;

#define INITIAL_INFECTED 100;

#define VIRUS_INTERVAL 500;


class Zone {


public: 
	// add in attributes for location struct. These attributes determine the virus and vaccine transmission rates, 
	// amongst other stuff
	struct Location{
		float pollution; // percentile value, higher means more percentile
		float government; // percentile value, lower means more corrupt, slow, useless government
		float healthcare; // percentile value, level of healthcare system (insurance, doctors, medical technology etc)
	} loc;

	Zone(Location loc, std::string name, int population, double area, float x, float y, bool wealthy);
	Zone();
	~Zone();

	// methods to obtain advanced attributes of zone
	double calculate_wealth();
	double calculate_population_density();

	// several methods to access attributes of zone
	std::string get_name(); 
	void set_population(int size);
	int get_initial_population();
	int get_population();
	double get_area();
	Location get_location();

	float x, y; //cartesian location coordinates

	// functions to access a person and modify their trait
	int get_total_dead();
	Person get_person(int i); 
	void person_dies(int i); // call whenever a person dies
	
	// functions to access and modify viruses attributes 
	int get_num_viruses();
	std::vector<Virus> get_viruses();
	std::map<std::string, int> get_virus_killed();
	int has_virus(std::string virus);

	int get_friendship(std::string name);
	void change_friendship(std::string name, int value);

	std::vector<Vaccine> get_vaccines();
	std::vector<Cure> get_cures();
	// functions for the step function in world
	// main functionality
	void introduce_virus(Virus virus); // introduces a virus into the zone
	std::list<int> get_nearest_people(Person infected, double strength); // gets people near to infected person
	void propogate_virus(); // propogates all viruses currently in this zone

	void update_time_left(); // updates the time left for everyone in this zone
	void update_virus(Virus virus); // updates a virus with its evolved version

	bool found_vaccine_and_cure(Virus virus); // returns true if virus needs a vaccine and cure
	
	void introduce_cure(Cure cure); // introduces a cure into the zone
	int propogate_cure(int total_cures, Cure cure); // propogates a cure to as many infected ppl as possible in znoe
	
	void introduce_vaccine(Vaccine vaccine); // introduces a vaccine into zone
	void propogate_vaccine(Vaccine vaccine); // propogates a vaccine to everyone in zone

private:
	std::string name;
	int initial_population;
	int	population; 
	double area; // in km square

	int num_viruses;
	// a map mapping each virus to the amount of time they have been infecting this zone
	std::map<std::string, int> virus_active_time;
	// a map mapping each virus to the number of people they have killed
	std::map<std::string, int> virus_killed;
	std::vector<Virus> viruses; // the viruses infecting this zone

	std::vector<Vaccine> vaccines; // the vaccines for each of the viruses
	std::vector<Cure> cures; // the cures for each of the vaccines

	Person* people; // the people living in this zone
	std::map <std::string, int> friendship; // how friendly a zone is with the other 5 zones (more trade, cooperation, communication etc)
};

#endif /* ZONE_H*/
