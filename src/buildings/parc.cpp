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
  std::cout << "effectBienEtre :\t" << effectBienEtre << endl;
}
