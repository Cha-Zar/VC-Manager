#include "../../include/buildings/batiment.hpp"
#include <iostream>
#include <string>

using namespace std;

float Batiment::POLLUTION_SATISFACTION_FACTOR = 0.5f;
float Batiment::SATISFACTION_POPULATION_FACTOR = 0.3f;
float Batiment::BUILDING_POLLUTION_FACTOR = 1.0f;
float Batiment::POPULATION_BUILDING_FACTOR = 0.1f;


void Batiment::setPollutionSatisfactionFactor(float factor) {
    POLLUTION_SATISFACTION_FACTOR = factor;
}

void Batiment::setSatisfactionPopulationFactor(float factor) {
    SATISFACTION_POPULATION_FACTOR = factor;
}

void Batiment::setBuildingPollutionFactor(float factor) {
    BUILDING_POLLUTION_FACTOR = factor;
}

void Batiment::setPopulationBuildingFactor(float factor) {
    POPULATION_BUILDING_FACTOR = factor;
}
// Constructors
Batiment::Batiment(int id, const string &nom, Ville *ville, TypeBatiment type,
                   int effectSatisfication, double cost, double consommationEau,
                   double consommationElectricite, float polution, int x, int y,
                   int largeur, int longeur)
    : id(id), nom(nom), ville(ville), type(type),
      effectSatisfication(effectSatisfication), cost(cost),
      consommation(consommationEau, consommationElectricite), position(x, y),
      surface(largeur, longeur) {
  if (ville)
    ville->setBudget(ville->getBudget() - cost);
}

Batiment::Batiment(int id, const string &nom, Ville *ville, TypeBatiment type,
                   int effectSatisfication, double cost, Resources consommation,
                   float polution, Position position, Surface surface)
    : id(id), nom(nom), ville(ville), type(type),
      effectSatisfication(effectSatisfication), cost(cost),
      consommation(consommation), position(position), surface(surface) {
  if (ville)
    ville->setBudget(ville->getBudget() - cost);
}

// methods
void Batiment::afficheDetails() const {
  std::cout << "Batiment Info :" << endl;
  std::cout << "ID :\t" << id << endl;
  std::cout << "Nom :\t" << nom << endl;
  std::cout << "Nom de ville :\t" << (ville ? ville->getNom() : "N/A") << endl;
  std::cout << "effectSatisfication :\t" << effectSatisfication << endl;
  std::cout << "consommationEau  :\t" << consommation.eau << "Liter/s" << endl;
  std::cout << "consommationElectricite  :\t" << consommation.electricite
            << "Watt/s" << endl;
  std::cout << "Position : (" << position.x << ", " << position.y << ")"
            << endl;
  std::cout << "Surface : (" << surface.largeur << ", " << surface.longeur
            << ")" << endl;
}

void Batiment::impacterRessources() {
  ville->setResources(ville->getResources() - consommation);
}

// getters
int Batiment::getID() { return id; }
Resources Batiment::getconsommation() { return consommation; }
int Batiment::getSatisfaction() { return effectSatisfication; }
float Batiment::getPolution() { return polution; }
double Batiment::getCost() { return cost; }
string Batiment::getNom() const { return nom; }
