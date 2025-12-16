#ifndef BATIMENT
#define BATIMENT

#include "../utils.hpp"
#include "../ville/ville.hpp"
#include "../../tools/imgui/imgui.h"
#include "../../tools/imgui/imgui_impl_sdl2.h"
#include "../../tools/imgui/imgui_impl_sdlrenderer2.h"
#include <string>

using namespace std;

class Batiment {
public:
  TypeBatiment type;
  Surface surface;
  Position position;
  Ville *ville;

  // Variables  ACCESSIBLES pour les interactions
  static float POLLUTION_SATISFACTION_FACTOR;       // Comment la pollution affecte la satisfaction
  static float SATISFACTION_POPULATION_FACTOR;      // Comment la satisfaction affecte la population
  static float BUILDING_POLLUTION_FACTOR;          // Comment les bâtiments affectent la pollution
  static float POPULATION_BUILDING_FACTOR;         // Comment la population affecte les besoins en bâtiments
  
  // Constructors
  Batiment(int id, const string &nom, Ville *ville, TypeBatiment type,
           int effectSatisfication, double cost, double consommationEau,
           double consommationElectricite, float polution, int x, int y,
           int largeur, int longeur);

  Batiment(int id, const string &nom, Ville *ville, TypeBatiment type,
           int effectSatisfication, double cost, Resources consommation,
           float polution, Position position, Surface surface);
  virtual ~Batiment() = default;

  virtual void afficheDetails() const;
  virtual void impacterRessources();

  // Getters
  int getID();
  double getCost();
  int getSatisfaction();
  float getPolution();
  Resources getconsommation();
  string getNom() const;  // Getter for building name

  // Setters 
  static void setPollutionSatisfactionFactor(float factor);
  static void setSatisfactionPopulationFactor(float factor);
  static void setBuildingPollutionFactor(float factor);
  static void setPopulationBuildingFactor(float factor);

protected:
  int id;
  string nom;
  Resources consommation;
  float polution;
  int effectSatisfication;
  double cost;
};


#endif // !BATIMENT
