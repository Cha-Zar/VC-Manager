#ifndef RESIDENT
#define RESIDENT

#include "batiment.hpp"

using namespace std;

class Resident : public Batiment {
protected:
  // Constructors
  Resident(int id, const string &nom, Ville *ville, TypeBatiment type,
           int effectSatisfication, double cost, double consommationEau,
           double consommationElectricite, float polution, int x, int y,
           int largeur, int longeur, int capaciteHabitants,
           int habitantsActuels);

  Resident(int id, const string &nom, Ville *ville, TypeBatiment type,
           int effectSatisfication, double cost, Resources consommation,
           float polution, Position position, Surface surface,
           int capaciteHabitants, int habitantsActuels);

  // Variables ACCESSIBLES
public:
  static float WATER_PER_PERSON;
  static float ELECTRICITY_PER_PERSON;
  static float SATISFACTION_PER_PERSON;
  static float POLLUTION_PER_PERSON;
  static int BASE_CAPACITY_HOUSE;
  static int BASE_CAPACITY_APARTMENT;
  
  // Méthodes statiques 
  static void setWaterPerPerson(float value);
  static void setElectricityPerPerson(float value);
  static void setSatisfactionPerPerson(float value);
  static void setPollutionPerPerson(float value);
  static void setBaseCapacityHouse(int value);
  static void setBaseCapacityApartment(int value);

protected:
  int capaciteHabitants;
  int habitantsActuels;

public:
  // Methods
  void afficheDetails() const override;
  void ajouterHabitants(int nombreHabitants);
  void retirerHabitants(int nombreHabitants);

  static Resident createHouse(int id, const string &nom, Ville *ville, int x,
                              int y);

  // Getters
  int gethabitantsActuels();
  int getcapaciteHabitants();
};

#endif // !MAISON
