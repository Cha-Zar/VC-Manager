#include "../include/cycle/simulation.hpp"
#include "../include/buildings/batiment.hpp"
#include <iostream>

Simulation::Simulation(const string &nomVille, Difficulty difficulty)
    : ville(nomVille,
            (difficulty == Difficulty::Easy     ? 2000.0
             : difficulty == Difficulty::Medium ? 1000.0
                                                : 500.0),
            0, Resources(0.0, 0.0), BatimentList{}) {

  TimePerCycle = (difficulty == Difficulty::Easy     ? 120.0
                  : difficulty == Difficulty::Medium ? 60.0
                                                     : 30.0);
  this->difficulty = difficulty;
  cycleActuel = 0;
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
  std::cout << "demarer working\n";
  currentTime = 0;
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
