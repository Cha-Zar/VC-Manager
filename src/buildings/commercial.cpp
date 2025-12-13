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
void Comercial::setProfitPerEmployee(float value) { PROFIT_PER_EMPLOYEE = value; }
void Comercial::setSatisfactionBonus(float value) { SATISFACTION_BONUS = value; }
void Comercial::setPollutionPenalty(float value) { POLLUTION_PENALTY = value; }
void Comercial::setEmployeeEfficiency(float value) { EMPLOYEE_EFFICIENCY = value; }
void Comercial::setBaseEmployeesCinema(int value) { BASE_EMPLOYEES_CINEMA = value; }
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

Comercial Comercial::createCinema(int id, const string &nom, Ville *ville,
                                  int x, int y) {
  
  float baseProfit = PROFIT_PER_EMPLOYEE * BASE_EMPLOYEES_CINEMA;
  int satisfaction = static_cast<int>(100 * SATISFACTION_BONUS);
  float pollution = 2.5f * (1.0f + POLLUTION_PENALTY);
  
  return Comercial(id, nom, ville, TypeBatiment::Cinema, satisfaction, 500.0, 
                   0, BASE_EMPLOYEES_CINEMA, 10, 30, pollution, x, y, 2, 1, 
                   baseProfit);
}

Comercial Comercial::createMall(int id, const string &nom, Ville *ville, int x,
                                int y) {
  float baseProfit = PROFIT_PER_EMPLOYEE * BASE_EMPLOYEES_MALL * EMPLOYEE_EFFICIENCY;
  int satisfaction = static_cast<int>(100 * SATISFACTION_BONUS);
  float pollution = 8.0f * (1.0f + POLLUTION_PENALTY);
  
  return Comercial(id, nom, ville, TypeBatiment::Mall, satisfaction, 2000.0, 
                   0, BASE_EMPLOYEES_MALL, 400, 600, pollution, x, y, 3, 3, 
                   baseProfit);
}

Comercial Comercial::createBank(int id, const string &nom, Ville *ville, int x,
                                int y) {
  float baseProfit = PROFIT_PER_EMPLOYEE * BASE_EMPLOYEES_BANK * 2.5f; 
  float pollution = 2.0f * (1.0f + POLLUTION_PENALTY);
  
  return Comercial(id, nom, ville, TypeBatiment::Bank, -20, 2000.0, 
                   0, BASE_EMPLOYEES_BANK, 10, 30, pollution, x, y, 1, 1, 
                   baseProfit);
}

// Methods
void Comercial::afficheDetails() const {
  Service::afficheDetails();
  std::cout << "Profit :\t" << profit << endl;
  std::cout << "Profit par employe :\t" << PROFIT_PER_EMPLOYEE << endl;
  std::cout << "Bonus satisfaction :\t" << SATISFACTION_BONUS << endl;
}

// Getters
double Comercial::getProfit() { 
  // Calcul 
  float efficiency = (Employees >= EmployeesNeeded) ? 1.0f : EMPLOYEE_EFFICIENCY;
  return profit * efficiency * (1.0f - (ville->getPolution() * POLLUTION_PENALTY / 100.0f));
}
