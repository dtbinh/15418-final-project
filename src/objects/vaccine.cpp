#include "vaccine.hpp"

Vaccine::Vaccine() {}
Vaccine::~Vaccine() {}
Vaccine::Vaccine(int id, std::string name, std::string virus, int strength) {
	this->id = id;
	this->name = name;
	this->strength = strength;
	this->vaccinated_virus = virus;
}

/*
 * vaccinate a person by making him immune to the virus 
 * only vaccinates a person if he is originally healthly
 * and only vaccinates him against viruses tht return 0 modulo (vaccine.strength)
 */
void Vaccine::vaccinate(Person person) {
	if (person.infected[vaccinated_virus].first == false) { 
		person.vaccined[vaccinated_virus] = std::make_pair(true, this->id);
	}
}


std::string Vaccine::get_name() {
	return this->name;
}

std::string Vaccine::get_vaccinated_virus_name() {
	return this->vaccinated_virus;
}

int Vaccine::get_strength() {
	return this->strength;
}

int Vaccine::get_id() {
	return this->id;
}

void Vaccine::set_id(int value) {
	this->id = value;
}

void Vaccine::set_strength(int value) {
	this->strength = value;
}