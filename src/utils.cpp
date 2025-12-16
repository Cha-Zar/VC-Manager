#include "../include/utils.hpp"
#include <stdexcept>
#include <iostream>

// Definition
std::hash<std::string> BuildingIDGenerator::string_hasher;
std::random_device BuildingIDGenerator::rd;
std::mt19937 BuildingIDGenerator::gen(BuildingIDGenerator::rd());
std::uniform_int_distribution<int> BuildingIDGenerator::dist(1000, 9999);

// Implémentation
int BuildingIDGenerator::generateID(const std::string& name, TypeBatiment type, 
                                   const Position& position, const Surface& surface) {
    std::string unique_string = 
        name + "_" +
        std::to_string(static_cast<int>(type)) + "_" +
        position.toString() + "_" +
        std::to_string(static_cast<int>(surface.largeur)) + "x" +
        std::to_string(static_cast<int>(surface.longeur)) + "_" +
        std::to_string(dist(gen));
    
    size_t hash = string_hasher(unique_string);
    return static_cast<int>(hash & 0x7FFFFFFF);
}

// Definition
std::map<TypeBatiment, std::vector<std::string>> NameGenerator::buildingNames;
std::random_device NameGenerator::rd;
std::mt19937 NameGenerator::gen(NameGenerator::rd());
bool NameGenerator::initialized = false;

// Implémentation 
void NameGenerator::initializeNames() {
    if (initialized) return;
    
    try {
        std::ifstream file("building-names.json");
        if (!file.is_open()) {
            std::cerr << "Fichier building-names.json non trouvé. Utilisation des noms par défaut." << std::endl;
            buildingNames[TypeBatiment::House] = {"Maison 1", "Maison 2", "Maison 3"};
            buildingNames[TypeBatiment::Apartment] = {"Appartement 1", "Appartement 2", "Appartement 3"};
            buildingNames[TypeBatiment::Park] = {"Parc Central", "Jardin Public", "Square"};
            buildingNames[TypeBatiment::Cinema] = {"Cinéma 1", "Cinéma 2", "Cinéma 3"};
            buildingNames[TypeBatiment::Mall] = {"Centre Commercial", "Galerie Marchande", "Shopping Center"};
            buildingNames[TypeBatiment::Bank] = {"Banque 1", "Banque 2", "Banque 3"};
            buildingNames[TypeBatiment::PowerPlant] = {"Centrale Électrique", "Usine d'Électricité"};
            buildingNames[TypeBatiment::WaterTreatmentPlant] = {"Station d'Épuration", "Usine de Traitement d'Eau"};
            buildingNames[TypeBatiment::UtilityPlant] = {"Usine des Services", "Complexe des Services"};
        } else {
            json j;
            try {
                file >> j;
                std::cout << "Fichier building-names.json chargé avec succès!" << std::endl;

                buildingNames[TypeBatiment::House] = j["House"].get<std::vector<std::string>>();
                buildingNames[TypeBatiment::Apartment] = j["Apartment"].get<std::vector<std::string>>();
                buildingNames[TypeBatiment::Park] = j["Park"].get<std::vector<std::string>>();
                buildingNames[TypeBatiment::Cinema] = j["Cinema"].get<std::vector<std::string>>();
                buildingNames[TypeBatiment::Mall] = j["Mall"].get<std::vector<std::string>>();
                buildingNames[TypeBatiment::Bank] = j["Bank"].get<std::vector<std::string>>();
                buildingNames[TypeBatiment::PowerPlant] = j["PowerPlant"].get<std::vector<std::string>>();
                buildingNames[TypeBatiment::WaterTreatmentPlant] = j["WaterTreatmentPlant"].get<std::vector<std::string>>();
                buildingNames[TypeBatiment::UtilityPlant] = j["UtilityPlant"].get<std::vector<std::string>>();
            } catch (const std::exception& json_error) {
                std::cerr << "Erreur de parsing JSON: " << json_error.what() << std::endl;
                throw;
            }
        }
        initialized = true;
    } catch (const std::exception& e) {
        std::cerr << "Erreur lors du chargement des noms de bâtiments: " << e.what() << std::endl;
        // Fallback final
        buildingNames[TypeBatiment::House] = {"Maison 1", "Maison 2", "Maison 3"};
        buildingNames[TypeBatiment::Apartment] = {"Appartement 1", "Appartement 2", "Appartement 3"};
        buildingNames[TypeBatiment::Park] = {"Parc Central", "Jardin Public", "Square"};
        buildingNames[TypeBatiment::Cinema] = {"Cinéma 1", "Cinéma 2", "Cinéma 3"};
        buildingNames[TypeBatiment::Mall] = {"Centre Commercial", "Galerie Marchande", "Shopping Center"};
        buildingNames[TypeBatiment::Bank] = {"Banque 1", "Banque 2", "Banque 3"};
        buildingNames[TypeBatiment::PowerPlant] = {"Centrale Électrique", "Usine d'Électricité"};
        buildingNames[TypeBatiment::WaterTreatmentPlant] = {"Station d'Épuration", "Usine de Traitement d'Eau"};
        buildingNames[TypeBatiment::UtilityPlant] = {"Usine des Services", "Complexe des Services"};
        initialized = true;
    }
}

// Implémentation 
std::string NameGenerator::getRandomName(TypeBatiment type) {
    initializeNames();
    
    auto it = buildingNames.find(type);
    if (it == buildingNames.end() || it->second.empty()) {
        return "Bâtiment Sans Nom";
    }

    std::uniform_int_distribution<> dis(0, it->second.size() - 1);
    return it->second[dis(gen)];
}
