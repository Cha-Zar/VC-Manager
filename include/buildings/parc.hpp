#ifndef PARC
#define PARC

#include "../utils.hpp"
#include "service.hpp"

using namespace std;

class Parc : public Service {
protected:
  // Constructors
  Parc(int id, const string &nom, Ville *ville, TypeBatiment type,
       int effectSatisfication, double cost, unsigned int Employees,
       unsigned int EmployeesNeeded, Resources consommation, float polution,
       Position position, Surface surface);

  Parc(int id, const string &nom, Ville *ville, TypeBatiment type,
       int effectSatisfication, double cost, unsigned int Employees,
       unsigned int EmployeesNeeded, double consommationEau,
       double consommationElectricite, float polution, int x, int y,
       int largeur, int longeur);

public:
  // Methods
  void afficheDetails() const override;
  void ameliorerBienEtre();
  void diminuerPollution();

private:
  static constexpr int POLLUTION_REDUCTION_FACTOR = 10;
  int effectBienEtre;
};

#endif // !PARC
