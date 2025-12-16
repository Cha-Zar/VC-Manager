#include "../../include/buildings/commercial.hpp"

#include <iostream>
#include <string>

using namespace std;

// Initialisation
float Comercial::PROFIT_PER_EMPLOYEE = 2.0f;
float Comercial::SATISFACTION_BONUS = 1.0f;
float Comercial::POLLUTION_PENALTY = 0.5f;
float Comercial::EMPLOYEE_EFFICIENCY = 0.8f;
int Comercial::BASE_EMPLOYEES_CINEMA = 5;
int Comercial::BASE_EMPLOYEES_MALL = 50;
int Comercial::BASE_EMPLOYEES_BANK = 10;

// Setters
void Comercial::setProfitPerEmployee(float value) {
  PROFIT_PER_EMPLOYEE = value;
}
void Comercial::setSatisfactionBonus(float value) {
  SATISFACTION_BONUS = value;
}
void Comercial::setPollutionPenalty(float value) { POLLUTION_PENALTY = value; }
void Comercial::setEmployeeEfficiency(float value) {
  EMPLOYEE_EFFICIENCY = value;
}
void Comercial::setBaseEmployeesCinema(int value) {
  BASE_EMPLOYEES_CINEMA = value;
}
void Comercial::setBaseEmployeesMall(int value) { BASE_EMPLOYEES_MALL = value; }
void Comercial::setBaseEmployeesBank(int value) { BASE_EMPLOYEES_BANK = value; }

Comercial::Comercial(int id, const string &nom, Ville *ville, TypeBatiment type,
                     int effectSatisfication, double cost,
                     unsigned int Employees, unsigned int EmployeesNeeded,
                     double consommationEau, double consommationElectricite,
                     float polution, int x, int y, int largeur, int longeur,
                     double profit)
    : profit(profit),
      Service(id, nom, ville, type, effectSatisfication, cost, Employees,
              EmployeesNeeded, consommationEau, consommationElectricite,
              polution, x, y, largeur, longeur) {}

Comercial::Comercial(int id, const string &nom, Ville *ville, TypeBatiment type,
                     int effectSatisfication, double cost,
                     unsigned int Employees, unsigned int EmployeesNeeded,
                     Resources consommation, float polution, Position position,
                     Surface surface, double profit)
    : profit(profit),
      Service(id, nom, ville, type, effectSatisfication, cost, Employees,
              EmployeesNeeded, consommation, polution, position, surface) {}

BatPtr Comercial::createCinema(Ville *ville, int x, int y) {
  // Auto-generate name and ID
  string generatedName = NameGenerator::getRandomName(TypeBatiment::Cinema);
  Position position(x, y);
  Surface surface(2, 1);
  int generatedID = BuildingIDGenerator::generateID(
      generatedName, TypeBatiment::Cinema, position, surface);

  float baseProfit = PROFIT_PER_EMPLOYEE * BASE_EMPLOYEES_CINEMA;
  int satisfaction = static_cast<int>(100 * SATISFACTION_BONUS);
  float pollution = 2.5f * (1.0f + POLLUTION_PENALTY);

  return BatPtr(new Comercial(generatedID, generatedName, ville,
                              TypeBatiment::Cinema, satisfaction, 500.0, 0,
                              BASE_EMPLOYEES_CINEMA, 10, 30, pollution, x, y, 2,
                              1, baseProfit));
}

BatPtr Comercial::createMall(Ville *ville, int x, int y) {
  // Auto-generate name and ID
  string generatedName = NameGenerator::getRandomName(TypeBatiment::Mall);
  Position position(x, y);
  Surface surface(3, 3);
  int generatedID = BuildingIDGenerator::generateID(
      generatedName, TypeBatiment::Mall, position, surface);

  float baseProfit =
      PROFIT_PER_EMPLOYEE * BASE_EMPLOYEES_MALL * EMPLOYEE_EFFICIENCY;
  int satisfaction = static_cast<int>(100 * SATISFACTION_BONUS);
  float pollution = 8.0f * (1.0f + POLLUTION_PENALTY);

  return BatPtr(new Comercial(generatedID, generatedName, ville,
                              TypeBatiment::Mall, satisfaction, 2000.0, 0,
                              BASE_EMPLOYEES_MALL, 400, 600, pollution, x, y, 3,
                              3, baseProfit));
}

BatPtr Comercial::createBank(Ville *ville, int x, int y) {
  // Auto-generate name and ID
  string generatedName = NameGenerator::getRandomName(TypeBatiment::Bank);
  Position position(x, y);
  Surface surface(1, 1);
  int generatedID = BuildingIDGenerator::generateID(
      generatedName, TypeBatiment::Bank, position, surface);

  float baseProfit = PROFIT_PER_EMPLOYEE * BASE_EMPLOYEES_BANK * 2.5f;
  float pollution = 2.0f * (1.0f + POLLUTION_PENALTY);

  return BatPtr(new Comercial(
      generatedID, generatedName, ville, TypeBatiment::Bank, -20, 2000.0, 0,
      BASE_EMPLOYEES_BANK, 10, 30, pollution, x, y, 1, 1, baseProfit));
}

// Methods
void Comercial::afficheDetails() const {
  Service::afficheDetails();
  if (ImGui::CollapsingHeader("Commercial Info", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::Text("Profit %.2f", profit);
    ImGui::Text("Profit par employe %.2f", PROFIT_PER_EMPLOYEE);
    ImGui::Text("Bonus satisfaction %.2f", SATISFACTION_BONUS);
  }
}

// Getters
double Comercial::getProfit() {
  // Calcul
  float efficiency =
      (Employees >= EmployeesNeeded) ? 1.0f : EMPLOYEE_EFFICIENCY;
  return profit * efficiency *
         (1.0f - (ville->getPolution() * POLLUTION_PENALTY / 100.0f));
}
