#include "zone.hpp"

#define H 500
#define W 1000

#define DEBUG

#define BLOCKWIDTH 16
#define BLOCKHEIGHT 16
#define BLOCKSIZE (BLOCKWIDTH*BLOCKHEIGHT)



Zone::Zone() {}
Zone::~Zone() {}
Zone::Zone(Location location, std::string name, int population, double area, float x, float y,
		   bool wealthy) {
	this->name = name;
	this->loc = location;
	this->initial_population = population;
	this->population = population;
	this->area = area;
	this->num_viruses = 0;
	// add in a newly create bunch of person structs
	this->people = new Person [population];
	this->x = x;
	this->y = y;

	double radius = sqrt(area/M_PI);

	int use = 0;
	float wealth = 0.0; 

	// give each person random attributes 
	for (int i = 0; i < population; i++) {
		// set person's age randomly between 1-100
		this->people[i].age = rand() * 100 + 1;
		// set virus infected and immune to false
		this->people[i].infected["HIV"] = std::make_pair(false,-1);
		this->people[i].infected["Flu"] = std::make_pair(false,-1);
		this->people[i].infected["Ebola"] = std::make_pair(false,-1);

		this->people[i].immune["HIV"] = false;
		this->people[i].immune["Flu"] = false;
		this->people[i].immune["Ebola"] = false;
		// number of days left to live their natural life
		this->people[i].time_left = (101 - people[i].age)*365;

		// also set person's x and y coordinates here
		this->people[i].position_x = ((double)rand()/(RAND_MAX)) * 2*radius - radius;
		this->people[i].position_y = ((double)rand()/(RAND_MAX)) * 2*radius - radius;

		// now find a random percentile value for this person's wealth
		use = rand()%4 + 1;
		// if this is meant to be a wealthy zone, then more people are in the upper 50% of world's wealth with a 3/4 chance
		if (wealthy) {
			if (use < 4) {
				// return a fraction between 0.5 and 1.0
				wealth = (rand()%50 + 1.0) / 100.0 + 0.5;
			}
			else {
				// return a fraction between 0.0 and 0.5
				wealth = (rand()%50 + 1.0) / 100.0;
			}
		}
		// else people are in the lower 50% of world's wealth with a 3/4 chance
		else {
			if (use < 4) {
				// return a fraction between 0.0 and 0.5
				wealth = (rand()%50 + 1.0) / 100.0;
			}
			else {
				// return a fraction between 0.5 and 1.0
				wealth = (rand()%50 + 1.0) / 100.0 + 0.5;
			}
		}

		// now we use the person's percentile wealth value to determine their net worth. 
		// the higher the percentile, their net worth becomes exponentially greater
		this->people[i].net_worth = exp(2.0*wealth);
	}
}

/*
 * returns the sum of the net_worth of everyone in the zone
 */
double Zone::calculate_wealth() {
	double sum = 0.0;
	for (int i = 0; i < this->initial_population; i++) {
		// if person is not dead
		if (this->people[i].dead != true) {	
			sum += this->people[i].net_worth;
		}
	}

	return sum;
}

/*
 * returns the population density for this zone
 */
double Zone::calculate_population_density() {
	return this->population/this->area;
}


/*
 * returns the name of the city this zone represents
 */
std::string Zone::get_name() {
	return this->name;
}


/*
 * sets the population to a certain number
 */
void Zone::set_population(int size) {
	this->population = size;
}

/*
 * gets the initial population of the zone
 */
int Zone::get_initial_population() {
	return this->initial_population;
}

/*
 * gets the area in km squared
 */
double Zone::get_area() {
	return this->area;
}


/*
 * gets the current population of the zone
 */
int Zone::get_population() {
	return this->population;
}


/*
 * gets the location of  this zone
 */
Zone::Location Zone::get_location() {
	return this->loc;
}


/*
 * returns the total number of dead people in the zone since start of simulation
 */
int Zone::get_total_dead() {
	return this->initial_population - this->population;
}

/*
 * returns the person at position i in people
 */ 
Person Zone::get_person(int i) {
	return this->people[i];
}

/*
 * function to call when a person dies
 */
void Zone::person_dies(int i) {
	this->people[i].dead = true;
	this->population = this->population-1;
}

/*
 * gets the number of viruses in the zone
 */
int Zone::get_num_viruses() {
	return this->num_viruses;
}

/*
 * returns the vector of all viruses infecting this zone
 */
std::vector<Virus> Zone::get_viruses() {
	return this->viruses;
}

/*
 * returns the map mapping viruses to the number of people they have killed
 */
std::map<std::string, int> Zone::get_virus_killed(){
	return this->virus_killed;
}

/*
 * returns the index of the virus in vector 'viruses' if it is vector, else 
 * returns -1
 */
int Zone::has_virus(std::string name) {
	std::string current;
	for (int i = 0; i < this->num_viruses; i++) {
		current = this->viruses[i].get_name();
		if (current.compare(name) == 0) {
			return i;
		}
	}

	return -1;
}

/*
 * returns the friendship level of this zone with another zone
 */
int Zone::get_friendship(std::string name){
	return this->friendship[name];
}

/*
 * changes the friendship level  of this zone with another zone to value
 */
void Zone::change_friendship(std::string name, int value){
	this->friendship[name] = value;
}


/*
 * returns the vector of vaccines for viruses in this zone
 */ 
std::vector<Vaccine> Zone:: get_vaccines() {
	return this->vaccines;
}

/*
 * returns the vector of cures for viruses in this zone
 */ 
std::vector<Cure> Zone::get_cures() {
	return this->cures;
}

/*
 * This is a method to introduce a new virus into a zone
 * Starts by infecting INITIAL_INFECTED number of people in each zone
 */
void Zone::introduce_virus(Virus virus) {
	this->virus_active_time[virus.get_name()] = 0;
	this->virus_killed[virus.get_name()] = 0;
	virus.set_dormant(0);

	this->num_viruses += 1;
	int pop = this->initial_population;
	Person current; 
	int infected = 0;
	int i = 0;

	int initial = INITIAL_INFECTED;
	// infect random INITIAL_INFECTED number of people
	while (infected < initial) {
		i = rand() % pop;
		current = people[i];
		if (!current.dead) {
			// infect person only if not dead yet
			if (virus.infect(*this,current,current, true)) {
				infected += 1;
				// add new infected person to map with original virus id
				virus.add_infected_person(i, virus.get_id());
				current.infected[virus.get_name()] = std::make_pair(true,virus.get_id());
				people[i] = current;
			}
		}
	}
	this->viruses.push_back(virus);
}

/*
 * returns an array of all people within +- strength units of infected person's x,y coordinates
 */
std::vector<int> Zone::get_nearest_people(Person infected, double strength) {
	// go through all people in the zone and add them to a vector of infected people
	std::vector<int> nearest;
	double x = infected.position_x;
	double y = infected.position_y;
	Person current;
	for (int i = 0; i < this->initial_population; i++) {
		current = this->people[i];
		// if current person is near enough and not dead, add him
		if (current.dead) {
			continue;
		}
		if (abs(current.position_x-x) <= strength || abs(current.position_y-y) <= strength) {
			nearest.push_back(i);
		}
	}
	return nearest;
}
/*
 * this is a method to propagate a virus in a zone. It goes through 
 * all infected people in the zone and tries to infect the people nearest to them
 * propagates all viruses in the system
 */
void Zone::propagate_virus() {
	Virus current;
	Person infector;
	Person infecting; 
	std::vector<int> nearest;

#ifdef DEBUG
	int hiv_mutations = 0;
	int flu_mutations = 0;
	int ebo_mutations = 0;

	int hiv_infections = 0;
	int flu_infections = 0;
	int ebo_infections = 0;
#endif

	for (int i = 0; i < this->num_viruses; i++) {
		current = this->viruses[i];
		if (current.get_dormant() == 0) {
			this->virus_active_time[current.get_name()]++;
		}
		int infect_num = current.get_current_infected_num();
		// go through each alive infected person of the current virus
		for (int j = 0; j < infect_num; j++) {
			infector = current.get_infected_people()[j];
			if (infector.dead) {
				continue;
			}
			nearest = get_nearest_people(infector, current.get_strength());
			// go through each of the nearest people and infect them
			for (int k = 0; k < nearest.size(); i++) {
				int index = nearest[k];
				infecting = people[index];
				if (infecting.dead) {
					continue;
				}
				if (current.infect(*this, infector, infecting,false)) {
					if ((rand()%100)/100.0 <= current.get_mutation_prob()) {
						// add new infected person to map of infected person with new mutated id
						int id = rand();
						current.add_infected_person(index, id);
						infecting.infected[current.get_name()] = std::make_pair(true,id);

#ifdef DEBUG
						std::string name = current.get_name();
						if (name.compare("HIV")==0)
							hiv_mutations++;
						else if (name.compare("Flu") == 0)
							flu_mutations++;
						else if (name.compare("Ebola") == 0)
							ebo_mutations++;
#endif
					}
					else {
						// add new infected person to map with old id 
						current.add_infected_person(index, current.get_id());
						infecting.infected[current.get_name()] = std::make_pair(true,current.get_id());
					}
#ifdef DEBUG
					std::string name = current.get_name();
					if (name.compare("HIV")==0)
						hiv_infections++;
					else if (name.compare("Flu") == 0)
						flu_infections++;
					else if (name.compare("Ebola") == 0)
						ebo_infections++;
#endif
				}
				people[index] = infecting;
			}
			current.get_infected_people()[j] = infector;
		}
		this->viruses[i] = current;
	}

#ifdef DEBUG
	std::cout << "\n*** Zone: " << this->name << " ***\n";
	std::cout << "Infected with HIV: " << hiv_infections << "\n";
	std::cout << "Infected with Influenza: " << flu_infections << "\n";
	std::cout << "Infected with Ebola: " << ebo_infections << "\n";
	std::cout << "\n";
	std::cout << "Total amount of distinct HIV viruses: " << hiv_mutations << "\n";
	std::cout << "Total amount of distinct Influenza viruses: " << flu_mutations << "\n";
	std::cout << "Total amount of distinct Ebola viruses: " << ebo_mutations << "\n";
#endif
}



/*
 * updates the time left of healthy people by decreasing it by 1 (1 day)
 * for infected people who are not immune to the virus, they get their time left 
 * decremented by a multiplier. For infected people who are immune to virus, 
 * same as healthly people
 * doesn't change anything for dead people
 */ 
void Zone::update_time_left() {
	Person current;
	bool is_healthy = true;
	for (int i = 0; i < this->initial_population; i++) {
		current = this->people[i];
		if (current.dead) {
			continue;
		}
		// if virus is dormant, then people time left isn't affected.
		if (current.infected["Flu"].first && !current.immune["Flu"]) {
			// loses 5 days of their life for each day infected with the flu
			if (this->viruses[0].get_dormant() == 0) {
				current.time_left -= 5;
				is_healthy = false;
				if (current.time_left <= 0) {
					this->virus_killed["Flu"]++;
				}
			}
		}

		if (current.infected["HIV"].first && !current.immune["HIV"]) {
			// loses 10 days of their life for each day infected with the hiv
			if (this->viruses[1].get_dormant() == 0) {
				current.time_left -= 10;
				is_healthy = false;
				if (current.time_left <= 0) {
					this->virus_killed["HIV"]++;
				}
			}
		}

		if (current.infected["Ebola"].first && !current.immune["Ebola"]) {	
			// loses 150 days of their life for each day infected with the flu
			if (this->viruses[2].get_dormant() == 0) {
				current.time_left -= 150;
				is_healthy = false;
				if (current.time_left <= 0) {
					this->virus_killed["Ebola"]++;
				}
			}
		}

		if (is_healthy) {
			current.time_left -= 1;
		}

		// now call person dead function
		if (current.time_left <= 0) {
			person_dies(i);
		}
		this->people[i] = current;
	}
}



/*
 * whenever a virus has evolved, we call this to update the virus's name and 
 * introduce it into the zone again
 */ 
void Zone::update_virus(Virus virus) {
	int evolved = virus.get_evolved();
    std::stringstream ss;
    ss << evolved;
    std::string str = ss.str();
	virus.set_name(virus.get_name() + this->name + str);

	// add it active time map and killed map but don't push it to list of viruses infecting the zone
	// this is because it is just an evolved virus, 
	this->virus_active_time[virus.get_name()] = 0;
	this->virus_killed[virus.get_name()] = 0;
	virus.set_dormant(0);

	int pop = this->initial_population;
	Person current; 
	int infected = 0;
	int i = 0;

	int initial = INITIAL_INFECTED;
	// infect random INITIAL_INFECTED number of people
	while (infected < initial) {
		i = rand() % pop;
		current = people[i];
		if (!current.dead) {
			// infect person only if not dead yet
			if (virus.infect(*this,current,current, true)) {
				infected += 1;
				// add new infected person to map with original virus id
				virus.add_infected_person(i, virus.get_id());
				current.infected[virus.get_name()] = std::make_pair(true,virus.get_id());
				people[i] = current;
			}
		}
	}
}


/*
 * checks if enough time has elapsed to make a vaccine and cure or if so many people have died such that
 * the time taken to need the vaccine has been sped up
 * if virus is a dormant virus, then return false too
 */
bool Zone::found_vaccine_and_cure(Virus virus) {
	if (virus.get_dormant() > 0) {
		return false;
	}
	int interval = VIRUS_INTERVAL;
	if (this->virus_active_time[virus.get_name()] >= interval/2) {
		return true;
	}

	if (this->virus_killed[virus.get_name()] > initial_population/50.0) {
		return true;
	}

	return false;
}

/*
 * This is a method to introduce a vaccine into the zone
 */
void Zone::introduce_vaccine(Vaccine vac) {
	// if virus that this vaccine vaccinates is an evolved virus, we replace the vaccine in vector rather than add new one
	int i = has_virus(vac.get_vaccinated_virus_name());
	if (viruses[i].get_evolved() > 0) {
		vaccines[i] = vac;
	}
	else {
		this->vaccines.push_back(vac);
	}
}

/*
 * This is a method to introduce a cure into the zone
 */
void Zone::introduce_cure(Cure cure) {
	// if virus that this cure cures is an evolved virus, we replace the cure in vector rather than add new one
	int i = has_virus(cure.get_cured_virus_name());
	if (viruses[i].get_evolved() > 0) {
		cures[i] = cure;
	}
	else {
		this->cures.push_back(cure);
	}
}


/*
 * this is a method to propagate a vaccine into a zone. It vaccines everyone
 * in the zone 
 */
void Zone::propagate_vaccine(Vaccine vaccine) {
	// go through everyone in the zone and vaccine everyone who is alive
	for (int i = 0; i < this->population; i++) {
		if (!people[i].dead) {
			vaccine.vaccinate(people[i]);
		}
	}
}


/*
 * this is a method to propagate a cure into a zone. It cures as many people as it can
 * It then returns the total number of cures left over
 */
int Zone::propagate_cure(int total_cures, Cure cure) {
	int current_cures = total_cures;

	// go through each infected person of this virus
	Virus virus = this->viruses[has_virus(cure.get_cured_virus_name())];
	std::vector<Person> infected = virus.get_infected_people();

	// cure the people who contracted the virus first if they are still alive
	for (int i = 0; i < infected.size(); i++) {
		if (!infected[i].dead) {
			cure.cure(infected[i]);
			current_cures--;
			if (current_cures == 0) {
				break;
			}
		}
	}

	return current_cures;
}



////////////////////////////////////////////////////////////////////////////////////////
// Putting all the cuda kernels here
///////////////////////////////////////////////////////////////////////////////////////

void Zone::introduce_new_virus(Virus* virus, int infected) {
	int i = rand() % initial_population;
	if (!people[i].dead && !people[i].infected[virus->get_name()].first ) {
		// infect person only if not dead yet and not infected yet
		if (virus->infect(*this,people[i],people[i], true)) {
			infected += 1;
			// add new infected person to map with original virus id
			virus->add_infected_person(i, virus->get_id());
			people[i].infected[virus->get_name()] = std::make_pair(true,virus->get_id());
		}
	}
}

// kernel_parallel_introduce_virus -- (CUDA device code)
//
// Each thread infects 1 random person until we have reached INITIAL_INFECTED

__global__ void Zone::kernel_parallel_introduce_virus(Virus virus) {
	// set up some variables first
	virus_active_time[virus.get_name()] = 0;
	virus_killed[virus.get_name()] = 0;
	virus.set_dormant(0);

	num_viruses += 1;

	__shared__ int infected = 0;

	int initial = INITIAL_INFECTED;
	// infect random INITIAL_INFECTED number of people
	while (infected < initial) {
		introduce_new_virus(&virus, infected);
		__syncthreads();
	}
	viruses.push_back(virus);
}
/*
 * this function infects people in parallel. However, for now, it works in parallel only on all currently infected people
 * it still has to serially go through each of the nearest people to them.
 */
__device__ __inline__ void Zone::infect_all_nearest1(unsigned int* working_threads, Virus virus, Person* infector,
													int current_infecting) 
{
	// get thread id
	int threadID = threadIdx.y * blockDim.x + threadIdx.x;

	// get infectingID, distinct person to infect for each thread
	int infectingID = current_infecting + threadID;

	// get person to infect and infect them if they are alive
	Person infecting = people[infectingID]; 
	if (infecting.dead) {
		// if dead, thread does no more work
		return;
	}

	if (virus.infect(*this, *infector, infecting,false)) {
		if ((rand()%100)/100.0 <= virus.get_mutation_prob()) {
		// add new infected person to map of infected person with new mutated id
			int id = rand();
			virus.add_infected_person(infectingID, id);
			infecting.infected[virus.get_name()] = std::make_pair(true,id);
		}
		else {
			// add new infected person to map with old id 
			virus.add_infected_person(infectingID, virus.get_id());
			infecting.infected[virus.get_name()] = std::make_pair(true,virus.get_id());
		}
	}
	people[infectingID] = infecting;
}

/*
 * this function infects people in parallel. However, for now, it works in parallel only on all currently infected people
 * it still has to serially go through each of the nearest people to them.
 */
__device__ __inline__ void Zone::infect_all_nearest2(unsigned int* working_threads, Virus virus, 
												std::map<Person*, std::vector<int> >::iterator it, 
												int current_infected) 
{
	// get thread id
	int threadID = threadIdx.y * blockDim.x + threadIdx.x;

	// get infectedID, distinct infected for each thread
	std::map<Person*, std::vector<int> >::iterator infectedID = it;
	for (int i = 0; i < threadID; i ++) {
		// forced to do this cause iterators don't have operator +=, only ++
		infectedID ++;
	}

	// get the infected person this thread is looking at
	Person* infector = infectedID->first;
	std::vector<int> nearest = infectedID->second;

	Person infecting; 
	// go through each of the nearest people and infect them
	for (int k = 0; k < nearest.size(); k++) { 
		int index = nearest[k];
		infecting = people[index];
		if (infecting.dead) {
			continue;
		}
		if (virus.infect(*this, *infector, infecting,false)) {
			if ((rand()%100)/100.0 <= virus.get_mutation_prob()) {
				// add new infected person to map of infected person with new mutated id
				int id = rand();
				virus.add_infected_person(index, id);
				infecting.infected[virus.get_name()] = std::make_pair(true,id);
			}
			else {
				// add new infected person to map with old id 
				virus.add_infected_person(index, virus.get_id());
				infecting.infected[virus.get_name()] = std::make_pair(true,virus.get_id());
			}
		}
		people[index] = infecting;
	}
}

/*
 * this function works in parallel to look through BLOCKSIZE - 1 number of infected people by the virus 
 * and adds the vector of all nearest people to each infected person to a map as the value, with the key 
 * being a pointer to the infected person.
 */
__device__ __inline__ void Zone::get_all_nearest(unsigned int* working_threads, Virus virus,
												std::map<Person*, std::vector<int> > nearest, int propagated) 
{
	// get thread id
	int threadID = threadIdx.y * blockDim.x + threadIdx.x;

	// get infectedID, distinct infected for each thread
	int infectedID = propagated + threadID;

	// get the infected person this thread is looking at
	Person* infector = &virus.get_infected_people()[infectedID];
	
	// if that person is dead, thread is done
	if (infector->dead) {
		return;
	}

	else { 
		// we get all the nearest people for that person and add them both to map
		// only for all threads doing work
		if ((threadID < BLOCKSIZE) && working_threads[threadID] > 0) { 
			// get nearest people for this guy
			// add to map
			nearest[infector] = get_nearest_people(*infector, virus.get_strength());
		}
	}
}

/**
 * This function partitions up the threads into those doing work and those not doing work
 * in the get nearest people and infect people kernels. It does so by working in parallel.
 * For now, threads do work till there are too many threads and too little infected people
 **/

__device__ __inline__ void Zone::partition_propagate_threads(unsigned int* working_threads, Virus virus, int propagated, int total_infected) {

	//get thread id
	int threadID = threadIdx.x * blockDim.x + threadIdx.y;
	
	// set workingThread entry to 0 first to show it is not used
	working_threads[threadID] = 0;
	
	// get unique infected id
	int infectedID = propagated + threadID;
	
	// check that infected id less than total number of infected
	// only do work in threads till we reach end of infected ppl
	if (infectedID < total_infected) {
		working_threads[threadID] = 1;
	}
}

// kernel_parallel_propagate_virus -- (CUDA device code)
//
// Each thread propagates one infected person
// note that we don't need to worry about two threads trying to infect the same person
// as we can just make the region right before they check for infection atomic

__global__ void Zone::kernel_parallel_propagate_virus() {
	// array that contains 0s and 1s, a 1 at a specific index means that thread is 
	// doing work in that block
	__shared__ unsigned int working_threads1[BLOCKSIZE];

	__shared__ unsigned int working_threads2[BLOCKSIZE];
	
	// map mapping each infected person to the list of all nearest people, for all infected people in each block
	__shared__ std::map<Person*, std::vector<int> > nearest;
	
	
	Virus current;

	for (int i = 0; i < this->num_viruses; i++) {
		current = this->viruses[i];
		if (current.get_dormant() == 0) {
			this->virus_active_time[current.get_name()]++;
		}

		int infect_num = current.get_current_infected_num();
		
		// main parallel for loops that that works in block to propagate each viruses
		// first loop goes through all infected people and adds all the nearest people 
		// to a map, block by block
		for (int propagated = 0; propagated < infect_num; propagated += BLOCKSIZE) {
			// get partition of threads in parallel to know which threads will do work
			// and which won't be doing work.
			partition_propagate_threads(working_threads1, current, propagated, infect_num);
			__syncthreads();

			// get list of all infected people and all the people nearest to them so we know who to infect
			// in parallel
			get_all_nearest(working_threads1, current, nearest, propagated);
			__syncthreads();

			
			// try going through each infected person in serial and infect their neighbours in parallel
			// more speedup since we have a lot more neighbours than infected people
			std::map<Person*, std::vector<int> >::iterator current_infected; 
			std::vector<int>::iterator it;

			for (current_infected = nearest.begin(); current_infected != nearest.end(); current_infected ++) {
				int total = current_infected->second.size();

				for (int infecting = 0; infecting < total; infecting += BLOCKSIZE) {
					// infect all nearest people
					infect_all_nearest1(working_threads2, current, current_infected->first, infecting);
					__syncthreads();
				}
			}
		}
		
		/*
		// second loop now goes all the nearest people to each infected person 
		// and tries to infect them block by block
		infect_num = nearest.size();
		std::map<Person*, std::vector<int> >::iterator it 
		int current_infected = 0;
		for (it = nearest.begin(); it != nearest.end(); it += BLOCKSIZE) {
			// get partition of threads in parallel to know which threads will do work
			// and which won't be doing work.
			partition_propagate_threads(working_threads2, current, current_infected, infect_num);
			__syncthreads();

			// infect all nearest people
			infect_all_nearest2(working_threads2, current, it, current_infected);
			__syncthreads();
			
			current_infected += BLOCKSIZE;
		}
		*/
		viruses[i] = current;
	}
}


/*
 * simply runs the vaccinate function on the person if he is alive, else thread does no work
 */
__device__ __inline__ void Zone::vaccinate_person(Vaccine vaccine, int person) {
	if (!people[person].dead) {
			vaccine.vaccinate(people[person]);
		}
}
// kernel_parallel_propagate_vaccine -- (CUDA device code)
//
// Each thread runs the vaccinate function on a person in the zone

__global__ void Zone::kernel_parallel_propagate_vaccine(Vaccine vaccine) {

	// we simply run the vaccinate person function in parallel on every person
	for (int vaccinate = 0; vaccinate < initial_population; vaccinate += BLOCKSIZE) {
		vaccinate_person(vaccine, vaccinate);
		__syncthreads();
	}
}

/*
 * cures a person if he is alive
 */
__device__ __inline__ void Zone::cure_person(Cure cure, int person, int current_cures, std::vector<Person> infected) {
	if (!infected[person].dead) {
		cure.cure(infected[person]);
		current_cures--;
		if (current_cures == 0) {
			return;
		}
	}
}


// kernel_parallel_propagate_vaccine -- (CUDA device code)
//
// Each thread runs the vaccinate function on a person in the zone

__global__ int Zone::kernel_parallel_propagate_cure(Cure cure,  int total_cures) {

	__shared__ int current_cures = total_cures; 

	// go through each infected person of this virus
	Virus virus = this->viruses[has_virus(cure.get_cured_virus_name())];
	std::vector<Person> infected = virus.get_infected_people();

	// we cure as many alive people as we can
	for (int i; i < infected.size(); i += BLOCKSIZE) {
		cure_person(cure, i, current_cures, infected);
		__syncthreads();
	}

	return current_cures;
}

/*
 * updates the time left of a healthly person by decreasing it by 1 (1 day)
 * for infected people who are not immune to the virus, they get their time left 
 * decremented by a multiplier. For infected people who are immune to virus, 
 * same as healthly people
 */ 
__device__ __inline__ void Zone::update_person_time_left(unsigned int* working_threads, int update, int total) {
	//get thread id
	int threadID = threadIdx.x * blockDim.x + threadIdx.y;
	
	// get unique infected id
	int updateID = update + threadID;

	Person current; 

	// only do work for thread that has work 
	if (threadID < BLOCKSIZE && working_threads[threadID] > 0) {
		// get person that we are updating time of
		current = people[updateID];
		bool is_healthy = true;
		// if virus is dormant, then people time left isn't affected.
		if (current.infected["Flu"].first && !current.immune["Flu"]) {
			// loses 5 days of their life for each day infected with the flu
			if (viruses[0].get_dormant() == 0) {
				current.time_left -= 5;
				is_healthy = false;
				if (current.time_left <= 0) {
					virus_killed["Flu"]++;
				}
			}
		}

		if (current.infected["HIV"].first && !current.immune["HIV"]) {
			// loses 10 days of their life for each day infected with the hiv
			if (viruses[1].get_dormant() == 0) {
				current.time_left -= 10;
				is_healthy = false;
				if (current.time_left <= 0) {
					virus_killed["HIV"]++;
				}
			}
		}

		if (current.infected["Ebola"].first && !current.immune["Ebola"]) {	
			// loses 150 days of their life for each day infected with the flu
			if (viruses[2].get_dormant() == 0) {
				current.time_left -= 150;
				is_healthy = false;
				if (current.time_left <= 0) {
					virus_killed["Ebola"]++;
				}
			}
		}

		if (is_healthy) {
			current.time_left -= 1;
		}

		// now call person dead function
		if (current.time_left <= 0) {
			person_dies(updateID);
		}

		people[updateID] = current;
	}
}

/**
 * This function partitions up the threads into those doing work and those not doing work
 * in the get nearest people and infect people kernels. It does so by working in parallel.
 * For now, threads do work till there are too many threads and too little infected people
 **/

__device__ __inline__ void Zone::partition_update_time_threads(unsigned int* working_threads, int update, int total) {

	//get thread id
	int threadID = threadIdx.x * blockDim.x + threadIdx.y;
	
	// set workingThread entry to 0 first to show it is not used
	working_threads[threadID] = 0;
	
	// get unique infected id
	int updateID = update + threadID;
	
	// check that update id is less than total population
	// only do work in block when thread has person to do work with
	// only do work for alive people
	if (updateID < total) {
		Person current = people[updateID];
		if (!current.dead) {
			working_threads[threadID] = 1;
		}
	}
}


// kernel_parallel_update_time_left -- (CUDA device code)
//
// Each thread updates the time left for a different person

__global__ void Zone::kernel_parallel_update_time_left() {
	// array that contains 0s and 1s, a 1 at a specific index means that thread is 
	// doing work in that block
	__shared__ unsigned int working_threads[BLOCKSIZE];
	
	for (int updated = 0; updated < initial_population; updated += BLOCKSIZE) {
		partition_update_time_threads(working_threads, updated, initial_population);
		__syncthreads();

		update_person_time_left(working_threads, updated, initial_population);
		__syncthreads();
	}
}


////////////////////////////////////////////////////////////////////////////////////////
// Parallel function using the cuda kernels
///////////////////////////////////////////////////////////////////////////////////////

void Zone::parallel_setup() {

    int deviceCount = 0;
    cudaError_t err = cudaGetDeviceCount(&deviceCount);

    printf("---------------------------------------------------------\n");
    printf("Initializing CUDA for Virus\n");
    printf("Found %d CUDA devices\n", deviceCount);

    for (int i=0; i<deviceCount; i++) {
        cudaDeviceProp deviceProps;
        cudaGetDeviceProperties(&deviceProps, i);
        printf("Device %d: %s\n", i, deviceProps.name);
        printf("   SMs:        %d\n", deviceProps.multiProcessorCount);
        printf("   Global mem: %.0f MB\n", static_cast<float>(deviceProps.totalGlobalMem) / (1024 * 1024));
        printf("   CUDA Cap:   %d.%d\n", deviceProps.major, deviceProps.minor);
    }
    printf("---------------------------------------------------------\n");
    
    // Now copy all the key data structures into device memory so they are accessible to CUDA kernels
	
	/*
    cudaMalloc(&(cudaDevicePeople), sizeof(Person) * this->initial_population);
	cudaMalloc(&(&(cudaDeviceVirusActiveTime, sizeof(std::string) * sizeof(int) * 3);
	cudaMalloc(&(&(cudaDeviceVirusKilled)), sizeof(std::string) * 3 + sizeof(int) * 3);
	cudaMalloc(&(&(cudaDeviceViruses)), sizeof(Virus) * 3);
	cudaMalloc(&(&(cudaDeviceVaccines)), sizeof(Vaccine) * 3);
	cudaMalloc(&(&(cudaDeviceCures)), sizeof(Cure) * 3);
	cudaMalloc(&(&(cudaDeviceLocation)), sizeof(Location));

    cudaMemcpy(cudaDevicePeople, this->people, sizeof(Person) * this->initial_population, cudaMemcpyHostToDevice);
    cudaMemcpy(&cudaDeviceVirusActiveTime, &this->virus_active_time(), sizeof(std::string) * sizeof(int) * 3, cudaMemcpyHostToDevice);
    cudaMemcpy(&cudaDeviceVirusKilled, &this->virus_killed, sizeof(std::string) * 3 + sizeof(int) * 3, cudaMemcpyHostToDevice);
    cudaMemcpy(&cudaDeviceViruses, &this->viruses, sizeof(Virus) * 3, cudaMemcpyHostToDevice);
	cudaMemcpy(&cudaDeviceVaccines, &this->vaccines, sizeof(Vaccine) * 3, cudaMemcpyHostToDevice);
	cudaMemcpy(&cudaDeviceCures, &this->cures, sizeof(Cure) * 3, cudaMemcpyHostToDevice);
	cudaMemcpy(&cudaDeviceLocation, &this->loc, sizeof(Location), cudaMemcpyHostToDevice);
	*/
}



void Zone::parallel_introduce_virus(Virus virus) {
	// pass the width and height of the block given to us
    dim3 blockDim(BLOCKWIDTH, BLOCKHEIGHT);
    
	// simply use 1 thread block
	dim3 gridDim(1,1);
    
    kernel_parallel_introduce_virus<<<gridDim, blockDim>>>(virus);
	// we synchronize in the kernel above so don't do it here anymore
}



void Zone::parallel_propagate_virus() {
	// pass the width and height of the block given to us
    dim3 blockDim(BLOCKWIDTH, BLOCKHEIGHT);
    
	// use the current number of ppl to get an idea how many people we might be infecting
	dim3 gridDim((this->population + blockDim.x  - 1) / blockDim.x , 
					(this->population + blockDim.y  - 1) / blockDim.y);
    
    kernel_parallel_propagate_virus<<<gridDim, blockDim>>>();
	// we synchronize in the kernel above so don't do it here anymore
}



void Zone::parallel_propagate_vaccine(Vaccine vaccine) {
	// pass the width and height of the block given to us
    dim3 blockDim(BLOCKWIDTH, BLOCKHEIGHT);
    
	// we go through everyone to see if we need to vaccinate them
	dim3 gridDim((this->initial_population + blockDim.x  - 1) / blockDim.x , 
					(this->initial_population + blockDim.y  - 1) / blockDim.y);
    
    kernel_parallel_propagate_vaccine<<<gridDim, blockDim>>>(vaccine);
	// we synchronize in the kernel above so don't do it here anymore
}



int Zone::parallel_propagate_cure(Cure cure, int total_cures) {
	// pass the width and height of the block given to us
    dim3 blockDim(BLOCKWIDTH, BLOCKHEIGHT);
    
	// we won't use on everyone, just everyone who is infected, so use current population to get an estimate
	dim3 gridDim((this->population + blockDim.x  - 1) / blockDim.x , 
					(this->population + blockDim.y  - 1) / blockDim.y);
    
    return kernel_parallel_propagate_cure<<<gridDim, blockDim>>>(cure, total_cures);
	// we synchronize in the kernel above so don't do it here anymore
}



void Zone::parallel_update_time_left() {
	// pass the width and height of the block given to us
    dim3 blockDim(BLOCKWIDTH, BLOCKHEIGHT);
    
	// we are updating all people in the zone so
	dim3 gridDim((this->initial_population + blockDim.x  - 1) / blockDim.x , 
					(this->initial_population + blockDim.y  - 1) / blockDim.y);
    
    kernel_parallel_update_time_left<<<gridDim, blockDim>>>();
	// we synchronize in the kernel above so don't do it here anymore
}
