#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <cstring>
#include <cstdlib>
#include <ctime>

using namespace std;

#define SPELL 30
#define XMAX 17630
#define YMAX 9000

class	Entity;
class	Monster;
class	Hero;
class	Base;

static int	g_turn = 0;
static int	g_i = 0;
static int	g_iga = 0;

Monster	&monsterClosest(vector<Monster> &monster);
bool isEnnemyIn(vector<Hero> &hero, Base &base);

enum e_type { TYPE_MONSTER, TYPE_MY_HERO, TYPE_OP_HERO };

enum e_name { GIMLI, GANDOULF, ARAGORN };

struct s_target {
	int	pos_x;
	int	pos_y;
	int	score_wind;
	int	score_attack;
	int	score_control;
	int	score_shield;
	int	score_wait;
	int	id;
};

class Base {
	public:
		Base() : health(0), mana(0), x(0), y(0), side(0), ennemy_in(0) {};
		Base(int health, int mana, int x, int y, int side) : health(health), mana(mana), x(x), y(y), side(side), ennemy_in(0) {};

		Base(const Base &base) {
			*this = base;
		};

		int	getHealth() const { return health; }
		int	getMana() const { return mana; }

		int	x;
		int	y;
		int	health;
		int	mana;
		int	side;
		int	ennemy_in;

		Base    &operator=(const Base &base) {
			x = base.x;
			y = base.y;
			health = base.health;
			mana = base.mana;
			side = base.side;
			ennemy_in = base.ennemy_in;
			return *this;
		}
};

class Entity {
	public:
		Entity() : _id(0), _type(0), _x(0), _y(0), _shield_life(0),
		_is_controlled(0), _health(0), _vx(0), _vy(0),
		_near_base(0), _threat_for(0) {};

		Entity(int id, int type, int x, int y, int shield_life, int is_controlled,
				int health, int vx, int vy, int near_base, int threat_for)
			: _id(id), _type(type), _x(x), _y(y), _shield_life(shield_life),
			_is_controlled(is_controlled), _health(health), _vx(vx), _vy(vy),
			_near_base(near_base), _threat_for(threat_for) {};

		Entity(const Entity &entity) {
			*this = entity;
		};

		struct s_target target;
		int	dist_from_base;
		int	dist_from_ennemy_base;

		void setTarget(struct s_target target) {
			this->target.id = target.id;
			this->target.pos_x = target.pos_x;
			this->target.pos_y = target.pos_y;
			this->target.score_attack = target.score_attack;
			this->target.score_wind = target.score_wind;
			this->target.score_control = target.score_control;
			this->target.score_shield = target.score_shield;
			this->target.score_wait = target.score_wait;;
		};

		void	resetTarget() {
			this->target.id = 0;
			this->target.pos_x = 0;
			this->target.pos_y = 0;
			this->target.score_attack = 0;
			this->target.score_wind = 0;
			this->target.score_control = 0;
			this->target.score_shield = 0;
			this->target.score_wait = 0;
		};
		int getId() const { return _id; };
		int getType() const { return _type; };
		int getX() const { return _x; };
		int getY() const { return _y; };
		int getShieldLife() const { return _shield_life; };
		int getIsControlled() const { return _is_controlled; };
		int getHealth() const { return _health; };
		int getVx() const { return _vx; };
		int getVy() const { return _vy; };
		int getNearBase() const { return _near_base; };
		int getThreatFor() const { return _threat_for; };
		void setId(int id) { _id = id; };
		void setType(int type) { _type = type; };
		void setX(int x) { _x = x; };
		void setY(int y) { _y = y; };
		void setShieldLife(int shield_life) { _shield_life = shield_life; };
		void setIsControlled(int is_controlled) { _is_controlled = is_controlled; };
		void setHealth(int health) { _health = health; };
		void setVx(int vx) { _vx = vx; };
		void setVy(int vy) { _vy = vy; };
		void setNearBase(int near_base) { _near_base = near_base; };
		void setThreatFor(int threat_for) { _threat_for = threat_for; };

		int offSet(int a, int b) { return a - b; };

		void    resetAllScore() {
			target.score_attack = 0;
			target.score_wind = 0;
			target.score_control = 0;
			target.score_shield = 0;
			target.score_wait = 0;
		}

		int	getDistFromPoint(int offset_x, int offset_y) const {
			return round(sqrt(((offset_x - this->getX()) * (offset_x - this->getX()))
						+ ((offset_y - this->getY()) * (offset_y - this->getY()))));
		};

		int	getDist() const {
			return round(sqrt(((this->getX() - this->target.pos_x) * (this->getX() - this->target.pos_x))
						+ ((this->getY() - this->target.pos_y) * (this->getY() - this->target.pos_y))));
		};

		void    displayScore() {
			cerr << "ID = " << this->target.id << endl;
			cerr << "ATTACK = " << this->target.score_attack << endl;
			cerr << "WIND = " << this->target.score_wind << endl;
			cerr << "CONTROL = " << this->target.score_control << endl;
			cerr << "SHIELD = " << this->target.score_shield << endl;
		};

		void    displayTarget() {
			cerr << "ID = " << this->target.id << endl;
			cerr << "pos_x = " << this->target.pos_x << endl;
			cerr << "pos_y = " << this->target.pos_y << endl;
			cerr << "ATTACK = " << this->target.score_attack << endl;
			cerr << "WIND = " << this->target.score_wind << endl;
			cerr << "CONTROL = " << this->target.score_control << endl;
			cerr << "SHIELD = " << this->target.score_shield << endl;
		};

		Entity &operator=(const Entity &entity) {
			_id = entity._id;
			_type = entity._type;
			_x = entity._x;
			_y = entity._y;
			_shield_life = entity._shield_life;
			_is_controlled = entity._is_controlled;
			_health = entity._health;
			_vx = entity._vx;
			_vy = entity._vy;
			_near_base = entity._near_base;
			_threat_for = entity._threat_for;
			target.id = entity.target.id;
			target.pos_x = entity.target.pos_x;
			target.pos_y = entity.target.pos_y;
			target.score_wind = entity.target.score_wind;
			target.score_attack = entity.target.score_attack;
			target.score_control = entity.target.score_control;
			target.score_shield = entity.target.score_shield;
			target.score_wait = entity.target.score_wait;
			dist_from_base = entity.dist_from_base;
			dist_from_ennemy_base = entity.dist_from_ennemy_base;

			return *this;
		};

	private:
		int _id;
		int _type;
		int _x;
		int _y;
		int _shield_life;
		int _is_controlled;
		int _health;
		int _vx;
		int _vy;
		int _near_base;
		int _threat_for;
};

class Monster : public Entity {
	public:
		Monster() : Entity(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0){};

		Monster(int id, int type, int x, int y, int shield_life, int is_controlled,
				int health, int vx, int vy, int near_base, int threat_for)
			: Entity(id, type, x, y, shield_life, is_controlled, health, vx, vy,
					near_base, threat_for) {
				this->resetTarget();
				dist_from_base = 0;
				dist_from_ennemy_base = 0;
			};

		Monster(int x, int y) : Entity() {
			this->setX(x);
			this->setY(y);
		};

		Monster(const Monster &monster) : Entity(monster) {
			*this = monster;
		};

		void upScoreAttack(int score) { this->target.score_attack += score; };
		void upScoreWind(int score) { this->target.score_wind += score; };
		void upScoreControl(int score) { this->target.score_control += score; };
		void upScoreShield(int score) { this->target.score_shield += score; };
		void upScoreWait(int score) { this->target.score_wait += score; };

		void resetScoreAttack() { this->target.score_attack = 0; };
		void resetScoreWind() { this->target.score_wind = 0; };
		void resetScoreControl() { this->target.score_control = 0; };
		void resetScoreShield() { this->target.score_shield = 0; };
		void resetScoreWait() { this->target.score_wait = 0; };

		void upAllScore(int score) {
			this->upScoreAttack(score);
			this->upScoreControl(score);
			this->upScoreWind(score);
			this->upScoreShield(score);
		};

		Monster &operator=(const Monster &entity) {
			this->setId(entity.getId());
			this->setType(entity.getType());
			this->setX(entity.getX());
			this->setY(entity.getY());
			this->setShieldLife(entity.getShieldLife());
			this->setIsControlled(entity.getIsControlled());
			this->setHealth(entity.getHealth());
			this->setVx(entity.getVx());
			this->setVy(entity.getVy());
			this->setNearBase(entity.getNearBase());
			this->setThreatFor(entity.getThreatFor());
			this->target.id = entity.target.id;
			this->target.pos_x = entity.target.pos_x;
			this->target.pos_y = entity.target.pos_y;
			this->target.score_attack = entity.target.score_attack;
			this->target.score_wind = entity.target.score_wind;
			this->target.score_control = entity.target.score_control;
			this->target.score_shield = entity.target.score_shield;
			this->target.score_wait = entity.target.score_wait;
			dist_from_base = entity.dist_from_base;
			dist_from_ennemy_base = entity.dist_from_ennemy_base;

			return *this;
		};
};

ostream &operator<<(ostream &os, const Monster &entity) {
	os << "id :" << entity.getId() << '\n'
		<< "type :" << entity.getType() << '\n'
		<< "x :" << entity.getX() << '\n'
		<< "y :" << entity.getY() << '\n'
		<< "shield :" << entity.getShieldLife() << '\n'
		<< "controlled :" << entity.getIsControlled() << '\n'
		<< "health :" << entity.getHealth() << '\n'
		<< "vx :" << entity.getVx() << '\n'
		<< "vy :" << entity.getVy() << '\n'
		<< "target id:" << entity.target.id << '\n'
		<< "target x:" << entity.target.pos_x  << '\n'
		<< "target y:" << entity.target.pos_y << '\n'
		<< "target attack:" << entity.target.score_attack << '\n'
		<< "target wind:" << entity.target.score_wind << '\n'
		<< "target control:" << entity.target.score_control << '\n'
		<< "target shield:" << entity.target.score_shield << '\n'
		<< "target wait:" << entity.target.score_wait << '\n'
		<< "dist from base :" << entity.dist_from_base << '\n'
		<< "dist from ennemy base :" << entity.dist_from_ennemy_base << '\n';
	return os;
}

class Hero : public Entity {
	public:
		Hero() : Entity(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0){
			this->target.id = 0;
			this->target.pos_x = 0;
			this->target.pos_y = 0;
			this->target.score_attack = 0;
			this->target.score_wind = 0;
			this->target.score_shield = 0;
			this->target.score_control = 0;
			this->target.score_wait = 0;
			Base tmp;
			vector<Monster> vec;

			this->checkpoint = vec;
			this->base = tmp;
			name = "";
			dist_from_base = 0;
			dist_from_ennemy_base = 0;
		};

		Hero(int id, int type, int x, int y, int shield_life, int is_controlled,
				int health, int vx, int vy, int near_base, int threat_for)
			: Entity(id, type, x, y, shield_life, is_controlled, health, vx, vy,
					near_base, threat_for) {
				this->target.id = 0;
				this->target.pos_x = 0;
				this->target.pos_y = 0;
				this->target.score_attack = 0;
				this->target.score_wind = 0;
				this->target.score_shield = 0;
				this->target.score_control = 0;
				this->target.score_wait = 0;
				name = "";
				Base tmp;
				vector<Monster> checkpoint;
				this->base = tmp;
				dist_from_base = 0;
				dist_from_ennemy_base = 0;
			};

		Hero(const Hero &hero) {
			*this = hero;
		}
		string name;
		Base base;
		vector<Monster>	checkpoint;

		void setName(int i) {
			switch (i) {
				case GANDOULF:
					name = "GANDOULF";
					break;
				case GIMLI:
					name = "GIMLI";
					break;
				case ARAGORN:
					name = "ARAGORN";
					break;
			}
		}
		void move() const {
			cout << "MOVE " << this->target.pos_x << " " << this->target.pos_y << " " << name << endl;
		}
		void wind(Base &base) const {
			base.mana -= 10;
			cout << "SPELL WIND " << XMAX - this->base.x << " " << YMAX - this->base.y <<  " " << name << endl;
		}
		void shield(int id, Base &base) const {
			base.mana -= 10;
			cout << "SPELL SHIELD " << id << " " << name << " Boubou" << endl;
		}
		void control(int id, Base &base) const {
			base.mana -= 10;
			cout << "SPELL CONTROL " << id << " " << this->target.pos_x << " " << this->target.pos_y <<  " " << name << endl;
		}
		void wait() const { cout << "WAIT " << "Repos" << endl; };

		int	bestAttackScore(vector<Monster> monsters) {
			vector<int>	vect;

			for (int i = 0; i < monsters.size(); i++) {
				vect.push_back(monsters[i].target.score_attack);
			}
			sort(vect.begin(), vect.end());
			return  vect[vect.size() - 1];
		}

		int	bestWindScore(vector<Monster> monsters) {
			vector<int>	vect;

			for (int i = 0; i < monsters.size(); i++) {
				vect.push_back(monsters[i].target.score_wind);
			}
			sort(vect.begin(), vect.end());
			return  vect[vect.size() - 1];
		}

		int	bestShieldScore(vector<Monster> monsters) {
			vector<int>	vect;

			for (int i = 0; i < monsters.size(); i++) {
				vect.push_back(monsters[i].target.score_shield);
			}
			sort(vect.begin(), vect.end());
			return  vect[vect.size() - 1];
		}

		int	bestControlScore(vector<Monster> monsters) {
			vector<int>	vect;

			for (int i = 0; i < monsters.size(); i++) {
				vect.push_back(monsters[i].target.score_control);
			}
			sort(vect.begin(), vect.end());
			return  vect[vect.size() - 1];
		}

		void	choseAction(string str, struct s_target target, Base &base) {
			this->setTarget(target);
			cerr << "TARGET ID =" << this->target.id << endl;
			if (str == "attack") {
				this->move();
			} else if (str == "wind") {
				this->wind(base);
			} else if (str == "control") {
				this->control(target.id, base);
			} else if (str == "shield") {
				this->shield(target.id, base);
			} else {
				this->wait();
			}
		}

		struct s_target getTarget(vector<Monster> &monsters, string type_action, int score) {
			for (int i = 0; i < monsters.size(); i++) {
				if (type_action == "attack") {
					if (monsters[i].target.score_attack == score)
						return monsters[i].target;
				} else if (type_action == "wind") {
					if (monsters[i].target.score_wind == score)
						return monsters[i].target;
				} else if (type_action == "control") {
					if (monsters[i].target.score_control == score)
						return monsters[i].target;
				} else if (type_action == "shield") {
					if (monsters[i].target.score_shield == score)
						return monsters[i].target;
				} else if (type_action == "wait") {
					if (monsters[i].target.score_wait == score)
						return monsters[i].target;
				}
			}
			return monsters[0].target;
		}

		void	choseBestAction(vector<Monster> &monsters, Base &base) {
			unordered_map<string, int> map;

			map.insert(pair<string, int> ("attack", this->bestAttackScore(monsters)));
			map.insert(pair<string, int> ("wind", this->bestWindScore(monsters)));
			map.insert(pair<string, int> ("control", this->bestControlScore(monsters)));
			map.insert(pair<string, int> ("shield", this->bestShieldScore(monsters)));
			map.insert(pair<string, int> ("wait", 0));

			pair<string, int> res = {"wait", 0};

			for (auto it = map.begin(); it != map.end() ; it++) {
				if (res.second < (*it).second) {
					res = *it;
				}
			}

			cerr << "map " << this->name << " Attack : " << map["attack"] << endl;
			cerr << "map " << this->name << " Wind : " << map["wind"] << endl;
			cerr << "map " << this->name << " Control : " << map["control"] << endl;
			cerr << "map " << this->name << " shield : " << map["shield"] << endl;
//			cerr << "map " << this->name << " wait : " << map["wait"] << endl;

			this->choseAction(res.first, this->getTarget(monsters, res.first, res.second), base);
		}

		/*
		bool	canWind(const Monster &monster) {
			if (this->canSpellTarget(monster) {
				return true;
			}
			return false;
		}
		*/

		bool	canAttack(const Monster &monster) {
			if (monster.getThreatFor() != 2) {
				return true;
			}
			return false;
		}

		bool	canShield(const Monster &monster) {
			if (this->canSpellTarget(monster)) {
				if (monster.getThreatFor() != 0 && monster.getHealth() > 12)
					return true;
			}
			return false;
		}

		/*
		bool	canControl(const Monster &monster) {
			if (this->canSpellTarget(monster) {
				return true;
			}
			return false;
		}
		*/

		bool	canSpellTarget(const Monster &monster) {
			if (this->base.getMana() >= 10
					&& monster.getDistFromPoint(this->getX(), this->getY()) < 1280
					&& monster.getShieldLife() == 0
					&& g_turn >= SPELL)
				return true;
			return false;
		}

		int shortest(vector<Monster> &monsters, Base base, int dist) {
			int short_dist_tmp;
			int shortest = 0;
			int short_dist = this->getDistFromPoint(monsters[0].getX(), monsters[0].getY());
			for (int j = 0; j < monsters.size(); j++) {
				short_dist_tmp = this->getDistFromPoint(monsters[j].getX(), monsters[j].getY());
				if (short_dist < short_dist_tmp && monsters[j].getDistFromPoint(base.x, base.y) < dist) {
					shortest = j;
				}
			}
			return shortest;
		}

		void	gandoulfAction(vector<Monster> &monsters, vector<Hero> &hero, vector<Hero> &op_hero, vector<Base> &base) {
			int start_x = 3500;
			int start_y = 5700;
			int nbr_closes = 0;
			int closest = 0;
			int monster_in_base = 0;

			if (monsters.size() > 0) {
				closest = monsters[0].dist_from_base;
				for (int j = 0; j < monsters.size(); j++) {
					if (monsters[j].getThreatFor() == 1) {
						monsters[j].upScoreAttack(1);
					}
					if (monsters[j].getNearBase() == 1) {
						if (monsters[j].dist_from_base > 4800 && monsters[j].dist_from_base < 5000) {
							if (this->canSpellTarget(monsters[j]))
								monsters[j].upScoreControl(1);
						}
						if (monsters[j].getShieldLife() > 0) {
							monsters[j].upScoreAttack(1);
						}
						if (this->canSpellTarget(monsters[j]) && this->base.getMana() > 30)
							monsters[j].upScoreWind(1);
						monsters[j].upScoreAttack(1);
						monster_in_base++;
					}
					if (monsters[j].dist_from_base > 5000 && monsters[j].dist_from_base < 10000) {
						monsters[j].upScoreAttack(1);
						if (monsters[j].getThreatFor() == 1) {
							monsters[j].upScoreAttack(1);
						}
						if (monsters[j].dist_from_base <= 3000)
							nbr_closes++;
						if (monsters[j].dist_from_base < closest) {
							closest = monsters[j].dist_from_base;
						}
					}
				}
				//Attack nearest
				for (int j = 0; j < monsters.size(); j++) {
					if (monsters[j].getDistFromPoint(this->getX(), this->getY()) < 2000)
						monsters[j].upScoreAttack(1);
				}
				//go back if too much monster in base
				if (monster_in_base >= 4) {
					for (int j = 0; j < monsters.size(); j++) {
						if (monsters[j].getNearBase() == 1) {
							if (this->canSpellTarget(monsters[j])) {
								monsters[j].upScoreWind(1);
							}
							monsters[j].upScoreAttack(1);
						}
						if (closest < 3000) {
							for (int j = 0; j < monsters.size(); j++) {
								if (monsters[j].dist_from_base == closest) {
									if (this->canSpellTarget(monsters[j]))
										monsters[j].upScoreWind(3);
									monsters[j].upScoreAttack(0);
								}
							}
						}
					}
				}
				if (nbr_closes > 5) {
					for (int j = 0; j < monsters.size(); j++) {
						if (monsters[j].dist_from_base <= 5000) {
							if (this->canSpellTarget(monsters[j]))
								monsters[j].upScoreWind(3);
							monsters[j].upScoreAttack(1);
						}
					}
				}
				//go defend on the closest
				for (int k = 0; k < monsters.size(); k++) {
					for (int j  = 0; j < hero.size(); j++) {
						if (j == GIMLI) {
							if (hero[j].target.id == monsters[k].target.id
								&& hero[j].dist_from_base > 1000)
								if (monsters[k].target.score_attack > 0)
									monsters[k].upScoreAttack(-1);
						}
					}
				}
				int no_action = 0;
				for (int j = 0; j < monsters.size(); j++) {
					if (monsters[j].target.score_attack != 0 || monsters[j].target.score_wind != 0
							|| monsters[j].target.score_control != 0 || monsters[j].target.score_shield != 0)
						no_action++;
				}
				if (no_action == 0){
					cerr << "NO ACTION!!!" << endl;
					Monster	tmp;

					tmp.resetTarget();
					tmp.upScoreAttack(200);
					if (g_iga == 0 && this->getDistFromPoint(this->checkpoint[g_iga].target.pos_x,
								this->checkpoint[g_iga].target.pos_y) < 1000)
						g_iga = 1;
					else if (g_iga == 1 && this->getDistFromPoint(this->checkpoint[g_iga].target.pos_x,
								this->checkpoint[g_iga].target.pos_y) < 1000)
						g_iga = 0;
					cerr << "INDEX =" << g_iga << endl;
					tmp.target.pos_x = this->checkpoint[g_iga].target.pos_x;
					tmp.target.pos_y = this->checkpoint[g_iga].target.pos_y;
					monsters.push_back(tmp);
				}
				this->choseBestAction(monsters, this->base);
			} else {
				this->target.pos_x = this->checkpoint[g_iga].target.pos_x;
				this->target.pos_y = this->checkpoint[g_iga].target.pos_y;
				this->move();
			}
		}

		void	gimliAction(vector<Monster> &monsters, vector<Hero> &hero, vector<Hero> &op_hero, vector<Base> &base) {
			int start_x = 6000;
			int start_y = 1200;
			int marge = 2000;
			int closest = 0;
			int monster_in_base = 0;

			if (monsters.size() == 0) {
				this->target.pos_x = this->base.x + (start_x * this->base.side);
				this->target.pos_y = this->base.y + (start_y * this->base.side);
				this->move();
			} else {
				closest = monsters[0].dist_from_base;
				for (int j = 0; j < monsters.size(); j++) {
					if (monsters[j].getNearBase() == 1) {
						monster_in_base++;
						if (this->canSpellTarget(monsters[j]) && this->dist_from_base < 2500) {
							monsters[j].upScoreWind(2);
						}
						monsters[j].upScoreAttack(1);
					}
					if (monsters[j].dist_from_base < 5000) {
						if (monsters[j].getThreatFor() == 1 && this->dist_from_base < 1500) {
							if (this->canSpellTarget(monsters[j]) && this->dist_from_base < 6000)
								monsters[j].upScoreWind(2);
						}
						if (monsters.size() == 1)
							monsters[j].upScoreAttack(2);
						if (monsters[j].getThreatFor() == 1)
							monsters[j].upScoreAttack(1);
						if (monsters[j].getNearBase() == 1) {
							if (this->canSpellTarget(monsters[j]) && this->dist_from_base < 6000) {
								monsters[j].upScoreWind(3);
							}
							monsters[j].upScoreAttack(3);
							if (monsters[j].getShieldLife() > 0) {
								monsters[j].upScoreAttack(1);
							}
						}
					}
					if (closest > monsters[j].dist_from_base)
						closest = monsters[j].dist_from_base;
				}
				//check around
				int count = 0;
				for (int j = 0; j < monsters.size(); j++) {
					if (this->canSpellTarget(monsters[j]))
						count++;
				}
				if (count > 1) {
					for (int j = 0; j < monsters.size(); j++) {
						if (this->canSpellTarget(monsters[j]))
							monsters[j].upScoreWind(1);
					}
				}
				//force attack closest monster
				for (int j = 0; j < monsters.size(); j++) {
					if (monsters[j].dist_from_base == closest) {
						if (this->canSpellTarget(monsters[j]) && this->dist_from_base < 2000) {
							monsters[j].upScoreWind(10);
						}
						monsters[j].upScoreAttack(5);
					}
				}
				if (monster_in_base >= 1) {
					for (int j = 0; j < monsters.size(); j++) {
						if (monsters[j].getNearBase() == 1) {
							if (this->canSpellTarget(monsters[j]) && this->dist_from_base < 6000) {
								monsters[j].upScoreWind(1);
							}
							monsters[j].upScoreAttack(1);
						}
					}
				}
				//Protect Defender SHIEEEEEEEEEEEEELDDDDDD

				if (isEnnemyIn(op_hero, this->base) && base[1].getMana() > 30) {
					int shield_my_friend = 0;
					for (int j = 0; j < op_hero.size(); j++) {
						if (op_hero[j].getDistFromPoint(this->getX(), this->getY()) < 1280
							&& op_hero[j].dist_from_base < 6000)
							shield_my_friend = 1;
					}
					if (shield_my_friend) {
						for (int j = 0; j < hero.size(); j++) {
							if (j == GIMLI && this->base.getMana() > 10 && this->getShieldLife() == 0
									&& this->getDistFromPoint(hero[j].getX(), hero[j].getY()) < 1280) {
								Monster tmp;

								tmp.resetTarget();
								tmp.target.id = hero[j].getId();
								tmp.target.pos_x = hero[j].getX();
								tmp.target.pos_y = hero[j].getY();
								tmp.upScoreShield(200);
								monsters.push_back(tmp);
							}
						}
					}
				}
				//Dont go to far away
				for (int j = 0; j < monsters.size(); j++) {
					if (monsters[j].dist_from_base > 6500) {
						monsters[j].resetAllScore();
					}
				}
				// check if another hero is already on target
				for (int k = 0; k < monsters.size(); k++) {
					for (int j  = 0; j < hero.size(); j++) {
						if (j != GIMLI) {
							if (hero[j].target.id == monsters[k].target.id
								&& hero[j].dist_from_base > 1000)
								if (monsters[k].target.score_attack > 0)
									monsters[k].upScoreAttack(-1);
						}
					}
				}
				if (this->bestAttackScore(monsters) == this->bestWindScore(monsters) && this->bestWindScore(monsters) != 0) {
					for (int j = 0; j < monsters.size(); j++) {
						if (this->canSpellTarget(monsters[j]))
							monsters[j].upScoreWind(1);
					}
				}
				for (int j  = 0; j < op_hero.size(); j++) {
					if (op_hero[j].dist_from_base < 6000 && monster_in_base == 0) {
						Monster tmp;

						tmp.resetTarget();
						tmp.target.pos_x = op_hero[j].getX() - (2000 * this->base.side);
						if (tmp.target.pos_x < 0)
							target.pos_x = 0;
						else if (target.pos_x > XMAX)
							target.pos_x = XMAX;
						tmp.target.pos_y = op_hero[j].getY() - (2000 * this->base.side);
						if (tmp.target.pos_y < 0)
							target.pos_y = 0;
						else if (target.pos_y > YMAX)
							target.pos_y = YMAX;
						tmp.upScoreAttack(5);
						monsters.push_back(tmp);
					}
				}
				this->choseBestAction(monsters, base[0]);
			}
		}

		void	aragornAction(vector<Monster> &monsters, vector<Hero> &hero, vector<Hero> &op_hero, vector<Base> base) {
//			int start_x = 10500;
//			int start_y = 7000;
			int near_base_ennemy = 0;

			if (monsters.size() > 0) {
				near_base_ennemy = monsters[0].dist_from_ennemy_base;
				for (int j = 0; j < monsters.size(); j++) {
					if (monsters[j].dist_from_ennemy_base < 7000 && monsters[j].getThreatFor() != 2) {
						if (this->canSpellTarget(monsters[j]) && this->base.getMana() > 50)
							monsters[j].upScoreControl(2);
					}
					if (monsters[j].dist_from_ennemy_base < 5000 && monsters[j].getThreatFor() == 2) {
						if (this->canSpellTarget(monsters[j])) {
							if (this->canShield(monsters[j]))
								monsters[j].upScoreShield(1);
							monsters[j].upScoreWind(1);
						}
					}
					if (monsters[j].getHealth() > 17 && this->canShield(monsters[j]))
						monsters[j].upScoreShield(10);
					if (monsters[j].dist_from_ennemy_base <= 8000)
						if (this->canSpellTarget(monsters[j]))
							monsters[j].upScoreWind(1);
					if (monsters[j].dist_from_ennemy_base < 7000) {
						if (monsters[j].dist_from_ennemy_base > 0 && monsters[j].dist_from_ennemy_base < 6500) {
							if (base[0].getMana() > 20 && this->canSpellTarget(monsters[j])) {
								if ((monsters[j].getHealth() / 2) * 400 - monsters[j].getDistFromPoint(base[1].x, base[1].y)
									&& this->canShield(monsters[j]))
									monsters[j].upScoreShield(10);
								monsters[j].upScoreWind(1);
							} else if (monsters[j].getShieldLife() == 0 && this->canAttack(monsters[j]))
								monsters[j].upScoreAttack(1);
						}
							if (monsters[j].getShieldLife() == 0 && this->canAttack(monsters[j]))
								monsters[j].upScoreAttack(1);
							if (base[0].getMana() > 50 && this->canSpellTarget(monsters[j])) {
								monsters[j].upScoreWind(1);
								monsters[j].upScoreControl(1);
							}
							if (near_base_ennemy > monsters[j].dist_from_ennemy_base)
								near_base_ennemy = monsters[j].dist_from_ennemy_base;
						}
						if (monsters[j].getThreatFor() == 2) {
							if (this->canSpellTarget(monsters[j]))
								monsters[j].upScoreWind(1);
						}
				}
				for (int j = 0; j < monsters.size(); j++) {
					if (monsters[j].dist_from_ennemy_base == near_base_ennemy) {
						if (this->canSpellTarget(monsters[j])) {
							if (monsters[j].getThreatFor() == 2) {
								if (this->canShield(monsters[j])
										&& (monsters[j].getHealth() / 2) * 400 - monsters[j].getDistFromPoint(base[1].x, base[1].y))
									monsters[j].upScoreShield(10);
							} else {
								monsters[j].upScoreWind(5);
							}
						}
					}
				}
				//dont use mana to early
				if (g_turn <= 70) {
					for (int j = 0; j < monsters.size(); j++) {
						monsters[j].target.score_wind = 0;
						monsters[j].target.score_control = 0;
						monsters[j].target.score_shield = 0;
					}
				}
				for (int j = 0; j < monsters.size(); j++) {
					if (monsters[j].getDistFromPoint(this->getX(), this->getY()) < 500
							&& this->canAttack(monsters[j]) && monsters[j].getShieldLife() == 0)
						monsters[j].upScoreAttack(1);
				}
				//Don't do nothing
				for (int j = 0; j < monsters.size(); j++) {
					if (monsters[j].dist_from_ennemy_base < 6000)
						monsters[j].upScoreAttack(-100);
				}
				int no_action = 0;
				for (int j = 0; j < monsters.size(); j++) {
					if (monsters[j].dist_from_ennemy_base > 6000 && monsters[j].dist_from_ennemy_base < 10000) {
						if (monsters[j].getShieldLife() == 0 && this->canAttack(monsters[j])) {
							monsters[j].upScoreAttack(1);
							no_action++;
						}
					}
				}
				if (no_action == 0){
					cerr << "NO ACTION!!!" << endl;
					Monster	tmp;

					tmp.resetTarget();
					tmp.upScoreAttack(200);
					if (g_i == 0 && this->getDistFromPoint(this->checkpoint[g_i].target.pos_x,
								this->checkpoint[g_i].target.pos_y) < 1000)
						g_i = 1;
					else if (g_i == 1 && this->getDistFromPoint(this->checkpoint[g_i].target.pos_x,
								this->checkpoint[g_i].target.pos_y) < 1000)
						g_i = 0;
					cerr << "INDEX =" << g_i << endl;
					tmp.target.pos_x = this->checkpoint[g_i].target.pos_x;
					tmp.target.pos_y = this->checkpoint[g_i].target.pos_y;
					monsters.push_back(tmp);
				}
				this->choseBestAction(monsters, base[0]);
			} else {
				this->target.pos_x = this->checkpoint[0].target.pos_x;
				this->target.pos_y = this->checkpoint[0].target.pos_y;
				this->move();
			}
		}

		Hero &operator=(const Hero &entity) {
			this->setId(entity.getId());
			this->setType(entity.getType());
			this->setX(entity.getX());
			this->setY(entity.getY());
			this->setShieldLife(entity.getShieldLife());
			this->setIsControlled(entity.getIsControlled());
			this->setHealth(entity.getHealth());
			this->setVx(entity.getVx());
			this->setVy(entity.getVy());
			this->setNearBase(entity.getNearBase());
			this->setThreatFor(entity.getThreatFor());
			this->target.id = entity.target.id;
			this->target.pos_x = entity.target.pos_x;
			this->target.pos_y = entity.target.pos_y;
			this->target.score_attack = entity.target.score_attack;
			this->target.score_wind = entity.target.score_wind;
			this->target.score_shield = entity.target.score_shield;
			this->target.score_control = entity.target.score_control;
			this->target.score_wait = entity.target.score_wait;
			this->base = entity.base;
			name = entity.name;
			dist_from_base = entity.dist_from_base;
			dist_from_ennemy_base = entity.dist_from_ennemy_base;
			checkpoint = entity.checkpoint;

			return *this;
		}
};

void	resetScoreTarget(vector<Monster> &entity) {
	for (int i = 0; i < entity.size(); i++) {
		entity[i].resetAllScore();
	}
}

void	resetScoreTarget(vector<Hero> &entity) {
	for (int i = 0; i < entity.size(); i++) {
		entity[i].resetAllScore();
	}
}

Monster	&monsterClosest(vector<Monster> &monster) {
	Monster	tmp;
	int		res = 0;

	tmp = monster[0];
	for (int i = 0; i < monster.size(); i++) {
		if (tmp.dist_from_base > monster[i].dist_from_base)
			tmp = monster[i];
	}
	return monster[res];
}

bool isEnnemyIn(vector<Hero> &hero, Base &base) {
	for (int i = 0; i < hero.size(); i++) {
		if (hero[i].dist_from_base < 6000)
			return true;
	}
	return false;
}

int main() {
	int base_x; // The corner of the map representing your base
	int base_y;
	cin >> base_x >> base_y;
	cin.ignore();
	int heroes_per_player; // Always 3
	cin >> heroes_per_player;
	cin.ignore();
	srand(time(NULL));

	int side = -1;
	if (!base_x)
		side = 1;
	int offset_x = base_x * side;
	int offset_y = base_y * side;

	vector<Monster> monsters;
	vector<Hero> my_heroes;
	vector<Hero> opp_heroes;
	vector<Base> base;

	// game loop
	while (1) {
		g_turn++;
		for (int i = 0; i < 2; i++) {
			int health; // Your base health
			int mana;   // Ignore in the first league; Spend ten mana to cast a spell
			cin >> health >> mana;
			cin.ignore();
			Base tmp(health, mana, base_x, base_y, side);
			if (i == 1) {
				if (base_x == 0) {
					tmp.x = XMAX;
					tmp.y = YMAX;
					tmp.side = -1;
				} else {
					tmp.x = 0;
					tmp.y = 0;
					tmp.side = 1;
				}
			}
			base.push_back(tmp);
		}
		int entity_count; // Amount of heros and monsters you can see
		cin >> entity_count;
		cin.ignore();
		int name_index = 0;
		for (int i = 0; i < entity_count; i++) {
			int id;   // Unique identifier
			int type; // 0=monster, 1=your hero, 2=opponent hero
			int x;    // Position of this entity
			int y;
			int shield_life; // Ignore for this league; Count down until shield spell
			// fades
			int is_controlled; // Ignore for this league; Equals 1 when this entity is
			// under a control spell
			int health;        // Remaining health of this monster
			int vx;            // Trajectory of this monster
			int vy;
			int near_base; // 0=monster with no target yet, 1=monster targeting a base
			int threat_for; // Given this monster's trajectory, is it a threat to
			// 1=your base, 2=your opponent's base, 0=neither
			cin >> id >> type >> x >> y >> shield_life >> is_controlled >> health >>
				vx >> vy >> near_base >> threat_for;
			cin.ignore();

			if (type == TYPE_MONSTER) {
				Monster monster(id, type, x, y, shield_life, is_controlled, health, vx,
						vy, near_base, threat_for);
				monster.target.id = id;
				monster.target.pos_x = x;
				monster.target.pos_y = y;
				monster.dist_from_base = monster.getDistFromPoint(base_x, base_y);
				monster.dist_from_ennemy_base = monster.getDistFromPoint(base[1].x, base[1].y);
				monsters.push_back(monster);
			} else if (type == TYPE_MY_HERO) {
				Hero myhero(id, type, x, y, shield_life, is_controlled, health, vx, vy,
						near_base, threat_for);
				myhero.target.id = id;
				myhero.target.pos_x = x;
				myhero.target.pos_y = y;
				myhero.base = base[0];
				myhero.dist_from_base = myhero.getDistFromPoint(base_x, base_y);
				myhero.dist_from_ennemy_base = myhero.getDistFromPoint(base[1].x, base[1].y);
				myhero.setName(name_index);
				name_index++;
				my_heroes.push_back(myhero);
			} else if (type == TYPE_OP_HERO) {
				Hero opphero(id, type, x, y, shield_life, is_controlled, health, vx, vy,
						near_base, threat_for);
				opphero.target.id = id;
				opphero.target.pos_x = x;
				opphero.target.pos_y = y;
				opphero.base = base[1];
				opphero.dist_from_base = opphero.getDistFromPoint(base_x, base_y);
				opphero.dist_from_ennemy_base = opphero.getDistFromPoint(base[1].x, base[1].y);
				opp_heroes.push_back(opphero);
			}
		}
		Hero hero;
		vector<Monster>	save;
		for (int i = 0; i < monsters.size(); i++) {
			save.push_back(monsters[i]);
		}
		for (int i = 0; i < heroes_per_player; i++) {
			// In the first league: MOVE <x> <y> | WAIT; In later leagues: | SPELL <spellParams>;
			if (i == GANDOULF) {
				hero = my_heroes[GANDOULF];
				Monster	tmp;
				tmp.target.pos_x = base_x + 3000 * side;
				tmp.target.pos_y = base_y + 7000 * side;
				hero.checkpoint.push_back(tmp);
				tmp.target.pos_x = base_x + 8600 * side;
				tmp.target.pos_y = base_y + 1000 * side;
				hero.checkpoint.push_back(tmp);
				hero.gandoulfAction(monsters, my_heroes, opp_heroes, base);
			} else if (i == GIMLI) {
				hero = my_heroes[GIMLI];
				hero.gimliAction(monsters, my_heroes, opp_heroes, base);
			} else if (i == ARAGORN) {
				hero = my_heroes[ARAGORN];
				Monster	tmp;
				tmp.target.pos_x = base_x + 10500 * side;
				tmp.target.pos_y = base_y + 7000 * side;
				hero.checkpoint.push_back(tmp);
				tmp.target.pos_x = base_x + 16000 * side;
				tmp.target.pos_y = base_y + 2500 * side;
				hero.checkpoint.push_back(tmp);
				hero.aragornAction(monsters, my_heroes, opp_heroes, base);
			}
			if (monsters.size() > 0) {
				resetScoreTarget(monsters);
			}
			monsters.clear();
			for (int j = 0; j < save.size(); j++) {
				monsters.push_back(save[j]);
			}
		}
		monsters.clear();
		my_heroes.clear();
		opp_heroes.clear();
		base.clear();
	}
	return 0;
}
