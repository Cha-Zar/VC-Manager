#include "../../include/buildings/appartement.hpp"
#include <stdexcept>

Appartement::Appartement(int id, const std::string &nom, Ville *ville,
                         TypeBatiment type, int effectSatisfication,
                         double cost, Resources consommation, float polution,
                         Position position, Surface surface,
                         int capaciteHabitants, int habitantsActuels,
                         unsigned int floorsCount)
    : floorsCount(floorsCount),
      Resident(id, nom, ville, type, effectSatisfication, cost, consommation,
               polution, position, surface, capaciteHabitants,
               habitantsActuels) {}

Appartement::Appartement(int id, const std::string &nom, Ville *ville,
                         TypeBatiment type, int effectSatisfication,
                         double cost, double consommationEau,
                         double consommationElectricte, float polution, int x,
                         int y, int largeur, int longeur, int capaciteHabitants,
                         int habitantsActuels, unsigned int floorsCount)
    : floorsCount(floorsCount),
      Resident(id, nom, ville, type, effectSatisfication, cost, consommationEau,
               consommationElectricte, polution, x, y, largeur, longeur,
               capaciteHabitants, habitantsActuels) {}

// Getters
unsigned int Appartement::getFloorsCount() { return floorsCount; }

void Appartement::addNewFloor() {
  if (floorsCount <= MAX_FLOOR_COUNT) {
    floorsCount++;
    cost += COST_PER_FLOOR;
    polution += POLUTION_PER_FLOOR;
    effectSatisfication += EFFET_SATISFACTION_PER_FLOOR;
    consommation +=
        Resources(CONSOMMATION_EAU_PER_FLOOR, CONSOMMATION_ELE_PER_FLOOR);
    capaciteHabitants += MAX_HABITATS_PER_FLOOR;
  } else {
    std::invalid_argument("Floor count connot execed 4.");
  }
}

Appartement Appartement::createAppartement(int id, const string &nom,
                                           Ville *ville,
                                           unsigned int floorsCount, int x,
                                           int y) {
  if (floorsCount > MAX_FLOOR_COUNT) {
    std::invalid_argument("Floor count connot execed 4.");
  }
  return Appartement(id, nom, ville, TypeBatiment::Apartment,
                     EFFET_SATISFACTION_PER_FLOOR * floorsCount,
                     COST_PER_FLOOR * floorsCount,
                     CONSOMMATION_EAU_PER_FLOOR * floorsCount,
                     CONSOMMATION_ELE_PER_FLOOR * floorsCount,
                     POLUTION_PER_FLOOR * floorsCount, x, y, 1, 1,
                     MAX_HABITATS_PER_FLOOR * floorsCount, 0, floorsCount);
}
