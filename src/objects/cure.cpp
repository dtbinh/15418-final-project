#include "cure.hpp"


Cure::Cure() {}
Cure::~Cure() {}
Cure::Cure(int id, std::string name, std::string virus, int strength) {
	this->id = id;
	this->name = name;
	this->strength = strength;
	this->cured_virus = virus;
}

/*
 * cures a person by making him not infected.
 * then make him vaccinated against virus
 */
void Cure::cure(Person person) {
	person.infected[cured_virus] = std::make_pair(false, person.infected[cured_virus].second);
	person.vaccined[cured_virus] = std::make_pair(true, this->id);
}

std::string Cure::get_name() {
	return this->name;
}

std::string Cure::get_cured_virus_name() {
	return this->cured_virus;
}

int Cure::get_strength() {
	return this->strength;
}

int Cure::get_id() {
	return this->id;
}

void Cure::set_id(int value) {
	this->id = value;
}

void Cure::set_strength(int value) {
	this->strength = value;
}