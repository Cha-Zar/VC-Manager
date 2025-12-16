#include "../../include/buildings/service.hpp"
#include <iostream>

using namespace std;

Service::Service(int id, const string &nom, Ville *ville, TypeBatiment type,
                 int effectSatisfication, double cost, unsigned int Employees,
                 unsigned int EmployeesNeeded, double consommationEau,
                 double consommationElectricite, float polution, int x, int y,
                 int largeur, int longeur)
    : Employees(Employees), EmployeesNeeded(EmployeesNeeded),
      Batiment(id, nom, ville, type, effectSatisfication, cost, consommationEau,
               consommationElectricite, polution, x, y, largeur, longeur) {}

Service::Service(int id, const string &nom, Ville *ville, TypeBatiment type,
                 int effectSatisfication, double cost, unsigned int Employees,
                 unsigned int EmployeesNeeded, Resources consommation,
                 float polution, Position position, Surface surface)
    : Employees(Employees), EmployeesNeeded(EmployeesNeeded),
      Batiment(id, nom, ville, type, effectSatisfication, cost, consommation,
               polution, position, surface) {}

// Methods
void Service::afficheDetails() const {
  Batiment::afficheDetails();
  std::cout << "Nomber of Employees :\t" << Employees << endl;
  std::cout << "Nomber of Employees Needed :\t" << EmployeesNeeded << endl;
}
