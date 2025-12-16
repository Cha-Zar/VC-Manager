#ifndef COMERCIAL
#define COMERCIAL

#include "service.hpp"

using namespace std;

class Comercial : public Service {
protected:
  // Constructors
  Comercial(int id, const string &nom, Ville *ville, TypeBatiment type,
            int effectSatisfication, double cost, unsigned int employees,
            unsigned int employeesNeeded, double consommationEau,
            double consommationElectricite, float polution, int x, int y,
            int largeur, int longeur, double profit);

  Comercial(int id, const string &nom, Ville *ville, TypeBatiment type,
            int effectSatisfication, double cost, unsigned int employees,
            unsigned int employeesNeeded, Resources consommation,
            float polution, Position position, Surface surface, double profit);

public:
  static float PROFIT_PER_EMPLOYEE;
  static float SATISFACTION_BONUS;
  static float POLLUTION_PENALTY;
  static float EMPLOYEE_EFFICIENCY;
  static int BASE_EMPLOYEES_CINEMA;
  static int BASE_EMPLOYEES_MALL;
  static int BASE_EMPLOYEES_BANK;

  // Setters
  static void setProfitPerEmployee(float value);
  static void setSatisfactionBonus(float value);
  static void setPollutionPenalty(float value);
  static void setEmployeeEfficiency(float value);
  static void setBaseEmployeesCinema(int value);
  static void setBaseEmployeesMall(int value);
  static void setBaseEmployeesBank(int value);

  // Methods
  void afficheDetails() const override;
  static BatPtr createCinema(Ville *ville, int x, int y);
  static BatPtr createMall(Ville *ville, int x, int y);
  static BatPtr createBank(Ville *ville, int x, int y);

  // Getters
  double getProfit();

private:
  double profit;
};

#endif // !COMERCIAL
