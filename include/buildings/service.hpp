#ifndef SERVICE
#define SERVICE

#include "batiment.hpp"

using namespace std;

class Service : public Batiment {
protected:
  // Constructors
  Service(int id, const string &nom, Ville *ville, TypeBatiment type,
          int effectSatisfication, double cost, unsigned int Employees,
          unsigned int EmployeesNeeded, double consommationEau,
          double consommationElectricite, float polution, int x, int y,
          int largeur, int longeur);

  Service(int id, const string &nom, Ville *ville, TypeBatiment type,
          int effectSatisfication, double cost, unsigned int Employees,
          unsigned int EmployeesNeeded, Resources consommation, float polution,
          Position position, Surface surface);

  virtual void afficheDetails() const;

  unsigned int EmployeesNeeded;
  unsigned int Employees;
public:
        // Job accessors
        unsigned int getEmployees() const { return Employees; }
        unsigned int getEmployeesNeeded() const { return EmployeesNeeded; }
        void setEmployees(unsigned int count) { Employees = count; }
};

#endif // !SERVICE
