
struct Person;
struct Location;

class Zone {

public: 
	Zone();
	~Zone();

	void calculate_wealth_distr();
	void calculate_age_distr();
	void calculate_living_distr();

private:
	char*  name;
	Location loc;
	int	   population;
	float* area;

	Person* people;
}
