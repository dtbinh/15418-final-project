#ifndef PERSON_H
#define PERSON_H

#include <string>
#include <map>
#include <utility>  
struct Person {
	bool dead;	// checks to see if dead. Better than turning into a pointer and checking for null;
	double net_worth; // how much this person has in monetary terms
	std::map <std::string, bool> immune; // set true or false depending on whether person naturally immune to a virus in them
	std::map <std::string, std::pair<bool, int> > vaccined;// set true or false depending on whether person vaccinated to a virus in them, map to strength of vaccine
	std::map <std::string, std::pair<bool, int> > infected; // set true or false depending on whether person is infected with a virus, map to virus id
	int age; // age of the person, between 1-100
	int time_left; // amount of time left in terms of the simulated system counter (days) (maximum age is 101)
	double position_x; // their x position on the grid
	double position_y; // their y position on the grid
};

#endif /* PERSON_H */