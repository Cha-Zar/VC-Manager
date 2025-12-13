#include "../include/ville/ville.hpp"
#include "../include/buildings/batiment.hpp"
#include "../include/buildings/commercial.hpp"
#include "../include/buildings/resident.hpp"
#include "../include/buildings/service.hpp"
#include "../include/utils.hpp"
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

// Constructor
Ville::Ville(const string &nom, double budget, unsigned int population,
             Resources resources, BatimentList batiments)
    : nom(nom), budget(budget), population(population), satisfaction(50),
      polution(0), resources(resources), batiments(std::move(batiments)) {}

Ville::~Ville() = default;

// List de batiments
void Ville::ajoutBatiment(BatPtr batiment) {
  batiments.push_back(std::move(batiment));
  budget -= batiment->getCost();
}

void Ville::supprimerBatiment(int id) {
  for (auto it = batiments.begin(); it != batiments.end(); ++it) {
    if ((*it)->getID() == id) {
      budget += (*it)->getCost();
      batiments.erase(it);
      return;
    }
  }
}

//pollution calculations
float Ville::calculerPolutionTotale() {
  float pollutionTotale = 0.0f;
  
  // Buildings generate pollution based on type
  for (auto& batiment : batiments) {
    // Power plants are major polluters
    if (batiment->type == TypeBatiment::PowerPlant) {
      pollutionTotale += 15.0f; // Significant pollution
    }
    // Commercial-like buildings (mall, cinema, bank) cause moderate pollution
    else if (batiment->type == TypeBatiment::Mall ||
             batiment->type == TypeBatiment::Cinema ||
             batiment->type == TypeBatiment::Bank) {
      pollutionTotale += 5.0f; // Moderate pollution
    }
    else if (batiment->type == TypeBatiment::House ||
             batiment->type == TypeBatiment::Apartment) {
      // Residential buildings generate minimal pollution themselves
      pollutionTotale += 2.0f;
    }
    
    // Occupied buildings generate more pollution
    if (batiment->type == TypeBatiment::House ||
        batiment->type == TypeBatiment::Apartment) {
      Resident *r = dynamic_cast<Resident *>(batiment.get());
      if (r) {
        // More residents = more waste
        float occupancyRatio = static_cast<float>(r->gethabitantsActuels()) / 
                              static_cast<float>(r->getcapaciteHabitants() + 1);
        pollutionTotale += occupancyRatio * 3.0f;
      }
    }
  }
  
  // Population contributes to pollution (traffic, waste, etc.)
  float populationPollution = (population / 100.0f) * 0.5f;
  pollutionTotale += populationPollution;
  
  // Apply natural decrease (environment cleanses itself slightly)
  pollutionTotale *= 0.95f;
  
  // Cap at 100%
  pollutionTotale = std::min(pollutionTotale, 100.0f);
  pollutionTotale = std::max(pollutionTotale, 0.0f);
  
  setPolution(pollutionTotale);
  return pollutionTotale;
}

//Satisfaction Calculations
int Ville::calculerSatisfactionTotale() {
  float satisfactionScore = 50.0f; // Base satisfaction
  
  //positive factors

  // Services & Amenities from buildings
  for (auto& batiment : batiments) {
    if (batiment->type == TypeBatiment::Park) {
      satisfactionScore += 8.0f; // Parks improve happiness
    } else if (batiment->type == TypeBatiment::Cinema ||
               batiment->type == TypeBatiment::Mall ||
               batiment->type == TypeBatiment::Bank) {
      satisfactionScore += 5.0f; // Commerce/leisure provides enjoyment
    }
  }
  
  // Population capacity satisfaction
  int populationActuelle = calculerPopulationTotale();
  int capaciteTotale = calculerCapacitePopulation();
  
  if (capaciteTotale > 0) {
    float housingRatio = static_cast<float>(populationActuelle) / 
                        static_cast<float>(capaciteTotale);
    
    if (housingRatio < 0.5f) {
      satisfactionScore += 5.0f; // Plenty of space
    }
    else if (housingRatio > 0.9f) {
      satisfactionScore -= 15.0f; // Overcrowding
    }
  }
  
  // negative factors

  // Unemployment reduces satisfaction (linear penalty: 100% unemployment => -20 satisfaction)
  float unemploymentRate = calculerTauxChomage();
  float unemploymentPenalty = unemploymentRate * 0.2f;
  satisfactionScore -= unemploymentPenalty;

  // Pollution heavily impacts satisfaction (quadratic effect for realism)
  float pollutionFactor = (polution / 100.0f);
  float pollutionPenalty = pollutionFactor * pollutionFactor * 50.0f;
  satisfactionScore -= pollutionPenalty;
  
  // Clamp to 0-100 range
  satisfactionScore = std::max(0.0f, std::min(100.0f, satisfactionScore));
  
  int finalSatisfaction = static_cast<int>(satisfactionScore);
  setSatisfaction(finalSatisfaction);
  return finalSatisfaction;
}

// calculations
Resources Ville::calculerconsommationTotale() {
  Resources ConsumationTotale;
  for (auto it = batiments.begin(); it != batiments.end(); ++it) {
    ConsumationTotale += (*it)->getconsommation();
  }
  return ConsumationTotale;
}

Resources Ville::calculerResourcesTotale() {
  Resources ResourcesTotale;
  for (auto it = batiments.begin(); it != batiments.end(); ++it) {
    if ((*it)->type == TypeBatiment::PowerPlant ||
        (*it)->type == TypeBatiment::WaterTreatmentPlant) {
      ResourcesTotale += (*it)->getconsommation();
    }
  }
  setResources(resources);
  return ResourcesTotale;
}

int Ville::calculerPopulationTotale() {
  int populationTotale = 0;
  for (auto it = batiments.begin(); it != batiments.end(); ++it) {
    if ((*it)->type == TypeBatiment::House ||
        (*it)->type == TypeBatiment::Apartment) {
      Resident *r = dynamic_cast<Resident *>(it->get());
      if (r)
        populationTotale += r->gethabitantsActuels();
    }
  }
  return populationTotale;
}

int Ville::calculerCapacitePopulation() {
  int capaciteTotale = 0;
  for (auto it = batiments.begin(); it != batiments.end(); ++it) {
    if ((*it)->type == TypeBatiment::House ||
        (*it)->type == TypeBatiment::Apartment) {
      Resident *r = dynamic_cast<Resident *>(it->get());
      if (r)
        capaciteTotale += r->getcapaciteHabitants();
    }
  }
  return capaciteTotale;
}

double Ville::calculerProfit() {
  double profitTotale = 0.0;
  for (auto it = batiments.begin(); it != batiments.end(); ++it) {
    Comercial *c = dynamic_cast<Comercial *>(it->get());
    if (c)
      profitTotale += c->getProfit();
  }
  return profitTotale;
}

void Ville::collectProfit() { budget += calculerProfit(); }

//population update
void Ville::updatePopulation() {
  int capaciteTotale = calculerCapacitePopulation();
  int populationActuelle = calculerPopulationTotale();

  if (capaciteTotale == 0) {
    // No housing: apply a small decline to avoid runaway numbers
    int decline = static_cast<int>(std::round(populationActuelle * 0.05f));
    int nouvellePopulation = std::max(0, populationActuelle - decline);
    // remove inhabitants
    int difference = nouvellePopulation - populationActuelle;
    if (difference < 0) {
      int toRemove = -difference;
      for (auto it = batiments.begin(); it != batiments.end() && toRemove > 0; ++it) {
        if ((*it)->type == TypeBatiment::House || (*it)->type == TypeBatiment::Apartment) {
          Resident *r = dynamic_cast<Resident *>(it->get());
          if (r && r->gethabitantsActuels() > 0) {
            int remove = std::min(r->gethabitantsActuels(), toRemove);
            r->retirerHabitants(remove);
            toRemove -= remove;
          }
        }
      }
    }
    setPopulation(nouvellePopulation);
    return;
  }

  // Compute modifiers
  float satisfactionRatio = static_cast<float>(satisfaction) / 100.0f; // 0..1
  float satisfactionEffect = (static_cast<float>(satisfaction) - 50.0f) / 50.0f; // -1..1
  float pollutionRatio = polution / 100.0f; // 0..1
  float unemploymentRatio = calculerTauxChomage() / 100.0f; // 0..1
  float density = static_cast<float>(populationActuelle) / static_cast<float>(capaciteTotale); // 0..inf
  float overcrowding = std::max(0.0f, density - 1.0f);

  // Base growth rate per cycle (conservative)
  const float baseRate = 0.005f; // 0.5% per cycle baseline

  // Compose growth rate from factors (small balanced contributions)
  float growthRate = baseRate
      + (satisfactionEffect * 0.02f)      // satisfaction influence +/-2%
      - (unemploymentRatio * 0.02f)      // unemployment reduces growth up to -2%
      - (pollutionRatio * 0.03f)         // pollution reduces growth up to -3%
      - (overcrowding * 0.05f);          // overcrowding penalizes growth

  // Clamp growth rate to reasonable bounds to avoid explosions
  growthRate = std::max(-0.2f, std::min(0.2f, growthRate)); // [-20%, +20%]

  // Compute migration due to job vacancies (small immediate inflow)
  unsigned int totalJobs = calculerCapaciteEmploi();
  unsigned int employed = calculerEmploiActuel();
  int vacancies = static_cast<int>(totalJobs > employed ? totalJobs - employed : 0);
  int housingSpace = capaciteTotale - populationActuelle;
  int migrants = 0;
  if (vacancies > 0 && housingSpace > 0) {
    int potential = std::min(vacancies, housingSpace);
    migrants = static_cast<int>(std::round(potential * 0.05f)); // 5% of available positions
  }

  // Apply growth
  int delta = static_cast<int>(std::round(populationActuelle * growthRate)) + migrants;
  int nouvellePopulation = populationActuelle + delta;

  // Cap at housing capacity
  if (nouvellePopulation > capaciteTotale)
    nouvellePopulation = capaciteTotale;
  if (nouvellePopulation < 0)
    nouvellePopulation = 0;

  // Distribute population into resident buildings (same logic as before)
  int difference = nouvellePopulation - populationActuelle;

  if (difference > 0) {
    for (auto it = batiments.begin(); it != batiments.end() && difference > 0; ++it) {
      if ((*it)->type == TypeBatiment::House || (*it)->type == TypeBatiment::Apartment) {
        Resident *r = dynamic_cast<Resident *>(it->get());
        if (r && r->gethabitantsActuels() < r->getcapaciteHabitants()) {
          int spacesAvailable = r->getcapaciteHabitants() - r->gethabitantsActuels();
          int toAdd = std::min(spacesAvailable, difference);
          r->ajouterHabitants(toAdd);
          difference -= toAdd;
        }
      }
    }
  } else if (difference < 0) {
    difference = -difference;
    for (auto it = batiments.begin(); it != batiments.end() && difference > 0; ++it) {
      if ((*it)->type == TypeBatiment::House || (*it)->type == TypeBatiment::Apartment) {
        Resident *r = dynamic_cast<Resident *>(it->get());
        if (r && r->gethabitantsActuels() > 0) {
          int habitants = r->gethabitantsActuels();
          int toRemove = std::min(habitants, difference);
          r->retirerHabitants(toRemove);
          difference -= toRemove;
        }
      }
    }
  }

  setPopulation(nouvellePopulation);
}

// Getters
string Ville::getNom() const { return nom; }
double Ville::getBudget() const { return budget; }
float Ville::getPolution() const { return polution; }
unsigned int Ville::getPopulation() const { return population; }  
int Ville::getSatisfaction() const { return satisfaction; }
Resources Ville::getResources() const { return resources; }

// Setters
void Ville::setBudget(double newBudget) { budget = newBudget; }
void Ville::setPopulation(unsigned int newPopulation) {
  population = newPopulation;
}
void Ville::setSatisfaction(int newSatisfaction) {
  satisfaction = std::max(0, std::min(100, newSatisfaction));
}
void Ville::setPolution(float newPolution) { 
  polution = std::max(0.0f, std::min(100.0f, newPolution)); 
}
void Ville::setResources(Resources newResources) { resources = newResources; }

// employment calculations
unsigned int Ville::calculerCapaciteEmploi() const {
  unsigned int totalJobs = 0;
  for (const auto &batiment : batiments) {
    const Service *s = dynamic_cast<const Service *>(batiment.get());
    if (s) {
      // Only count job slots from buildings that actually hire
      if (batiment->type == TypeBatiment::Cinema || 
          batiment->type == TypeBatiment::Mall ||
          batiment->type == TypeBatiment::Bank ||
          batiment->type == TypeBatiment::PowerPlant ||
          batiment->type == TypeBatiment::WaterTreatmentPlant) {
        totalJobs += s->getEmployeesNeeded();
      }
    }
  }
  return totalJobs;
}

unsigned int Ville::calculerEmploiActuel() const {
  unsigned int employed = 0;
  for (const auto &batiment : batiments) {
    const Service *s = dynamic_cast<const Service *>(batiment.get());
    if (s) {
      employed += s->getEmployees();
    }
  }
  return employed;
}

float Ville::calculerTauxChomage() const {
  unsigned int pop = static_cast<unsigned int>(calculerPopulationTotale());
  if (pop == 0)
    return 0.0f;
  unsigned int employed = calculerEmploiActuel();
  unsigned int unemployed = (pop > employed ? pop - employed : 0);
  return (unemployed / static_cast<float>(pop)) * 100.0f;
}

// job assignment
void Ville::assignerEmplois() {
  unsigned int remainingPopulation = static_cast<unsigned int>(calculerPopulationTotale());
  
  // Fill jobs only in Commercial and Infrastructure buildings that employ people
  for (auto &batiment : batiments) {
    Service *s = dynamic_cast<Service *>(batiment.get());
    if (s && remainingPopulation > 0) {
      // Only assign jobs to buildings that actually employ people
      // Commercial buildings (Comercial) and Infrastructure have employees
      // Parks (Parc) do NOT have employees
      if (batiment->type == TypeBatiment::Cinema || 
          batiment->type == TypeBatiment::Mall ||
          batiment->type == TypeBatiment::Bank ||
          batiment->type == TypeBatiment::PowerPlant ||
          batiment->type == TypeBatiment::WaterTreatmentPlant) {
        
        unsigned int capacity = s->getEmployeesNeeded();
        unsigned int toAssign = std::min(remainingPopulation, capacity);
        s->setEmployees(toAssign);
        remainingPopulation -= toAssign;
      } else {
        // Parks and other services don't employ people
        s->setEmployees(0);
      }
    } else if (s) {
      s->setEmployees(0);
    }
  }
}

// employment status display
void Ville::afficherStatutEmploi() const {
  std::cout << "\n EMPLOYMENT STATUS \n";
  std::cout << "Total Jobs: " << calculerCapaciteEmploi() << " | Employed: " 
            << calculerEmploiActuel() << " | Unemployment: " 
            << calculerTauxChomage() << "%\n\n";
  
  for (const auto &batiment : batiments) {
    const Service *s = dynamic_cast<const Service *>(batiment.get());
    if (s && (batiment->type == TypeBatiment::Cinema || 
              batiment->type == TypeBatiment::Mall ||
              batiment->type == TypeBatiment::Bank ||
              batiment->type == TypeBatiment::PowerPlant ||
              batiment->type == TypeBatiment::WaterTreatmentPlant)) {
      
      unsigned int employed = s->getEmployees();
      unsigned int capacity = s->getEmployeesNeeded();
      unsigned int empty = capacity - employed;
      
      std::cout << batiment->getNom() << ": " << employed << " / " << capacity 
                << " employed (" << empty << " empty slots)\n";
    }
  }
  std::cout << "\n\n";
}
