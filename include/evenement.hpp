#ifndef EVENEMENT
#define EVENEMENT

#include <string>
#include <functional>
#include <memory>
#include <vector>
#include <random>

// Forward declaration
class Ville;

// Enum for event categories
enum class EventCategory {
    Natural,      // Natural disasters
    Social,       // Social movements, strikes
    Economic,     // Economic events
    Technical,    // Technical failures
    Entertainment,// Fun/weird events
    Seasonal,     // Weather, seasonal events
    Health,       // Health-related events
    Transport,    // Transportation issues
    Wildlife      // Animal-related events
};

// Enum for event severity
enum class EventSeverity {
    Minor,        // Small impact
    Moderate,     // Medium impact
    Major,        // Large impact
    Critical      // Catastrophic impact
};

// Event effect structure
struct EventEffect {
    float satisfactionMultiplier = 1.0f;  // Multiplier for satisfaction
    float satisfactionChange = 0.0f;       // Direct change to satisfaction
    float pollutionMultiplier = 1.0f;      // Multiplier for pollution
    float pollutionChange = 0.0f;          // Direct change to pollution
    float waterConsumptionMultiplier = 1.0f; // Multiplier for water consumption
    float electricityConsumptionMultiplier = 1.0f; // Multiplier for electricity consumption
    float budgetChange = 0.0;              // Direct budget impact
    float budgetMultiplier = 1.0f;         // Income/expense multiplier
    int populationChange = 0;              // Direct population change
    bool blockParkEffects = false;         // Disable park benefits
    bool blockCommercialIncome = false;    // Disable commercial profits
    bool blockJobAssignment = false;       // Disable job assignments
    int duration = 1;                      // Duration in cycles (1 = this cycle only)
};

class Evenement {
public:
    Evenement(int id, const std::string& name, const std::string& description,
              EventCategory category, EventSeverity severity, 
              float probability, const EventEffect& effect);
    
    virtual ~Evenement() = default;

    // Apply the event effects to the city
    virtual void appliquer(Ville* ville);
    
    // Check if event can occur (some events have preconditions)
    virtual bool peutSeProduire(const Ville* ville) const;
    
    // Get display information
    std::string getNom() const;
    std::string getDescription() const;
    EventCategory getCategory() const;
    EventSeverity getSeverity() const;
    float getProbability() const;
    int getId() const;
    const EventEffect& getEffect() const;
    
    // Create a copy of this event
    virtual std::unique_ptr<Evenement> clone() const;

protected:
    int id;
    std::string nom;
    std::string description;
    EventCategory category;
    EventSeverity severity;
    float probability;  // 0.0 to 1.0 (chance per cycle)
    EventEffect effect;
};

// Event Manager - handles event generation and tracking
class EventManager {
public:
    EventManager();
    
    // Initialize all possible events
    void initialiserEvenements();
    
    // Try to trigger a random event for this cycle
    std::unique_ptr<Evenement> genererEvenementAleatoire(const Ville* ville);
    
    // Get event by ID
    const Evenement* getEvenement(int id) const;
    
    // Get all events in a category
    std::vector<const Evenement*> getEvenementsParCategorie(EventCategory category) const;

private:
    std::vector<std::unique_ptr<Evenement>> evenementsPossibles;
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    float baseTriggerChance = 0.02f; // Very low chance of any event per cycle
    
    // Helper to add events
    void ajouterEvenement(std::unique_ptr<Evenement> event);
};

#endif // !EVENEMENT

