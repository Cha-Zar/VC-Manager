#ifndef SIMULATION
#define SIMULATION

#include "../utils.hpp"
#include "../ville/ville.hpp"
#include "../evenement.hpp"
#include <memory>

using namespace std;

class Simulation {
public:
  // Constructor
  Simulation(const string &nomVille, Difficulty difficulty);

  // Methods
  void demarerCycle();
  void terminerCycle();
  void terminerCycleEarly();
  void declencherEvenement();
  void tick(float delta);
  bool canInteract() const;

  // Getters
  int getCycle() const;
  float getTimePerCycle() const;
  SimState getState() const;
  float getCurrentTime() const;
  const Ville& getVille() const;
  Ville& getVille();
  const Evenement* getEvenementActuel() const;

private:
  Ville ville;
  unsigned int cycleActuel;
  Difficulty difficulty;
  float TimePerCycle;
  float currentTime;
  SimState state;
  
  // Event system
  EventManager eventManager;
  std::unique_ptr<Evenement> evenementActuel;
};

#endif // !SIMULATION
