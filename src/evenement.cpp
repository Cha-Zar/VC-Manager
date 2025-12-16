#include "../include/evenement.hpp"
#include "../include/ville/ville.hpp"
#include "../include/buildings/batiment.hpp"
#include <iostream>
#include <algorithm>

// ========== Evenement Implementation ==========

Evenement::Evenement(int id, const std::string& name, const std::string& description,
                     EventCategory category, EventSeverity severity,
                     float probability, const EventEffect& effect)
    : id(id), nom(name), description(description), category(category),
      severity(severity), probability(probability), effect(effect) {}

void Evenement::appliquer(Ville* ville) {
    if (!ville) return;
    
    std::cout << "\nEVENEMENT: " << nom << std::endl;
    std::cout << "Description: " << description << std::endl;
    
    // Apply satisfaction changes
    if (effect.satisfactionChange != 0.0f) {
        int currentSat = ville->getSatisfaction();
        int newSat = currentSat + static_cast<int>(effect.satisfactionChange);
        newSat = std::max(0, std::min(100, newSat));
        ville->setSatisfaction(newSat);
        std::cout << "Satisfaction: " << currentSat << " -> " << newSat << std::endl;
    }
    
    // Apply pollution changes
    if (effect.pollutionChange != 0.0f) {
        float currentPol = ville->getPolution();
        float newPol = currentPol + effect.pollutionChange;
        newPol = std::max(0.0f, std::min(100.0f, newPol));
        ville->setPolution(newPol);
        std::cout << "Pollution: " << currentPol << " -> " << newPol << std::endl;
    }
    
    // Apply budget changes
    if (effect.budgetChange != 0.0) {
        double currentBudget = ville->getBudget();
        double newBudget = currentBudget + effect.budgetChange;
        ville->setBudget(newBudget);
        std::cout << "Budget: " << currentBudget << " -> " << newBudget 
                  << " (" << (effect.budgetChange >= 0 ? "+" : "") << effect.budgetChange << ")" << std::endl;
    }
    
    // Apply population changes
    if (effect.populationChange != 0) {
        unsigned int currentPop = ville->getPopulation();
        int newPop = static_cast<int>(currentPop) + effect.populationChange;
        newPop = std::max(0, newPop);
        ville->setPopulation(static_cast<unsigned int>(newPop));
        std::cout << "Population: " << currentPop << " -> " << newPop << std::endl;
    }
    
    std::cout << std::endl;
}

bool Evenement::peutSeProduire(const Ville* ville) const {
    // By default, all events can occur
    // Subclasses can override for specific conditions
    return true;
}

std::string Evenement::getNom() const { return nom; }
std::string Evenement::getDescription() const { return description; }
EventCategory Evenement::getCategory() const { return category; }
EventSeverity Evenement::getSeverity() const { return severity; }
float Evenement::getProbability() const { return probability; }
int Evenement::getId() const { return id; }
const EventEffect& Evenement::getEffect() const { return effect; }

std::unique_ptr<Evenement> Evenement::clone() const {
    return std::make_unique<Evenement>(*this);
}

// ========== EventManager Implementation ==========

EventManager::EventManager() : rng(std::random_device{}()), dist(0.0f, 1.0f) {}

void EventManager::ajouterEvenement(std::unique_ptr<Evenement> event) {
    evenementsPossibles.push_back(std::move(event));
}

void EventManager::initialiserEvenements() {
    evenementsPossibles.clear();
    
    // ===== NATURAL DISASTERS =====
    
    // 1. Fire / Incendie
    EventEffect incendie;
    incendie.satisfactionChange = -20.0f;
    incendie.pollutionChange = 15.0f;
    incendie.budgetChange = -500.0;
    ajouterEvenement(std::make_unique<Evenement>(
        1, "Grand Incendie",
        "Un incendie majeur ravage plusieurs quartiers! Les pompiers luttent pour contenir les flammes.",
        EventCategory::Natural, EventSeverity::Major, 0.08f, incendie
    ));
    
    // 2. Flood / Inondation
    EventEffect inondation;
    inondation.satisfactionChange = -25.0f;
    inondation.waterConsumptionMultiplier = 0.5f;
    inondation.budgetChange = -700.0;
    ajouterEvenement(std::make_unique<Evenement>(
        2, "Inondation Catastrophique",
        "Des pluies torrentielles causent des inondations massives. Les infrastructures d'eau sont endommagées.",
        EventCategory::Natural, EventSeverity::Critical, 0.05f, inondation
    ));
    
    // 3. Earthquake / Tremblement de terre
    EventEffect earthquake;
    earthquake.satisfactionChange = -30.0f;
    earthquake.budgetChange = -1000.0;
    earthquake.populationChange = -20;
    ajouterEvenement(std::make_unique<Evenement>(
        3, "Tremblement de Terre",
        "Un séisme secoue la ville! Des bâtiments s'effondrent et la population panique.",
        EventCategory::Natural, EventSeverity::Critical, 0.03f, earthquake
    ));
    
    // 4. Tornado / Tornade
    EventEffect tornado;
    tornado.satisfactionChange = -22.0f;
    tornado.budgetChange = -650.0;
    tornado.electricityConsumptionMultiplier = 0.7f;
    ajouterEvenement(std::make_unique<Evenement>(
        4, "Tornade Destructrice",
        "Une tornade traverse la ville, détruisant tout sur son passage!",
        EventCategory::Natural, EventSeverity::Major, 0.04f, tornado
    ));
    
    // 5. Heatwave / Canicule
    EventEffect canicule;
    canicule.satisfactionChange = -15.0f;
    canicule.waterConsumptionMultiplier = 2.0f;
    canicule.electricityConsumptionMultiplier = 1.8f;
    ajouterEvenement(std::make_unique<Evenement>(
        5, "Canicule Extreme",
        "Une vague de chaleur écrasante frappe la ville. La consommation d'eau et d'électricité explose!",
        EventCategory::Seasonal, EventSeverity::Moderate, 0.12f, canicule
    ));
    
    // 6. Blizzard / Tempête de neige
    EventEffect blizzard;
    blizzard.satisfactionChange = -18.0f;
    blizzard.blockCommercialIncome = true;
    blizzard.budgetChange = -400.0;
    ajouterEvenement(std::make_unique<Evenement>(
        6, " Blizzard Paralysant",
        "Une tempête de neige bloque les routes et interrompt la production. Envoyez les déneigeurs!",
        EventCategory::Seasonal, EventSeverity::Major, 0.06f, blizzard
    ));
    
    // 7. Hurricane / Ouragan
    EventEffect hurricane;
    hurricane.satisfactionChange = -35.0f;
    hurricane.budgetChange = -1200.0;
    hurricane.pollutionChange = -10.0f; // Washes away pollution
    hurricane.populationChange = -30;
    ajouterEvenement(std::make_unique<Evenement>(
        7, " Ouragan Dévastateur",
        "Un ouragan catégorie 4 frappe la ville! Destructions massives mais l'air est purifié.",
        EventCategory::Natural, EventSeverity::Critical, 0.02f, hurricane
    ));
    
    // ===== SOCIAL EVENTS =====
    
    // 8. General Strike / Grève générale
    EventEffect strike;
    strike.satisfactionChange = -20.0f;
    strike.blockJobAssignment = true;
    strike.blockCommercialIncome = true;
    ajouterEvenement(std::make_unique<Evenement>(
        8, "Grève Générale",
        "Les travailleurs sont en grève! Aucune production, les services sont paralysés.",
        EventCategory::Social, EventSeverity::Major, 0.10f, strike
    ));
    
    // 9. Gardeners Strike / Grève des jardiniers
    EventEffect gardenerStrike;
    gardenerStrike.satisfactionChange = -20.0f;
    gardenerStrike.blockParkEffects = true;
    ajouterEvenement(std::make_unique<Evenement>(
        9, "Grève des Jardiniers",
        "Les jardiniers refusent d'entretenir les parcs! L'effet des espaces verts diminue de moitié.",
        EventCategory::Social, EventSeverity::Moderate, 0.09f, gardenerStrike
    ));
    
    // 10. Protest / Manifestation
    EventEffect protest;
    protest.satisfactionChange = -12.0f;
    protest.budgetChange = -200.0; // Police costs
    ajouterEvenement(std::make_unique<Evenement>(
        10, "Manifestation Massive",
        "Des milliers de citoyens manifestent dans les rues. La police doit intervenir.",
        EventCategory::Social, EventSeverity::Minor, 0.15f, protest
    ));
    
    // 11. Cultural Festival / Festival culturel
    EventEffect festival;
    festival.satisfactionChange = 15.0f;
    festival.budgetChange = 300.0; // Tourism revenue
    ajouterEvenement(std::make_unique<Evenement>(
        11, "Festival Culturel",
        "Un grand festival attire des touristes du monde entier! Joie et profits pour tous.",
        EventCategory::Entertainment, EventSeverity::Minor, 0.12f, festival
    ));
    
    // 12. Revolution Attempt / Tentative de révolution
    EventEffect revolution;
    revolution.satisfactionChange = -40.0f;
    revolution.budgetChange = -800.0;
    revolution.populationChange = -15;
    ajouterEvenement(std::make_unique<Evenement>(
        12, "Tentative de Révolution",
        "Des révolutionnaires tentent de renverser le gouvernement! Chaos dans les rues.",
        EventCategory::Social, EventSeverity::Critical, 0.03f, revolution
    ));
    
    // ===== TECHNICAL / INFRASTRUCTURE =====
    
    // 13. Power Outage / Panne de courant
    EventEffect powerOut;
    powerOut.satisfactionChange = -15.0f;
    powerOut.electricityConsumptionMultiplier = 1.5f;
    ajouterEvenement(std::make_unique<Evenement>(
        13, " Panne de Courant Géante",
        "Une panne massive fait grimper la consommation d'énergie de 50%!",
        EventCategory::Technical, EventSeverity::Moderate, 0.11f, powerOut
    ));
    
    // 14. Water Pipe Burst / Rupture de canalisation
    EventEffect pipeBurst;
    pipeBurst.satisfactionChange = -10.0f;
    pipeBurst.budgetChange = -350.0;
    pipeBurst.waterConsumptionMultiplier = 2.0f;
    ajouterEvenement(std::make_unique<Evenement>(
        14, " Rupture des Canalisations",
        "Les canalisations principales explosent! L'eau se déverse dans les rues.",
        EventCategory::Technical, EventSeverity::Moderate, 0.10f, pipeBurst
    ));
    
    // 15. Internet Blackout / Coupure Internet
    EventEffect internetDown;
    internetDown.satisfactionChange = -18.0f;
    internetDown.blockCommercialIncome = true;
    ajouterEvenement(std::make_unique<Evenement>(
        15, "Coupure Internet Totale",
        "Tout Internet est hors service! Les commerces en ligne sont paralysés.",
        EventCategory::Technical, EventSeverity::Major, 0.07f, internetDown
    ));
    
    // 16. Nuclear Meltdown Scare / Alerte nucléaire
    EventEffect nuclearScare;
    nuclearScare.satisfactionChange = -45.0f;
    nuclearScare.pollutionChange = 30.0f;
    nuclearScare.populationChange = -50;
    nuclearScare.budgetChange = -1500.0;
    ajouterEvenement(std::make_unique<Evenement>(
        16, " Alerte Nucléaire",
        "Une centrale nucléaire menace de fondre! Panique et évacuations massives.",
        EventCategory::Technical, EventSeverity::Critical, 0.01f, nuclearScare
    ));
    
    // 17. Gas Leak / Fuite de gaz
    EventEffect gasLeak;
    gasLeak.satisfactionChange = -20.0f;
    gasLeak.budgetChange = -450.0;
    gasLeak.pollutionChange = 12.0f;
    ajouterEvenement(std::make_unique<Evenement>(
        17, " Fuite de Gaz Massive",
        "Une énorme fuite de gaz force l'évacuation de plusieurs quartiers!",
        EventCategory::Technical, EventSeverity::Major, 0.06f, gasLeak
    ));
    
    // ===== TRANSPORT =====
    
    // 18. Traffic Jam Hell / Embouteillage monstre
    EventEffect trafficJam;
    trafficJam.satisfactionChange = -25.0f;
    trafficJam.blockJobAssignment = true;
    ajouterEvenement(std::make_unique<Evenement>(
        18, "Embouteillage Géant",
        "Tous les bus sont bloqués! Les habitants doivent marcher pour aller au travail.",
        EventCategory::Transport, EventSeverity::Major, 0.13f, trafficJam
    ));
    
    // 19. Metro Strike / Grève des transports
    EventEffect metroStrike;
    metroStrike.satisfactionChange = -20.0f;
    metroStrike.budgetChange = -300.0;
    ajouterEvenement(std::make_unique<Evenement>(
        19, " Grève des Transports",
        "Les conducteurs de métro et bus sont en grève. Chaos dans la circulation!",
        EventCategory::Transport, EventSeverity::Moderate, 0.12f, metroStrike
    ));
    
    // 20. Bridge Collapse / Effondrement de pont
    EventEffect bridgeCollapse;
    bridgeCollapse.satisfactionChange = -30.0f;
    bridgeCollapse.budgetChange = -900.0;
    bridgeCollapse.populationChange = -10;
    ajouterEvenement(std::make_unique<Evenement>(
        20, " Effondrement de Pont",
        "Un pont majeur s'effondre! Victimes et isolement de quartiers entiers.",
        EventCategory::Transport, EventSeverity::Critical, 0.04f, bridgeCollapse
    ));
    
    // ===== ECONOMIC =====
    
    // 21. Economic Boom / Boom économique
    EventEffect boom;
    boom.satisfactionChange = 20.0f;
    boom.budgetChange = 1000.0;
    boom.budgetMultiplier = 1.5f;
    ajouterEvenement(std::make_unique<Evenement>(
        21, " Boom Économique",
        "L'économie explose! Les revenus augmentent de 50% ce cycle.",
        EventCategory::Economic, EventSeverity::Major, 0.05f, boom
    ));
    
    // 22. Stock Market Crash / Krach boursier
    EventEffect crash;
    crash.satisfactionChange = -25.0f;
    crash.budgetChange = -800.0;
    crash.budgetMultiplier = 0.5f;
    ajouterEvenement(std::make_unique<Evenement>(
        22, " Krach Boursier",
        "La bourse s'effondre! Les revenus commerciaux sont divisés par deux.",
        EventCategory::Economic, EventSeverity::Major, 0.06f, crash
    ));
    
    // 23. Tax Revolt / Révolte fiscale
    EventEffect taxRevolt;
    taxRevolt.satisfactionChange = -20.0f;
    taxRevolt.budgetMultiplier = 0.7f;
    ajouterEvenement(std::make_unique<Evenement>(
        23, "Révolte Fiscale",
        "Les citoyens refusent de payer leurs impôts! Les revenus chutent de 30%.",
        EventCategory::Economic, EventSeverity::Moderate, 0.10f, taxRevolt
    ));
    
    // 24. Lottery Winner Donation / Don de gagnant de loterie
    EventEffect lotteryDonation;
    lotteryDonation.satisfactionChange = 10.0f;
    lotteryDonation.budgetChange = 2000.0;
    ajouterEvenement(std::make_unique<Evenement>(
        24, "Don Généreux",
        "Un gagnant de loterie fait don de sa fortune à la ville! 2000$ ajoutés au budget.",
        EventCategory::Economic, EventSeverity::Minor, 0.02f, lotteryDonation
    ));
    
    // 25. Bank Robbery / Braquage de banque
    EventEffect bankRobbery;
    bankRobbery.satisfactionChange = -10.0f;
    bankRobbery.budgetChange = -400.0;
    ajouterEvenement(std::make_unique<Evenement>(
        25, "Braquage de Banque",
        "Des bandits braquent la banque centrale et s'enfuient avec l'argent!",
        EventCategory::Economic, EventSeverity::Moderate, 0.08f, bankRobbery
    ));
    
    // ===== HEALTH =====
    
    // 26. Flu Epidemic / Épidémie de grippe
    EventEffect flu;
    flu.satisfactionChange = -15.0f;
    flu.blockJobAssignment = true;
    flu.budgetChange = -350.0;
    ajouterEvenement(std::make_unique<Evenement>(
        26, "Épidémie de Grippe",
        "Une grippe virulente frappe la ville. 30% des travailleurs sont malades!",
        EventCategory::Health, EventSeverity::Moderate, 0.10f, flu
    ));
    
    // 27. Food Poisoning / Intoxication alimentaire
    EventEffect foodPoison;
    foodPoison.satisfactionChange = -20.0f;
    foodPoison.populationChange = -5;
    foodPoison.budgetChange = -250.0;
    ajouterEvenement(std::make_unique<Evenement>(
        27, "Intoxication Massive",
        "Des aliments contaminés causent une intoxication massive! Hôpitaux débordés.",
        EventCategory::Health, EventSeverity::Moderate, 0.07f, foodPoison
    ));
    
    // 28. Pandemic / Pandémie
    EventEffect pandemic;
    pandemic.satisfactionChange = -40.0f;
    pandemic.populationChange = -100;
    pandemic.budgetChange = -1500.0;
    pandemic.blockJobAssignment = true;
    ajouterEvenement(std::make_unique<Evenement>(
        28, "Pandémie Mondiale",
        "Une pandémie dévastatrice frappe! Confinement général, l'économie s'arrête.",
        EventCategory::Health, EventSeverity::Critical, 0.01f, pandemic
    ));
    
    // 29. Air Pollution Crisis / Crise de pollution
    EventEffect pollutionCrisis;
    pollutionCrisis.satisfactionChange = -25.0f;
    pollutionCrisis.pollutionChange = 40.0f;
    pollutionCrisis.budgetChange = -400.0;
    ajouterEvenement(std::make_unique<Evenement>(
        29, "Crise de Pollution",
        "Un nuage toxique recouvre la ville! Alertes sanitaires et hospitalisations.",
        EventCategory::Health, EventSeverity::Major, 0.08f, pollutionCrisis
    ));
    
    // 30. Medical Breakthrough / Percée médicale
    EventEffect medicalBreak;
    medicalBreak.satisfactionChange = 15.0f;
    medicalBreak.budgetChange = 500.0;
    ajouterEvenement(std::make_unique<Evenement>(
        30, "Percée Médicale",
        "Un chercheur local découvre un remède révolutionnaire! La ville devient célèbre.",
        EventCategory::Health, EventSeverity::Minor, 0.03f, medicalBreak
    ));
    
    // ===== WILDLIFE / ANIMALS =====
    
    // 31. Giant Pigeons / Pigeons géants
    EventEffect pigeons;
    pigeons.satisfactionChange = -15.0f;
    pigeons.pollutionChange = 10.0f;
    ajouterEvenement(std::make_unique<Evenement>(
        31, "Invasion de Pigeons Géants",
        "Des pigeons géants envahissent la ville! Déchets partout, nettoyage urgent nécessaire.",
        EventCategory::Wildlife, EventSeverity::Moderate, 0.12f, pigeons
    ));
    
    // 32. Rat Infestation / Invasion de rats
    EventEffect rats;
    rats.satisfactionChange = -22.0f;
    rats.pollutionChange = 15.0f;
    rats.budgetChange = -300.0;
    ajouterEvenement(std::make_unique<Evenement>(
        32, "Invasion de Rats",
        "Les rats envahissent les égouts et remontent dans les maisons! Désinfection urgente.",
        EventCategory::Wildlife, EventSeverity::Moderate, 0.09f, rats
    ));
    
    // 33. Bee Swarm / Essaim d'abeilles
    EventEffect bees;
    bees.satisfactionChange = -10.0f;
    bees.budgetChange = -150.0;
    ajouterEvenement(std::make_unique<Evenement>(
        33, "Essaim d'Abeilles Géant",
        "Un essaim d'abeilles géant s'installe en plein centre-ville! Apiculteurs à la rescousse.",
        EventCategory::Wildlife, EventSeverity::Minor, 0.06f, bees
    ));
    
    // 34. Wolf Pack / Meute de loups
    EventEffect wolves;
    wolves.satisfactionChange = -18.0f;
    wolves.populationChange = -8;
    wolves.budgetChange = -250.0;
    ajouterEvenement(std::make_unique<Evenement>(
        34, "Meute de Loups",
        "Une meute de loups descend des montagnes et attaque! Panique et victimes.",
        EventCategory::Wildlife, EventSeverity::Major, 0.04f, wolves
    ));
    
    // 35. Seagull Attack / Attaque de mouettes
    EventEffect seagulls;
    seagulls.satisfactionChange = -8.0f;
    seagulls.pollutionChange = 5.0f;
    ajouterEvenement(std::make_unique<Evenement>(
        35, "Attaque de Mouettes",
        "Des mouettes agressives volent la nourriture et attaquent les passants!",
        EventCategory::Wildlife, EventSeverity::Minor, 0.10f, seagulls
    ));
    
    // 36. Stray Dog Army / Armée de chiens errants
    EventEffect dogs;
    dogs.satisfactionChange = -12.0f;
    dogs.budgetChange = -200.0;
    ajouterEvenement(std::make_unique<Evenement>(
        36, " Armée de Chiens Errants",
        "Des centaines de chiens errants forment des meutes dans les rues! Refuge animalier débordé.",
        EventCategory::Wildlife, EventSeverity::Minor, 0.08f, dogs
    ));
    
    // 37. Locust Swarm / Nuée de sauterelles
    EventEffect locusts;
    locusts.satisfactionChange = -20.0f;
    locusts.budgetChange = -400.0;
    locusts.pollutionChange = 8.0f;
    ajouterEvenement(std::make_unique<Evenement>(
        37, "Nuée de Sauterelles",
        "Des milliards de sauterelles dévorent tout! Les jardins et parcs sont dévastés.",
        EventCategory::Wildlife, EventSeverity::Major, 0.03f, locusts
    ));
    
    // 38. Dolphin Visit / Visite de dauphins
    EventEffect dolphins;
    dolphins.satisfactionChange = 15.0f;
    dolphins.budgetChange = 300.0; // Tourism
    ajouterEvenement(std::make_unique<Evenement>(
        38, "Visite de Dauphins",
        "Des dauphins visitent le port! Les touristes affluent pour les voir.",
        EventCategory::Wildlife, EventSeverity::Minor, 0.05f, dolphins
    ));
    
    // ===== ENTERTAINMENT / WEIRD =====
    
    // 39. UFO Sighting / Observation d'OVNI
    EventEffect ufo;
    ufo.satisfactionChange = 10.0f;
    ufo.budgetChange = 500.0; // Tourist attraction
    ajouterEvenement(std::make_unique<Evenement>(
        39, "Observation d'OVNI",
        "Un OVNI est aperçu au-dessus de la ville! Les ufologues envahissent la ville.",
        EventCategory::Entertainment, EventSeverity::Minor, 0.02f, ufo
    ));
    
    // 40. Celebrity Visit / Visite de célébrité
    EventEffect celebrity;
    celebrity.satisfactionChange = 12.0f;
    celebrity.budgetChange = 400.0;
    ajouterEvenement(std::make_unique<Evenement>(
        40, "Visite de Célébrité",
        "Une star mondiale visite votre ville! Euphorie générale et revenus touristiques.",
        EventCategory::Entertainment, EventSeverity::Minor, 0.08f, celebrity
    ));
    
    // 41. Zombie Apocalypse Drill / Exercice zombie
    EventEffect zombieDrill;
    zombieDrill.satisfactionChange = 5.0f;
    zombieDrill.budgetChange = -100.0;
    ajouterEvenement(std::make_unique<Evenement>(
        41, "Exercice Anti-Zombie",
        "La ville organise un exercice d'apocalypse zombie! C'est fun mais coûteux.",
        EventCategory::Entertainment, EventSeverity::Minor, 0.04f, zombieDrill
    ));
    
    // 42. Flash Mob / Flash mob géant
    EventEffect flashMob;
    flashMob.satisfactionChange = 8.0f;
    ajouterEvenement(std::make_unique<Evenement>(
        42, "Flash Mob Géant",
        "Un flash mob spontané réunit des milliers de danseurs! Vidéos virales garanties.",
        EventCategory::Entertainment, EventSeverity::Minor, 0.10f, flashMob
    ));
    
    // 43. Time Capsule Discovery / Découverte de capsule temporelle
    EventEffect timeCapsule;
    timeCapsule.satisfactionChange = 10.0f;
    timeCapsule.budgetChange = 600.0;
    ajouterEvenement(std::make_unique<Evenement>(
        43, " Capsule Temporelle",
        "Une capsule temporelle de 1900 est découverte! Trésors historiques et attraction touristique.",
        EventCategory::Entertainment, EventSeverity::Minor, 0.03f, timeCapsule
    ));
    
    // 44. Giant Rubber Duck / Canard géant
    EventEffect rubberDuck;
    rubberDuck.satisfactionChange = 15.0f;
    rubberDuck.budgetChange = 300.0;
    ajouterEvenement(std::make_unique<Evenement>(
        44, "Canard de Bain Géant",
        "Un énorme canard de bain gonflable apparaît dans le port! Sensation mondiale.",
        EventCategory::Entertainment, EventSeverity::Minor, 0.05f, rubberDuck
    ));
    
    // 45. Kaiju Scare / Alerte monstre
    EventEffect kaiju;
    kaiju.satisfactionChange = -15.0f;
    kaiju.budgetChange = -200.0;
    ajouterEvenement(std::make_unique<Evenement>(
        45, "Fausse Alerte Monstre",
        "Une fausse alerte Godzilla cause une panique! C'était juste un gros lézard.",
        EventCategory::Entertainment, EventSeverity::Minor, 0.03f, kaiju
    ));
    
    // ===== MORE UNIQUE EVENTS =====
    
    // 46. Meteor Shower / Pluie de météorites
    EventEffect meteor;
    meteor.satisfactionChange = 20.0f;
    meteor.budgetChange = 700.0; // Space tourism
    ajouterEvenement(std::make_unique<Evenement>(
        46, " Pluie de Météorites",
        "Un spectacle céleste époustouflant! Les touristes affluent pour observer.",
        EventCategory::Natural, EventSeverity::Minor, 0.04f, meteor
    ));
    
    // 47. Ancient Ruins Discovery / Ruines anciennes
    EventEffect ruins;
    ruins.satisfactionChange = 18.0f;
    ruins.budgetChange = 1000.0;
    ajouterEvenement(std::make_unique<Evenement>(
        47, "Découverte Archéologique",
        "Des ruines antiques sont découvertes! Votre ville devient un site du patrimoine mondial.",
        EventCategory::Entertainment, EventSeverity::Minor, 0.02f, ruins
    ));
    
    // 48. Volcano Eruption / Éruption volcanique
    EventEffect volcano;
    volcano.satisfactionChange = -50.0f;
    volcano.pollutionChange = 50.0f;
    volcano.budgetChange = -2000.0;
    volcano.populationChange = -80;
    ajouterEvenement(std::make_unique<Evenement>(
        48, "Éruption Volcanique",
        "Un volcan entre en éruption! Lave, cendres et destruction massive.",
        EventCategory::Natural, EventSeverity::Critical, 0.01f, volcano
    ));
    
    // 49. Rainbow Phenomenon / Phénomène arc-en-ciel
    EventEffect rainbow;
    rainbow.satisfactionChange = 12.0f;
    rainbow.budgetChange = 200.0;
    ajouterEvenement(std::make_unique<Evenement>(
        49, "Double Arc-en-ciel",
        "Un magnifique double arc-en-ciel apparaît! Tout le monde prend des photos.",
        EventCategory::Seasonal, EventSeverity::Minor, 0.08f, rainbow
    ));
    
    // 50. Fountain Malfunction / Fontaine dysfonctionnelle
    EventEffect fountain;
    fountain.satisfactionChange = 8.0f;
    fountain.waterConsumptionMultiplier = 1.3f;
    ajouterEvenement(std::make_unique<Evenement>(
        50, "Fontaine Folle",
        "La fontaine centrale devient incontrôlable et arrose tout le monde! C'est hilarant.",
        EventCategory::Technical, EventSeverity::Minor, 0.06f, fountain
    ));
    
    // 51. Insomnia Wave / Vague d'insomnie
    EventEffect insomnia;
    insomnia.satisfactionChange = -14.0f;
    insomnia.electricityConsumptionMultiplier = 1.4f;
    ajouterEvenement(std::make_unique<Evenement>(
        51, "Vague d'Insomnie",
        "Personne n'arrive à dormir! Les lumières restent allumées toute la nuit.",
        EventCategory::Health, EventSeverity::Minor, 0.07f, insomnia
    ));
    
    // 52. Singing Competition / Concours de chant
    EventEffect singing;
    singing.satisfactionChange = 10.0f;
    singing.budgetChange = 250.0;
    ajouterEvenement(std::make_unique<Evenement>(
        52, "Concours de Chant",
        "Un grand concours de talents attire des participants de partout!",
        EventCategory::Entertainment, EventSeverity::Minor, 0.09f, singing
    ));
    
    // 53. Alien Crop Circles / Cercles de culture
    EventEffect cropCircles;
    cropCircles.satisfactionChange = 5.0f;
    cropCircles.budgetChange = 300.0;
    ajouterEvenement(std::make_unique<Evenement>(
        53, "Cercles de Culture",
        "Des motifs mystérieux apparaissent dans les champs! Les théoriciens du complot arrivent.",
        EventCategory::Entertainment, EventSeverity::Minor, 0.03f, cropCircles
    ));
    
    // 54. Fashion Week / Semaine de la mode
    EventEffect fashion;
    fashion.satisfactionChange = 15.0f;
    fashion.budgetChange = 800.0;
    ajouterEvenement(std::make_unique<Evenement>(
        54, "Fashion Week",
        "Votre ville accueille la Fashion Week! Défilés, célébrités et économie florissante.",
        EventCategory::Entertainment, EventSeverity::Minor, 0.04f, fashion
    ));
    
    // 55. Robot Malfunction / Robots défectueux
    EventEffect robots;
    robots.satisfactionChange = -10.0f;
    robots.budgetChange = -300.0;
    ajouterEvenement(std::make_unique<Evenement>(
        55, "Robots en Folie",
        "Les robots de nettoyage deviennent fous et nettoient... trop! Chaos organisé.",
        EventCategory::Technical, EventSeverity::Minor, 0.05f, robots
    ));
    
    // 56. Mysterious Fog / Brouillard mystérieux
    EventEffect fog;
    fog.satisfactionChange = -8.0f;
    fog.pollutionChange = 5.0f;
    ajouterEvenement(std::make_unique<Evenement>(
        56, " Brouillard Mystérieux",
        "Un brouillard épais et étrange enveloppe la ville. Visibilité nulle!",
        EventCategory::Seasonal, EventSeverity::Minor, 0.10f, fog
    ));
    
    // 57. Sports Championship / Championnat sportif
    EventEffect sports;
    sports.satisfactionChange = 20.0f;
    sports.budgetChange = 600.0;
    ajouterEvenement(std::make_unique<Evenement>(
        57, " Championnat Gagné",
        "L'équipe locale gagne le championnat! Euphorie collective et célébrations.",
        EventCategory::Entertainment, EventSeverity::Minor, 0.05f, sports
    ));
    
    // 58. Magician's Mistake / Erreur de magicien
    EventEffect magician;
    magician.satisfactionChange = 5.0f;
    magician.budgetChange = -100.0;
    ajouterEvenement(std::make_unique<Evenement>(
        58, "Tour de Magie Raté",
        "Un magicien fait disparaître... la mairie! Heureusement, elle réapparaît.",
        EventCategory::Entertainment, EventSeverity::Minor, 0.04f, magician
    ));
    
    // 59. Noise Pollution / Pollution sonore
    EventEffect noise;
    noise.satisfactionChange = -12.0f;
    ajouterEvenement(std::make_unique<Evenement>(
        59, "Pollution Sonore",
        "Des travaux nocturnes créent un vacarme insupportable! Personne ne dort.",
        EventCategory::Health, EventSeverity::Minor, 0.11f, noise
    ));
    
    // 60. Treasure Found / Trésor trouvé
    EventEffect treasure;
    treasure.satisfactionChange = 15.0f;
    treasure.budgetChange = 1500.0;
    ajouterEvenement(std::make_unique<Evenement>(
        60, "Trésor de Pirates",
        "Un trésor de pirates est découvert sous la ville! Fortune instantanée.",
        EventCategory::Entertainment, EventSeverity::Minor, 0.01f, treasure
    ));
    
    std::cout << " " << evenementsPossibles.size() << " événements initialisés!" << std::endl;
}

std::unique_ptr<Evenement> EventManager::genererEvenementAleatoire(const Ville* ville) {
    if (!ville || evenementsPossibles.empty()) {
        return nullptr;
    }
    // Global gate: only try events with a small base chance per cycle
    if (dist(rng) >= baseTriggerChance) {
        return nullptr;
    }
    
    // Collect all eligible events
    std::vector<Evenement*> eligibleEvents;
    for (auto& event : evenementsPossibles) {
        if (event->peutSeProduire(ville)) {
            float roll = dist(rng);
            if (roll < event->getProbability()) {
                eligibleEvents.push_back(event.get());
            }
        }
    }
    
    // If multiple events can occur, pick one randomly
    if (eligibleEvents.empty()) {
        return nullptr;
    }
    
    std::uniform_int_distribution<size_t> indexDist(0, eligibleEvents.size() - 1);
    size_t selectedIndex = indexDist(rng);
    
    return eligibleEvents[selectedIndex]->clone();
}

const Evenement* EventManager::getEvenement(int id) const {
    for (const auto& event : evenementsPossibles) {
        if (event->getId() == id) {
            return event.get();
        }
    }
    return nullptr;
}

std::vector<const Evenement*> EventManager::getEvenementsParCategorie(EventCategory category) const {
    std::vector<const Evenement*> results;
    for (const auto& event : evenementsPossibles) {
        if (event->getCategory() == category) {
            results.push_back(event.get());
        }
    }
    return results;
}
