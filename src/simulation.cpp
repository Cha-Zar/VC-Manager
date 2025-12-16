#include "../include/cycle/simulation.hpp"
#include "../include/buildings/batiment.hpp"
#include "../include/evenement.hpp"
#include <iostream>

Simulation::Simulation(const string &nomVille, Difficulty difficulty)
    : ville(nomVille,
            (difficulty == Difficulty::Easy     ? 2000.0
             : difficulty == Difficulty::Medium ? 1000.0
                                                : 500.0),
            500, Resources(0.0, 0.0), BatimentList{}) {

  TimePerCycle = (difficulty == Difficulty::Easy     ? 120.0
                  : difficulty == Difficulty::Medium ? 60.0
                                                     : 30.0);
  this->difficulty = difficulty;
  cycleActuel = 0;
  
  // Initialize event system
  eventManager.initialiserEvenements();
  evenementActuel = nullptr;
  
  demarerCycle();
}

bool Simulation::canInteract() const { return state == SimState::Running; }

void Simulation::terminerCycle() {
  std::cout << "Terminer working\n";
  state = SimState::Evaluating;

  // Computing stats
  ville.collectProfit();
  ville.calculerPolutionTotale();
  ville.assignerEmplois();        // Distribute population to jobs
  ville.calculerSatisfactionTotale();
  ville.updatePopulation();
  cycleActuel++;

  // GAME OVER check
  if (ville.getPopulation() <= 0 && ville.getBudget() <= 0 &&
      ville.getSatisfaction() <= 0) {
    state = SimState::GameOver;
    return;
  }

  demarerCycle();
}

void Simulation::terminerCycleEarly() {
  std::cout << "Early working\n";
  if (state != SimState::Running)
    return;

  currentTime = TimePerCycle;

  terminerCycle();
}

void Simulation::demarerCycle() {
  state = SimState::Running;
  std::cout << "\n\n";
  std::cout << "CYCLE " << cycleActuel + 1 << " COMMENCE\n";
  std::cout << "\n" << std::endl;
  currentTime = 0;
  
  // Clear previous cycle's event
  evenementActuel = nullptr;
  
  // Try to trigger a random event
  declencherEvenement();
}

void Simulation::declencherEvenement() {
  // Attempt to generate a random event
  evenementActuel = eventManager.genererEvenementAleatoire(&ville);
  
  if (evenementActuel) {
    // Apply the event
    evenementActuel->appliquer(&ville);
  } else {
    std::cout << "Aucun événement ce cycle. Tout est calme." << std::endl;
  }
}

void Simulation::tick(float delta) {
  if (state != SimState::Running)
    return;

  currentTime += delta;

  if (currentTime >= TimePerCycle)
    terminerCycle();
}

// Getters
int Simulation::getCycle() const { return cycleActuel; }
float Simulation::getTimePerCycle() const { return TimePerCycle; }
float Simulation::getCurrentTime() const { return currentTime; }
SimState Simulation::getState() const { return state; }
const Ville &Simulation::getVille() const { return ville; }
Ville &Simulation::getVille() { return ville; }
const Evenement* Simulation::getEvenementActuel() const { 
  return evenementActuel.get(); 
}
