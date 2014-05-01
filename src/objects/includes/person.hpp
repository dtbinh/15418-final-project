#include <string>
#include <map>

struct Person {
	bool dead;	// checks to see if dead. Better than turning into a pointer and checking for null;
	double net_worth; // how much this person has in monetary terms
	std::map <std::string, bool> immune; // set true or false depending on whether person immune to a virus in them
	int age; // age of the person, between 1-100
	bool infected;
	int time_left;
};
