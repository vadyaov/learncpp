#include <iostream>
#include <string>
#include <ctime>
#include <string_view>

class Creature {
    protected:
        std::string name;
        char symbol;
        int health;
        int damage;
        int gold;

    public:
        Creature(std::string_view c_name, char s, int h, int d, int g) : name{c_name}, symbol{s}, health{h}, damage{d}, gold{g} {}

        const std::string& getName() const { return name; }
        char getSymbol() const { return symbol; }
        int getHealth() const { return health; }
        int getDamage() const { return damage; }
        int getGold() const { return gold; }

        void reduceHealth(int delta) { health -= delta; }
        bool isDead() const { return health <= 0; }
        void addGold(int n) { gold += n; }

};

class Potion {
  public:
    enum Type {health, strength, poison, max_types};
    enum Size {small, medium, large, max_sizes};

    Potion(Type t, Size s) : type{t}, size{s} {}

    static Potion getRandomPotion() {
      Type randomType = Type(rand() % max_types);
      Size randomSize = Size(rand() % max_sizes);

      return Potion(randomType, randomSize);
    }

    Type getType() const { return type; }

    int getSize() const {
      if (type == health) {
        return static_cast<int>(size) * 1 + 2;
      } else if (type == strength) {
        return static_cast<int>(size) * 1 + 1;
      } else {
        return static_cast<int>(size) + 1;
      }
    }

    friend std::ostream& operator<<(std::ostream& os, const Potion& p) {
      switch (p.size) {
        case Potion::Size::small:
          os << "Small";
          break;
        case Potion::Size::medium:
          os << "Medium";
          break;
        case Potion::Size::large:
          os << "Large";
          break;
      }

      os << " potion of ";

      switch (p.type) {
        case Potion::Type::health:
          os << "Health";
          break;
        case Potion::Type::strength:
          os << "Strength";
          break;
        case Potion::Type::poison:
          os << "Poison";
          break;
      }
    
      return os;
    }

  private:
    Type type;
    Size size;
};

class Player : public Creature {
    private:
        int level{1};

    public:
        Player(std::string_view name) : Creature(name, '@', 10, 1, 0) {}

        void levelUp() { level += 1; damage += 1; }
        int getLevel() const { return level; }
        bool hasWon() const { return level == 20; }

        void drinkPotion(const Potion& potion) {
          Potion::Type potionType = potion.getType();

          switch (potionType) {
            case Potion::Type::health: {
              health += potion.getSize();
              std::cout << "Your health increased by " << potion.getSize() << "\n";
              break;
            }
            case Potion::Type::strength: {
              damage += potion.getSize();
              std::cout << "Your strength increased by " << potion.getSize() << "\n";
              break;
            }
            case Potion::Type::poison: {
              health -= potion.getSize();
              std::cout << "Your health decreased by " << potion.getSize() << "\n";
              break;
            }
          }
          std::cout << "You drank a " << potion << "\n";
        }
};

class Monster : public Creature {
    public:
        enum Type
        {
            dragon,
            orc,
            slime,
            max_types
        };

        Monster(Type t) : Creature(monsterData[t]) {}

        static Monster getRandomMonster() {
            return Monster(static_cast<Type>(rand() % max_types));
        }

        private:
            static inline Creature monsterData[]
            {
                Creature("dragon", 'D', 20, 4, 100),
                Creature("orc", 'o', 4, 2, 25),
                Creature("slime", 's', 1, 1, 10)
            };

            static_assert(std::size(monsterData) == max_types);
};

void attackPlayer(const Monster& m, Player& pl) {
    pl.reduceHealth(m.getDamage());
    std::cout << m.getName() << " hit you for " << m.getDamage() << " damage.\n";
}

void attackMonster(Player& pl, Monster& m) {
    std::cout << "You hit the " << m.getName() << " for " << pl.getDamage() << " damage.\n";
    m.reduceHealth(pl.getDamage());
    if (m.isDead()) {
        std::cout << "You killed the " << m.getName() << ".\n";

        pl.levelUp();
        std::cout << "You are now level " << pl.getLevel() << ".\n";

        pl.addGold(m.getGold());
        std::cout << "You found " << m.getGold() << " gold.\n";
        
        if (rand() % 4 == 3) {
          std::cout << "You found a mythical potion! Do you want to drink it? [y/n]: ";
          char c; std::cin >> c;
          if (c == 'y' || c == 'Y') {
            pl.drinkPotion(Potion::getRandomPotion());
          }
        }
    }
}

void fightMonster(Player& pl, Monster& m) {
    if (pl.isDead() || m.isDead()) return;

    std::cout << "(R)un or (F)ight: ";
    char c; std::cin >> c;

    switch (c) {
        case 'r': case 'R': {
            if (rand() % 2 == 1) {
                std::cout << "You succesfully fled.\n";
                return;
            }
            else {
                std::cout << "You failed to flee.\n";
                attackPlayer(m, pl);
            }

            break;
        }

        case 'f': case 'F': {
            attackMonster(pl, m);
            if (!m.isDead()) attackPlayer(m, pl);
            break;
        }
    }
    fightMonster(pl, m);
}

int main()
{
    srand(time(NULL));
    std::string playerName;
    std::cout << "Enter your name: ";
    std::cin >> playerName;
    std::cout << "Welcome, " << playerName << "\n";
    Player pl(playerName);

    while (1) {
        Monster monster = Monster::getRandomMonster();
        std::cout << "You have encountered " << monster.getName() << '(' << monster.getSymbol() << ')' << "\n";
        fightMonster(pl, monster);

        if (pl.isDead()) {
            std::cout << "You died at level " << pl.getLevel() << " and with " << pl.getGold() << "\n";
            std::cout << "Too bad you can`t take it with you!\n";
            break;
        }

        if (pl.hasWon()) {
            std::cout << "You won by reaching level " << pl.getLevel() << " and with " << pl.getGold() << " gold! Congratulations!\n";
            break;
        }
    }

	return 0;
}