Cure::Cure(int dna, string name, string virus, float strength, int cost) {
	dna = dna;
	name = name;
	strength = strength;
	cost = cost;
	cured_virus = virus;
}

/*
 * cures a person by making him not infected.
 */
void Cure::cure(Person person) {
	person.infected = false;
}



