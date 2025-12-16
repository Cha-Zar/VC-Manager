#ifndef UTILS
#define UTILS

#include <memory>
#include <string>
#include <vector>
#include <random>
#include <map>
#include <fstream>
#include <functional>
#include <sstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;


class Batiment;


struct Surface {
  float longeur;
  float largeur;
  Surface(int longeur = 0, int largeur = 0)
      : longeur(longeur), largeur(largeur) {}
};

struct Position {
  int x;
  int y;
  Position(int x = 0, int y = 0) : x(x), y(y) {}

  Position operator+(const Position &other) const {
    return {x + other.x, y + other.y};
  }

  Position operator-(const Position &other) const {
    return {x - other.x, y - other.y};
  }

  Position &operator+=(const Position &other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  Position &operator-=(const Position &other) {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  std::string toString() const {
    return "(" + std::to_string(x) + "," + std::to_string(y) + ")";
  }
};

struct Resources {
  double eau;
  double electricite;
  Resources(double e = 0.0, double el = 0.0) : eau(e), electricite(el) {}

  Resources operator+(const Resources &other) const {
    return {eau + other.eau, electricite + other.electricite};
  }

  Resources operator-(const Resources &other) const {
    return {eau - other.eau, electricite - other.electricite};
  }

  Resources &operator+=(const Resources &other) {
    eau += other.eau;
    electricite += other.electricite;
    return *this;
  }

  Resources &operator-=(const Resources &other) {
    eau -= other.eau;
    electricite -= other.electricite;
    return *this;
  }
};

struct WindowSettings {
  std::string title;
  const int width{960};
  const int height{640};
};

// Énumérations
enum class TypeBatiment {
  Blank, //0 
  House, //1
  Apartment, //2 
  Bank, //3
  PowerPlant, //4
  WaterTreatmentPlant, //5 
  UtilityPlant, //6
  Park, //7
  Cinema, //8
  Mall, //9
  Custom //10
};

enum class SimState { Running, Evaluating, GameOver };
enum class Difficulty { Easy, Medium, Hard };

using BatPtr = std::unique_ptr<Batiment>;
using BatimentList = std::vector<BatPtr>;

// Générateur d'IDs - déclaration seulement
class BuildingIDGenerator {
private:
    static std::hash<std::string> string_hasher;
    static std::random_device rd;
    static std::mt19937 gen;
    static std::uniform_int_distribution<int> dist;

public:
    static int generateID(const std::string& name, TypeBatiment type, 
                         const Position& position, const Surface& surface);
};

// Générateur de noms - déclaration seulement
class NameGenerator {
private:
    static std::map<TypeBatiment, std::vector<std::string>> buildingNames;
    static std::random_device rd;
    static std::mt19937 gen;
    static bool initialized;

    static void initializeNames();

public:
    static std::string getRandomName(TypeBatiment type);
};

#endif // !UTILS
