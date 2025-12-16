#include "../../include/buildings/resident.hpp"

#include <iostream>
#include <string>
#include <memory>

using namespace std;

// Initialisation 
float Resident::WATER_PER_PERSON = 0.5f;
float Resident::ELECTRICITY_PER_PERSON = 0.3f;
float Resident::SATISFACTION_PER_PERSON = 0.1f;
float Resident::POLLUTION_PER_PERSON = 0.05f;
int Resident::BASE_CAPACITY_HOUSE = 6;
int Resident::BASE_CAPACITY_APARTMENT = 4;

// Setters 
void Resident::setWaterPerPerson(float value) { WATER_PER_PERSON = value; }
void Resident::setElectricityPerPerson(float value) { ELECTRICITY_PER_PERSON = value; }
void Resident::setSatisfactionPerPerson(float value) { SATISFACTION_PER_PERSON = value; }
void Resident::setPollutionPerPerson(float value) { POLLUTION_PER_PERSON = value; }
void Resident::setBaseCapacityHouse(int value) { BASE_CAPACITY_HOUSE = value; }
void Resident::setBaseCapacityApartment(int value) { BASE_CAPACITY_APARTMENT = value; }

// Constructor
Resident::Resident(int id, const std::string &nom, Ville *ville,
                   TypeBatiment type, int effectSatisfication, double cost,
                   double consommationEau, double consommationElectricite,
                   float polution, int x, int y, int largeur, int longeur,
                   int capaciteHabitants, int habitantsActuels)
    : capaciteHabitants(capaciteHabitants), habitantsActuels(habitantsActuels),
      Batiment(id, nom, ville, type, effectSatisfication, cost, consommationEau,
               consommationElectricite, polution, x, y, largeur, longeur) {}

Resident::Resident(int id, const std::string &nom, Ville *ville,
                   TypeBatiment type, int effectSatisfication, double cost,
                   Resources consommation, float polution, Position position,
                   Surface surface, int capaciteHabitants, int habitantsActuels)
    : capaciteHabitants(capaciteHabitants), habitantsActuels(habitantsActuels),
      Batiment(id, nom, ville, type, effectSatisfication, cost, consommation,
               polution, position, surface) {}

// Methods
void Resident::afficheDetails() const {
  Batiment::afficheDetails();
  if (ImGui::CollapsingHeader("Resident Info", ImGuiTreeNodeFlags_DefaultOpen)) {
  ImGui::Text("Habitants Actuels %d",habitantsActuels);
  ImGui::Text("Capacite Habitants %d",capaciteHabitants);
  ImGui::Text("Eau par personne : %.2f L/s",WATER_PER_PERSON);
  ImGui::Text("Eau par personne : %.2f W/s",ELECTRICITY_PER_PERSON);
  ImGui::Text("Satisfaction par personne : %.2f %%",SATISFACTION_PER_PERSON);
  }
}

void Resident::ajouterHabitants(int nombreHabitants) {
  habitantsActuels += nombreHabitants;
  if (habitantsActuels >= capaciteHabitants)
    habitantsActuels = capaciteHabitants;
 
  consommation.eau = habitantsActuels * WATER_PER_PERSON;
  consommation.electricite = habitantsActuels * ELECTRICITY_PER_PERSON;
}

void Resident::retirerHabitants(int nombreHabitants) {
  if (nombreHabitants >= habitantsActuels)
    habitantsActuels = 0;
  else
    habitantsActuels -= nombreHabitants;
  
  consommation.eau = habitantsActuels * WATER_PER_PERSON;
  consommation.electricite = habitantsActuels * ELECTRICITY_PER_PERSON;
}

BatPtr Resident::createHouse(Ville *ville,
                             int x, int y) {
    // Auto-generate name and ID
    string generatedName = NameGenerator::getRandomName(TypeBatiment::House);
    Position position(x, y);
    Surface surface(1, 1);
    int generatedID = BuildingIDGenerator::generateID(generatedName, TypeBatiment::House, position, surface);
    
    float pollution = POLLUTION_PER_PERSON * BASE_CAPACITY_HOUSE;
    int satisfaction = static_cast<int>(
        SATISFACTION_PER_PERSON * BASE_CAPACITY_HOUSE * 10
    );
    float water = WATER_PER_PERSON * BASE_CAPACITY_HOUSE;
    float electricity = ELECTRICITY_PER_PERSON * BASE_CAPACITY_HOUSE;

    return BatPtr(
        new Resident(
            generatedID, generatedName, ville, TypeBatiment::House,
            satisfaction, 30.0,
            water, electricity, pollution,
            x, y, 1, 1,
            BASE_CAPACITY_HOUSE, 0
        )
    );
}



// Getters
int Resident::gethabitantsActuels() { return habitantsActuels; }
int Resident::getcapaciteHabitants() { return capaciteHabitants; }
