#ifndef MAISON
#define MAISON

#include "resident.hpp"

class Appartement : public Resident {
private:
  Appartement(int id, const std::string &nom, Ville *ville, TypeBatiment type,
              int effectSatisfication, double cost, Resources consommation,
              float polution, Position position, Surface surface,
              int capaciteHabitants, int habitantsActuels,
              unsigned int floorsCount);

  Appartement(int id, const std::string &nom, Ville *ville, TypeBatiment type,
              int effectSatisfication, double cost, double consommationEau,
              double consommationElectricte, float polution, int x, int y,
              int largeur, int longeur, int capaciteHabitants,
              int habitantsActuels, unsigned int floorsCount);

  unsigned int floorsCount;

protected:
  static constexpr unsigned int MAX_FLOOR_COUNT = 4;
  static constexpr int MAX_HABITATS_PER_FLOOR = 2;
  static constexpr double CONSOMMATION_EAU_PER_FLOOR = 10.0;
  static constexpr double CONSOMMATION_ELE_PER_FLOOR = 10.0;
  static constexpr double COST_PER_FLOOR = 15.0;
  static constexpr float POLUTION_PER_FLOOR = 2.0;
  static constexpr int EFFET_SATISFACTION_PER_FLOOR = 1.0;

public:
  static Appartement createAppartement(int id, const string &nom, Ville *ville,
                                       unsigned int floorNumbers, int x, int y);
  void addNewFloor();
  void destroyFloor();

  // Getters
  unsigned int getFloorsCount();
};

#endif // !MAISON
