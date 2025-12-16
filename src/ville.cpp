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
    // Immediately reassign jobs to include new building's employees
    assignerEmplois();
}


void Ville::supprimerBatiment(int x , int y) {
  for (auto it = batiments.begin(); it != batiments.end(); ++it) {
    Position pos = (*it)->position;
    if (pos.x == x && pos.y == y) {
      budget += (*it)->getCost();
      batiments.erase(it);
      // Immediately reassign jobs to remaining buildings
      assignerEmplois();
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
  // Use city population to determine if we should compute satisfaction
  unsigned int popVille = getPopulation();
  if (popVille == 0) {
    setSatisfaction(0);
    return 0;
  }
  // Occupancy and housing pressure are based on actual residents in buildings
  int populationActuelle = calculerPopulationTotale();

  float satisfactionScore = 50.0f; // Base satisfaction when city is active
  
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

int Ville::calculerPopulationTotale() const {
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

int Ville::calculerCapacitePopulation() const {
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
  // Distinguish between city population and actual occupants in buildings
  int popDansBatiments = calculerPopulationTotale();
  int popVille = static_cast<int>(getPopulation());

  // If current population exceeds capacity, force it down to capacity immediately
  if (popVille > capaciteTotale) {
    popVille = capaciteTotale;
    setPopulation(capaciteTotale);
    // Remove excess inhabitants from buildings
    int toRemove = popDansBatiments - capaciteTotale;
    if (toRemove > 0) {
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
    // If capacity is 0, we're done - population is now 0
    if (capaciteTotale == 0) {
      return;
    }
  }

  // Compute modifiers based on current city state
  float satisfactionRatio = static_cast<float>(satisfaction) / 100.0f; // 0..1
  float satisfactionEffect = (static_cast<float>(satisfaction) - 50.0f) / 50.0f; // -1..1
  float pollutionRatio = polution / 100.0f; // 0..1
  float unemploymentRatio = calculerTauxChomage() / 100.0f; // 0..1
  float density = static_cast<float>(popVille) / static_cast<float>(capaciteTotale); // 0..inf
  float overcrowding = std::max(0.0f, density - 1.0f);

  // Base growth rate per cycle (conservative)
  const float baseRate = 0.005f; // 0.5% per cycle baseline

  // Compose growth rate from factors (small balanced contributions)
  float growthRate = baseRate
      + (satisfactionEffect * 0.02f)      // satisfaction influence +/-2%
      - (unemploymentRatio * 0.02f)       // unemployment reduces growth up to -2%
      - (pollutionRatio * 0.03f)          // pollution reduces growth up to -3%
      - (overcrowding * 0.05f);           // overcrowding penalizes growth

  // Clamp growth rate to reasonable bounds to avoid explosions
  growthRate = std::max(-0.2f, std::min(0.2f, growthRate)); // [-20%, +20%]

  // Compute migration due to job vacancies (small immediate inflow)
  unsigned int totalJobs = calculerCapaciteEmploi();
  unsigned int employed = calculerEmploiActuel();
  int vacancies = static_cast<int>(totalJobs > employed ? totalJobs - employed : 0);
  int housingSpace = capaciteTotale - popVille;
  int migrants = 0;
  if (vacancies > 0 && housingSpace > 0) {
    int potential = std::min(vacancies, housingSpace);
    migrants = static_cast<int>(std::round(potential * 0.05f)); // 5% of available positions
  }

  // Apply growth to the city population
  int delta = static_cast<int>(std::round(popVille * growthRate)) + migrants;
  
  // When satisfaction is very low, enforce minimum percentage decrease
  // (not just -1, but scales with actual population)
  if (satisfaction < 25) {
    float minDecreasePercent = (25 - satisfaction) / 100.0f; // 0-0.25 (0-25% decrease)
    int minDecrease = static_cast<int>(std::round(popVille * minDecreasePercent));
    if (delta >= 0) {
      delta = -minDecrease; // Override growth with decrease
    } else if (delta > -minDecrease) {
      delta = -minDecrease; // Make decrease stronger
    }
  }
  
  int nouvellePopulation = popVille + delta;

  // Cap at housing capacity and ensure >= 0
  if (nouvellePopulation > capaciteTotale)
    nouvellePopulation = capaciteTotale;
  if (nouvellePopulation < 0)
    nouvellePopulation = 0;

  // Distribute population into resident buildings relative to current occupants
  int difference = nouvellePopulation - popDansBatiments;

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
  
  // If population reaches 0, satisfaction is automatically 0
  if (nouvellePopulation == 0) {
    setSatisfaction(0);
  }
}

// Getters
string Ville::getNom() const { return nom; }
double Ville::getBudget() const { return budget; }
float Ville::getPolution() const { return polution; }
unsigned int Ville::getPopulation() const { return population; }  
int Ville::getSatisfaction() const { return satisfaction; }
Resources Ville::getResources() const { return resources; }


Batiment* Ville::getBatimentByPos(int x, int y) const {
    for (const auto &batimentPtr : batiments) {
        const Batiment &bat = *batimentPtr;

        int width = bat.surface.largeur;
        int height = bat.surface.longeur;

        if (x >= bat.position.x && x < bat.position.x +  height&&
            y >= bat.position.y && y < bat.position.y + width) {
            return batimentPtr.get(); 
        }
    }
    return nullptr;
}



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
      // Count job slots from all buildings that hire
      if (batiment->type == TypeBatiment::Cinema || 
          batiment->type == TypeBatiment::Mall ||
          batiment->type == TypeBatiment::Bank ||
          batiment->type == TypeBatiment::PowerPlant ||
          batiment->type == TypeBatiment::WaterTreatmentPlant ||
          batiment->type == TypeBatiment::Park) {
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
  unsigned int availableWorkers = static_cast<unsigned int>(calculerPopulationTotale());
  unsigned int totalJobs = calculerCapaciteEmploi();
  
  // Collect all buildings that employ people
  std::vector<Service*> employers;
  for (auto &batiment : batiments) {
    Service *s = dynamic_cast<Service *>(batiment.get());
    if (s && (batiment->type == TypeBatiment::Cinema || 
              batiment->type == TypeBatiment::Mall ||
              batiment->type == TypeBatiment::Bank ||
              batiment->type == TypeBatiment::PowerPlant ||
              batiment->type == TypeBatiment::WaterTreatmentPlant ||
              batiment->type == TypeBatiment::Park)) {
      employers.push_back(s);
    } else if (s) {
      s->setEmployees(0);
    }
  }
  
  if (employers.empty() || availableWorkers == 0) {
    // No jobs or no workers - set all to 0
    for (auto* s : employers) {
      s->setEmployees(0);
    }
    return;
  }
  
  if (availableWorkers >= totalJobs) {
    // Enough workers to fill all jobs
    for (auto* s : employers) {
      s->setEmployees(s->getEmployeesNeeded());
    }
  } else {
    // Not enough workers - distribute proportionally
    unsigned int workersAssigned = 0;
    
    // First pass: floor allocation to avoid over-allocation
    for (auto* s : employers) {
      unsigned int capacity = s->getEmployeesNeeded();
      float proportion = static_cast<float>(capacity) / static_cast<float>(totalJobs);
      unsigned int allocated = static_cast<unsigned int>(std::floor(availableWorkers * proportion));
      allocated = std::min(allocated, capacity); // Don't exceed building capacity
      s->setEmployees(allocated);
      workersAssigned += allocated;
    }
    
    // Second pass: distribute remaining workers one by one
    unsigned int remaining = availableWorkers - workersAssigned;
    for (auto* s : employers) {
      if (remaining == 0) break;
      unsigned int current = s->getEmployees();
      unsigned int capacity = s->getEmployeesNeeded();
      if (current < capacity) {
        s->setEmployees(current + 1);
        remaining--;
      }
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
              batiment->type == TypeBatiment::WaterTreatmentPlant ||
              batiment->type == TypeBatiment::Park)) {
      
      unsigned int employed = s->getEmployees();
      unsigned int capacity = s->getEmployeesNeeded();
      unsigned int empty = capacity - employed;
      
      std::cout << batiment->getNom() << ": " << employed << " / " << capacity 
                << " employed (" << empty << " empty slots)\n";
    }
  }
  std::cout << "\n\n";
}
