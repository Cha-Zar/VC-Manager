#include "../../include/buildings/infrastructure.hpp"

#include <string>

using namespace std;

Infrastructure::Infrastructure(int id, const string &nom, Ville *ville,
                               TypeBatiment type, int effectSatisfication,
                               double cost, unsigned int employees,
                               unsigned int employeesNeeded,
                               double consommationEau,
                               double consommationElectricite, double pollution,
                               int x, int y, int largeur, int longeur,
                               Resources productionRessources)
    : productionRessources(productionRessources),
      Service(id, nom, ville, type, effectSatisfication, cost, employees,
              employeesNeeded, consommationEau, consommationElectricite,
              polution, x, y, largeur, longeur) {}

Infrastructure::Infrastructure(int id, const string &nom, Ville *ville,
                               TypeBatiment type, int effectSatisfication,
                               double cost, unsigned int employees,
                               unsigned int employeesNeeded,
                               double consommationEau,
                               double consommationElectricite, double pollution,
                               Position position, Surface surface,
                               Resources productionRessources)
    : productionRessources(productionRessources),
      Service(id, nom, ville, type, effectSatisfication, cost, employees,
              employeesNeeded, consommation, polution, position, surface) {}

Infrastructure Infrastructure::createPowerPlant(int id, const string &nom,
                                                Ville *ville, int x, int y) {
  // Auto-generate name and ID
  string generatedName = NameGenerator::getRandomName(TypeBatiment::PowerPlant);
  Position position(x, y);
  Surface surface(1, 1);
  int generatedID = BuildingIDGenerator::generateID(generatedName, TypeBatiment::PowerPlant, position, surface);
  
  return Infrastructure(generatedID, generatedName, ville, TypeBatiment::PowerPlant, -2, 20.0, 0,
                        40, 3.0, 0.0, 20, x, y, 1, 1, Resources(0.0, 200.0));
}

Infrastructure Infrastructure::createWaterTreatmentPlant(int id,
                                                         const string &nom,
                                                         Ville *ville, int x,
                                                         int y) {
  // Auto-generate name and ID
  string generatedName = NameGenerator::getRandomName(TypeBatiment::WaterTreatmentPlant);
  Position position(x, y);
  Surface surface(1, 1);
  int generatedID = BuildingIDGenerator::generateID(generatedName, TypeBatiment::WaterTreatmentPlant, position, surface);
  
  return Infrastructure(generatedID, generatedName, ville, TypeBatiment::WaterTreatmentPlant, -2,
                        20.0, 0, 40, 0.0, 7.0, 15, x, y, 1, 1,
                        Resources(200.0, 0.0));
}

Infrastructure Infrastructure::createUtilityPlant(int id, const string &nom,
                                                  Ville *ville, int x, int y) {
  // Auto-generate name and ID
  string generatedName = NameGenerator::getRandomName(TypeBatiment::UtilityPlant);
  Position position(x, y);
  Surface surface(1, 1);
  int generatedID = BuildingIDGenerator::generateID(generatedName, TypeBatiment::UtilityPlant, position, surface);
  
  return Infrastructure(generatedID, generatedName, ville, TypeBatiment::UtilityPlant, -6, 60.0, 0,
                        45, 0.0, 0.0, 45, x, y, 1, 1, Resources(150.0, 150.0));
}

void Infrastructure::impacterRessources() {
  ville->setResources(ville->getResources() - consommation +
                      productionRessources);
}

void Infrastructure::produireRessources() {
  ville->setResources(ville->getResources() + productionRessources);
}
