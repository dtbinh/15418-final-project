#include "world.hpp"
#include <assert.h>

#define NUM_ZONES 6
#define S  0.01;

#define DEBUG
World::World() {
	setup_zones();
	this->steps = 0;
}
/*
 * construct the 6 different zones with all their attributes and people here
 */
void World::setup_zones(){
	// set up 6 different locations
	// singapore, new york city, tokyo, mumbai, zurich and cairo
	Zone::Location singapore_loc, nyc_loc, tokyo_loc, mumbai_loc, zurich_loc, cairo_loc;
	singapore_loc.pollution = 0.55;
	singapore_loc.government = 0.83;
	singapore_loc.healthcare = 0.88;
	tokyo_loc.pollution = 0.45;
	tokyo_loc.government = 0.61;
	tokyo_loc.healthcare = 0.85;
	nyc_loc.pollution = 0.67;
	nyc_loc.government = 0.68;
	nyc_loc.healthcare = 0.75;
	mumbai_loc.pollution = 0.75;
	mumbai_loc.government = 0.35;
	mumbai_loc.healthcare = 0.52;
	zurich_loc.pollution = 0.25;
	zurich_loc.government = 0.75;
	zurich_loc.healthcare = 0.95;
	cairo_loc.pollution = 0.81;
	cairo_loc.government = 0.27;
	cairo_loc.healthcare = 0.48;
	zones = new Zone[NUM_ZONES];
	// intialize zones
	Zone singapore = Zone(singapore_loc, "Singapore", 5400, 716.1, 1.15, 0.016, true);
	Zone nyc = Zone(nyc_loc, "New York City", 840, 1213.4, -0.82, 0.4503, true);
	Zone tokyo = Zone(tokyo_loc, "Tokyo", 1318, 2187.66, 1.55, 0.39, true);
	Zone mumbai = Zone(mumbai_loc, "Mumbai", 1247, 603, 0.809, 0.208, false);
	Zone zurich = Zone(zurich_loc, "Zurich", 38, 87.88, 0.10, 0.53, false);
	Zone cairo = Zone(cairo_loc, "Cairo", 675, 374, 0.346, 0.33, false);
	zones[0] = singapore;
	zones[1] = nyc;
	zones[2] = tokyo;
	zones[3] = mumbai;
	zones[4] = zurich; 
	zones[5] = cairo; 
	
	init_random_friendships();
}


void World::init_random_friendships() {
	// initialize friendships between zones with random values between 1 and 5
	double total = 100.0;
	double current = 0.0;
	for (int i = 0; i < NUM_ZONES; i++) {
		total = 100.0;
		for (int j = 0; j < NUM_ZONES; j++) {
			double r = (double) (rand() % 5);
			current = fmod(r,total) + 1.0;
			total -= current;
			this->zones[i].change_friendship(this->zones[j].get_name(), current/100.0);
		}
	}
}


/*
 * this function makes 1 time step in the world. Each step, new events happen. We can have a vaccine/ cure made if certain conditions are met, 
 * we can introduce new viruses to a zone, we propagate all active viruses, propagate vaccine and cures, evolve a virus or make it dormant, sell viruses and cures
 * from one zone to another zone, update the time that each person has left (each person has a natural time left calculated from their age and the value decreases
 * every step, but being infected drops this value faster). Once their time left reaches 0, the person dies and is remvoed from the zone. 
 * a step is a day
 */
void World::step() {
	// introduce virus to each zone at periodic intervals (500 days)
	int interval = VIRUS_INTERVAL;
	double start,end;
#ifdef DEBUG
	std::cout << "\n** IN WORLD.CPP **\n";
	std::cout << "Day: " << steps << "\n";
/*	for (int i = 0; i < NUM_ZONES; i++) {
	}*/
#endif
	for (int i = 0; i <NUM_ZONES; i++) {
		if (steps == interval*0) {
			std::string flu = "Flu";
			double str = 2.0 * S;

			start = CycleTimer::currentSeconds();
			this->zones[i].introduce_virus(Virus(2, flu, str, 0.70));	
			end = CycleTimer::currentSeconds();
#ifdef DEBUG
			printf("Introducing Flu took: %.3f ms in %s\n", 1000.f * (end-start), this->zones[i].get_name().c_str());
#endif
		}
		else if (steps == interval*1) {
			std::string hiv = "HIV";
			double str = 1.0 * S;

			start = CycleTimer::currentSeconds();
			this->zones[i].introduce_virus(Virus(9, hiv, str, 0.85));
			end = CycleTimer::currentSeconds();
#ifdef DEBUG
			printf("Introducing HIV took: %.3f ms in %s\n", 1000.f * (end-start), this->zones[i].get_name().c_str());
#endif
		}
		else if (steps == interval*2) {
			std::string ebola = "Ebola";
			double str = 3.0 * S;

			start = CycleTimer::currentSeconds();
			this->zones[i].introduce_virus(Virus(13, ebola, str, 0.45));
			end = CycleTimer::currentSeconds();
#ifdef DEBUG
			printf("Introducing Ebola took: %.3f ms %s\n", 1000.f * (end-start), this->zones[i].get_name().c_str());
#endif
		}
	}
	if (steps > 0) {
		// propagate virus in every zone for each active virus, each step after 0
		// update everyone's time left, depending on which viruses they are infected with
		for (int i = 0; i <NUM_ZONES; i++) {

			start = CycleTimer::currentSeconds();
			this->zones[i].propagate_virus();
			end = CycleTimer::currentSeconds();
#ifdef DEBUG
			printf("\npropagating virus took: %.3f ms in %s\n", 1000.f * (end-start), this->zones[i].get_name().c_str());
#endif

			this->zones[i].update_time_left();
		}

		// now, we check how many people have died and how much time has passed. If either has passed the limit
		// we get one of the zones to find vaccine and cure. Then, this zone has a limited amount of vaccine and cure
		// it passes them out to each other zone. 
		// check all viruses to see which ones needs a vaccine and cure for each zone
		// only active viruses (not dormant) need a new vaccine and cure
		Virus current;

		start = CycleTimer::currentSeconds();
		for (int i = 0; i < NUM_ZONES; i++) {
			Zone curr = this->zones[i];
			int num_vs = curr.get_num_viruses();

			for (int j = 0; j < num_vs; j++) {
				current = curr.get_viruses()[j];
				if (curr.found_vaccine_and_cure(current)) {
					distribute_vaccine_and_cure(current);
					// distribute the vaccine and cure for that virus
					// set the virus dormant
					current.set_dormant(1);
				}
			}
			this->zones[i] = curr;
		}
		end = CycleTimer::currentSeconds();
#ifdef DEBUG
		printf("\nDistributing vaccine and cure took: %.3f ms\n", 1000.f * (end-start));
#endif


		start = CycleTimer::currentSeconds();
		// now for all viruses that have been dormant for more than 100 steps, check if they can evolve
		for (int i = 0; i < NUM_ZONES; i++) {
			Zone curr = this->zones[i];
			int num_vs = curr.get_num_viruses();

			for (int j = 0; j < num_vs; j++) {
				current = curr.get_viruses()[j];
				if (current.get_dormant() > 100) {
					if (current.evolve_virus(curr)) {
						current.set_evolved(current.get_evolved() + 1);
						// update viruses status in the zone
						curr.update_virus(current);
					}
				}
			}
			this->zones[i] = curr;
		}
		end = CycleTimer::currentSeconds();
#ifdef DEBUG
		printf("\nEvolution of virus took: %.3f ms\n", 1000.f * (end-start));
#endif

	}	
	steps ++;
}


/*
 * function decides on one zone to make the vaccine and cure, and then distributes them to other zones that have the virus
 */
void World::distribute_vaccine_and_cure(Virus virus) {
	int num_zones = NUM_ZONES;
	std::map<std::string, double> zone_prob;
	double prob;
	//TODO prob always calculates to 0 inside the if statement and causes a crash later in the code
	//in order to compile the rest of the program, I hard coded a non 0 value for testing purposes
	for (int i=0; i < num_zones; i++) {
		// we look at the amount of people the virus killed in the zone
		// its healthcare and government levels to decide which zone to come up with the vaccine
		// some randomness involved too
		if (this->zones[i].has_virus(virus.get_name()) > -1) {
			prob =  (rand()%100 + 1.0)/100.0 * this->zones[i].get_location().healthcare * this->zones[i].get_location().government;
			prob *= (this->zones[i].get_virus_killed()[virus.get_name()] * 1.0/this->zones[i].get_initial_population());
			/****/
			assert(prob > 0);
			prob = 0.1;
			/****/
			zone_prob[this->zones[i].get_name()] = prob;
		}
	}

	// only look at infected for those are are infected with the virus
	double highest = 0.0;
	double current = 0.0;
	int total_infected = 0;
	Zone current_zone = Zone();
	Zone distributor = Zone();
	std::map<std::string, double>::iterator it;
	for (it = zone_prob.begin(); it != zone_prob.end(); it++) {
		current = it->second;
		int index = get_zone_index(it->first);
		current_zone =  this->zones[index];
		if (current > highest) {
			highest = current;
			distributor = current_zone;
		}
		total_infected += (current_zone.get_viruses()[current_zone.has_virus(virus.get_name())]).get_current_infected_num();
	}

	int evolved = virus.get_evolved();
    std::stringstream ss;
    ss << evolved;
    std::string str = ss.str();
	std::string add1 = "_Vaccine" + str;
	std::string add2 = "_Cure" + str;

	int id; 
	if (virus.get_name().compare(0,3,"Flu") == 0) {
		id = 6;
	}
	else if (virus.get_name().compare(0,3,"HIV") == 0) {
		id = 3;
	}
	else if (virus.get_name().compare(0,5,"Ebola") == 0) {
		id = 2;
	}
	Vaccine vaccine = Vaccine(id, virus.get_name() + add1, virus.get_name(), virus.get_evolved());
	Cure cure = Cure(id, virus.get_name() + add2, virus.get_name(), virus.get_evolved());
	
	int total_cures = total_infected*5/6;

	// now propagate the cure throughout each zone first
	// each zone gets different number of cures;
	// look at the friendship level of distributor, propagate cures to the best friend first with randomness
	// first let the zone that made the cure, cure all of their infected


	distributor.introduce_cure(cure);
	total_cures = distributor.propagate_cure(total_cures, cure);
	distributor.introduce_vaccine(vaccine);
	distributor.propagate_vaccine(vaccine);

	// go through zones with that virus and distribute cures according to their friendship levels
	for (it = zone_prob.begin(); it != zone_prob.end(); it++) {
		// make new cure and vaccine object for each zone
		Vaccine vaccine = Vaccine(id, virus.get_name() + add1, virus.get_name(), virus.get_evolved());
		Cure cure = Cure(id, virus.get_name() + add2, virus.get_name(), virus.get_evolved());
		// propagate cure and vaccine for each zone
		current_zone = this->zones[get_zone_index(it->first)];
		current_zone.introduce_cure(cure);
		total_cures = current_zone.propagate_cure(int(total_cures * distributor.get_friendship(current_zone.get_name())), cure);
		current_zone.introduce_vaccine(vaccine);
		current_zone.propagate_vaccine(vaccine);
	}
	init_random_friendships();
}

/* 
 * returns the zone with name we are looking for, or a null ptr if that zone does not exist
 */
int World::get_zone_index(std::string name) {
	for (int i = 0; i < NUM_ZONES; i++) {
		if (this->zones[i].get_name().compare(name) == 0) {
			return i;
		}
	}

	return -1;
}


int World::get_current_step() {
	return this->steps;
}

Zone* World::get_zone(int index)
{
	return &(this->zones[index]);
}

int World::get_num_zones() {
	return NUM_ZONES;
}
