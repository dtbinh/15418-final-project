#include "zone.hpp"

class Virus {

public:
	Virus();
	~Virus();

	void infect(Person* people);

private:
	int id;
	char* name;

	int amount;
	bool dormant;

	Person* infected;
}
