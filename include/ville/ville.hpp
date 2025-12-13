#ifndef VILLE
#define VILLE

#include "../utils.hpp"
#include <string>

using namespace std;

class Batiment;

class Ville {
public:
  Ville(const string &nom, double budget, unsigned int population,
        Resources resources, BatimentList batiments);
  ~Ville();

  Ville(const Ville &) = delete;
  Ville &operator=(const Ville &) = delete;

  // Enable move
  Ville(Ville &&) = default;
  Ville &operator=(Ville &&) = default;

  void ajoutBatiment(BatPtr batiment);
  void supprimerBatiment(int id);
  Resources calculerconsommationTotale();
  Resources calculerResourcesTotale();
  float calculerPolutionTotale();
  int calculerSatisfactionTotale();
  // Employment
  unsigned int calculerCapaciteEmploi();
  unsigned int calculerEmploiActuel();
  float calculerTauxChomage();
  void assignerEmplois(); // Distribute population to jobs
  void afficherStatutEmploi() const; // Display job status per building
  int calculerPopulationTotale();
  int calculerCapacitePopulation();
  double calculerProfit();
  void collectProfit();
  void updatePopulation();

  // Getters
  string getNom() const;
  double getBudget() const;
  unsigned int getPopulation() const;
  int getSatisfaction() const;
  float getPolution() const;
  Resources getResources() const;

  // Setters
  void setBudget(double newBudget);
  void setPopulation(unsigned int newPopulation);
  void setSatisfaction(int newSatisfaction);
  void setPolution(float newPolution);
  void setResources(Resources newResources);

private:
  string nom;
  double budget;
  unsigned int population;
  int satisfaction;
  float polution;
  Resources resources;
  BatimentList batiments;
  BatimentList newBatiments;
};

#endif // !VILLE
