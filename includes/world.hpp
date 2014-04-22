#include "zone.hpp"
#include "virus.hpp"

class World {

public:
	World();
	~World();

	void setup_zones();
	void setup_virus();
	
	void step();

	Zone*  getZone(char* name);
	Virus* getVirus(char* name);
	int    getCurrentStep();
	
private:
	Virus* hiv;
	Virus* flu;
	Virus* hepatitis;

	Zone* zones;

	int step;

}
