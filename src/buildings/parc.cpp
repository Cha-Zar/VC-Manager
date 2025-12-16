#include "../../include/buildings/parc.hpp"
#include <iostream>

Parc::Parc(int id, const string &nom, Ville *ville, TypeBatiment type,
           int effectSatisfication, double cost, unsigned int Employees,
           unsigned int EmployeesNeeded, Resources consommation, float polution,
           Position position, Surface surface)
    : Service(id, nom, ville, type, effectSatisfication, cost, Employees,
              EmployeesNeeded, consommation, polution, position, surface) {}

Parc::Parc(int id, const string &nom, Ville *ville, TypeBatiment type,
           int effectSatisfication, double cost, unsigned int Employees,
           unsigned int EmployeesNeeded, double consommationEau,
           double consommationElectricite, float polution, int x, int y,
           int largeur, int longeur)
    : Service(id, nom, ville, type, effectSatisfication, cost, Employees,
              EmployeesNeeded, consommationEau, consommationElectricite,
              polution, x, y, largeur, longeur) {}

void Parc::diminuerPollution() {
  ville->setPolution(ville->getPolution() - (surface.largeur * surface.longeur *
                                             Parc::POLLUTION_REDUCTION_FACTOR));
}

void Parc::ameliorerBienEtre() {
  ville->setSatisfaction(
      ville->getSatisfaction() +
      (float)(surface.largeur * surface.longeur * effectBienEtre));
}

void Parc::afficheDetails() const {
  Batiment::afficheDetails();
  ImGui::Separator();
  ImGui::Text("Park Info:");
  ImGui::Separator();
  ImGui::Text("effect Bien Etre : %d", effectBienEtre);
}

BatPtr Parc::createPark(Ville *ville, int x, int y) {
  // Auto-generate name and ID
  string generatedName = NameGenerator::getRandomName(TypeBatiment::Park);
  Position position(x, y);
  Surface surface(2, 2);
  int generatedID = BuildingIDGenerator::generateID(generatedName, TypeBatiment::Park, position, surface);
  
  // Parks: 2x2 surface, reduce pollution, use water for plants/fountains, minimal electricity
  // Realistic values for a park:
  // - Cost: 1500 (moderately expensive to build)
  // - Satisfaction effect: 80 (parks increase well-being)
  // - Water consumption: 25 L/s (plants, fountains)
  // - Electricity consumption: 5 W/s (lighting, minimal)
  // - Pollution: -15 (NEGATIVE - parks reduce pollution!)
  // - Employees: 8 (maintenance staff)
  // - Employees needed: 8
  
  return BatPtr( new Parc(generatedID, generatedName, ville, TypeBatiment::Park, 
              80,           // satisfaction
              1500.0,       // cost
              8,            // employees
              8,            // employees needed
              25.0,         // water consumption
              5.0,          // electricity consumption
              -15.0f,       // pollution (negative = reduces)
              x, y, 2, 2));  // position and surface (2x2)
}

