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
#include <set>

using namespace std;

#define STEPSPELL 50
#define ATTACKGANDOULF 50
#define MANABIGPUSH 120
#define ATTACK 40
#define SPELL 30
#define XMAX 17630
#define YMAX 9000

class	Entity;
class	Monster;
class	Hero;
class	Base;

static int	g_turn = 0;
static int	g_big_push = 0;
static int	g_i = 0;
static int	g_iga = 0;
static int	g_igi = 0;
static int	g_ara_wind = 0;
static int	g_goal = 0;
static int	g_preshot = 0;
vector<int> alreadyControl;
int			goShield = 0;

int		monsterClosest(vector<Monster> &monster);
bool	isEnnemyIn(vector<Hero> &hero, Base &base);
void	isPack(vector<Monster> &monsters, int dist);
int		biggestPack(vector<Monster> &monsters, int dist);
int		checkNoAction(vector<Monster> &monsters);
void	canWindDepthOne(vector<Monster> &monsters, vector<Monster> &dst, vector<Base> &base);
void	canControlDepthOne(vector<Monster> &monsters, vector<Monster> &dst, vector<Base> &base);

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

static struct s_target	g_old_target = {0,0,0,0,0,0,0,0};

class Base {
	public:
		int	x;
		int	y;
		int	health;
		int	mana;
		int	side;
		int	ennemy_in;
		int op_hero_in;

		Base() : health(0), mana(0), x(0), y(0), side(0), ennemy_in(0), op_hero_in(0)
	{
	};

		Base(int health, int mana, int x, int y, int side) : health(health), mana(mana), x(x), y(y), side(side), ennemy_in(0), op_hero_in(0) {};

		Base(const Base &base) {
			*this = base;
		};

		int	getHealth() const { return health; }
		int	getMana() const { return mana; }

		Base    &operator=(const Base &base) {
			x = base.x;
			y = base.y;
			health = base.health;
			mana = base.mana;
			side = base.side;
			ennemy_in = base.ennemy_in;
			op_hero_in = base.op_hero_in;
			return *this;
		}
};

class Entity {
	public:
		Base base;
		struct s_target target;
		int	dist_from_base;
		int	dist_from_ennemy_base;
		int	nbr_arround;
		int menace;

		Entity() : _id(0), _type(0), _x(0), _y(0), _shield_life(0),
		_is_controlled(0), _health(0), _vx(0), _vy(0),
		_near_base(0), _threat_for(0) {
			dist_from_base = 0;
			dist_from_ennemy_base = 0;
			nbr_arround = 0;
			menace = 0;
			Base tmp;
			this->base = tmp;
		};

		Entity(int id, int type, int x, int y, int shield_life, int is_controlled,
				int health, int vx, int vy, int near_base, int threat_for)
			: _id(id), _type(type), _x(x), _y(y), _shield_life(shield_life),
			_is_controlled(is_controlled), _health(health), _vx(vx), _vy(vy),
			_near_base(near_base), _threat_for(threat_for) {
				dist_from_base = 0;
				dist_from_ennemy_base = 0;
				nbr_arround = 0;
				menace = 0;
				Base tmp;
				this->base = tmp;
			};

		Entity(const Entity &entity) {
			*this = entity;
		};

		void setTarget(struct s_target target) {
			this->target.id = target.id;
			this->target.pos_x = target.pos_x;
			this->target.pos_y = target.pos_y;
			this->target.score_attack = target.score_attack;
			this->target.score_wind = target.score_wind;
			this->target.score_control = target.score_control;
			this->target.score_shield = target.score_shield;
			this->target.score_wait = target.score_wait;
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
			nbr_arround = entity.nbr_arround;
			base = entity.base;
			menace = entity.menace;

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
				nbr_arround = 0;
				menace = 0;
			};

		Monster(int x, int y) : Entity() {
			this->setX(x);
			this->setY(y);
		};

		Monster(const Monster &monster) : Entity(monster) {
			*this = monster;
		};

		bool    PotentialRisk(Hero &hero) {
			return ((this->getHealth() * 200) > (this->dist_from_base - 1600));
		}

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

		void    resetNbrArround(vector<Monster> &monsters) {
			for (int j = 0; j < monsters.size(); j++) {
				monsters[j].nbr_arround = 0;
			}
		}

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
			dist_from_ennemy_base = entity.dist_from_ennemy_base;
			base = entity.base;

			return *this;
		};
};

ostream &operator<<(ostream &os, const Monster &entity) {
	os << "id :" << entity.getId() << '\n'
		//		<< "type :" << entity.getType() << '\n'
				<< "x :" << entity.getX() << '\n'
				<< "y :" << entity.getY() << '\n'
		//		<< "shield :" << entity.getShieldLife() << '\n'
		//		<< "controlled :" << entity.getIsControlled() << '\n'
		//		<< "health :" << entity.getHealth() << '\n'
//				<< "vx :" << entity.getVx() << '\n'
//				<< "vy :" << entity.getVy() << '\n'
		<< "target id:" << entity.target.id << '\n'
		//		<< "target x:" << entity.target.pos_x  << '\n'
		//		<< "target y:" << entity.target.pos_y << '\n'
		<< "target attack:" << entity.target.score_attack << '\n'
		<< "target wind:" << entity.target.score_wind << '\n'
		<< "target control:" << entity.target.score_control << '\n'
		<< "target shield:" << entity.target.score_shield << '\n'
		//		<< "target wait:" << entity.target.score_wait << '\n'
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
			vector<Monster> vec;

			this->checkpoint = vec;
			name = "";
			dist_from_base = 0;
			dist_from_ennemy_base = 0;
			nbr_arround = 0;
			menace = 0;
			hp1280 = 0;
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
				vector<Monster> checkpoint;
				dist_from_base = 0;
				dist_from_ennemy_base = 0;
				nbr_arround = 0;
				menace = 0;
				hp1280 = 0;
			};

		Hero(const Hero &hero) {
			*this = hero;
		}
		string name;
		vector<Monster>	checkpoint;
		int hp1280;

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
			if (g_preshot) {
				int x = this->getX() + this->base.x + (12980 * this->base.side) - this->target.pos_x;
				int y = this->getY() + this->base.y + (9000 * this->base.side) - this->target.pos_y;
				cout << "SPELL WIND " << x << " " << y <<  " " << "Preshot!" << endl;
			} else {
				cout << "SPELL WIND " << this->base.x + (XMAX - 300) * this->base.side << " "
					<< this->base.y + (YMAX - 300) * this->base.side <<  " " << "Clac!" << endl;
			}
		}
		void shield(int id, Base &base) const {
			base.mana -= 10;
			cout << "SPELL SHIELD " << id << " " << name << " Boubou" << endl;
		}
		void control(int id, Base &base) const {
			base.mana -= 10;
			alreadyControl.push_back(id);
			cout << "SPELL CONTROL " << id << " " << this->base.x + (12991 * base.side) << " " << this->base.y + (8999 * base.side) <<  " " << name << endl;
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
			g_old_target.score_attack = target.score_attack;
			g_old_target.score_wind = target.score_wind;
			g_old_target.score_shield = target.score_shield;
			g_old_target.score_control = target.score_control;
			g_old_target.score_wait = target.score_wait;
			g_old_target.id = target.id;
			g_old_target.pos_x = target.pos_x;
			g_old_target.pos_y = target.pos_y;
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
					{
						this->target.id = monsters[i].target.id;
						return monsters[i].target;
					}
				} else if (type_action == "wind") {
					if (monsters[i].target.score_wind == score)
					{
						this->target.id = monsters[i].target.id;
						return monsters[i].target;
					}
				} else if (type_action == "control") {
					if (monsters[i].target.score_control == score)
					{
						this->target.id = monsters[i].target.id;
						return monsters[i].target;
					}
				} else if (type_action == "shield") {
					if (monsters[i].target.score_shield == score)
					{
						this->target.id = monsters[i].target.id;
						return monsters[i].target;
					}
				} else if (type_action == "wait") {
					if (monsters[i].target.score_wait == score)
					{
						this->target.id = monsters[i].target.id;
						return monsters[i].target;
					}
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
			this->choseAction(res.first, this->getTarget(monsters, res.first, res.second), base);
		}

		bool	canSpellTarget(const Monster &monster) {
			if (g_big_push == 0)
				return false;
			if (this->base.getMana() >= 10
					&& monster.getDistFromPoint(this->getX(), this->getY()) < 2200
					&& monster.getShieldLife() == 0)
				return true;
			return false;
		}

		bool	canWind(const Monster &monster) {
			if (this->name == "GIMLI" && this->base.mana < 50)
				return false;
			if (this->canSpellTarget(monster) && this->getDistFromPoint(monster.getX(), monster.getY()) < 1280) {
				return true;
			}
			return false;
		}

		bool	canAttack(const Monster &monster) {
			if (this->name == "GIMLI" && monster.dist_from_base > 6000)
				return false;
			if (monster.getThreatFor() != 2) {
				return true;
			}
			return false;
		}

		bool	canShield(const Monster &monster) {
			if (this->canSpellTarget(monster)) {
				if (monster.getThreatFor() == 2 && monster.getHealth() >= 17 && monster.dist_from_ennemy_base < 5000
						&& g_turn >= STEPSPELL)
					return true;
			}
			return false;
		}

		bool	canControl(const Monster &monster) {
			if (monster.getHealth() < 7)
				return false;
			if (this->base.mana >= 30 && this->canSpellTarget(monster) && this->getIsControlled() == 0)
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

		void	shieldGimli(vector<Hero> &op_hero, vector<Monster> &monsters) {
			Monster me;

			me.resetTarget();
			me.setX(this->getX());
			me.setY(this->getY());
			me.target.pos_x = this->getX();
			me.target.pos_y = this->getY();
			me.target.id = this->getId();
			me.setId(this->getId());
			me.setShieldLife(this->getShieldLife());
			me.base.mana = this->base.mana;
			for (int j = 0; j < op_hero.size(); j++) {
				if (this->canSpellTarget(me)) {
					me.upScoreShield(100);
					monsters.push_back(me);
				}
			}
		}

		void	shieldMe(vector<Hero> &op_hero, vector<Monster> &monsters) {
			Monster me;

			me.resetTarget();
			me.setX(this->getX());
			me.setY(this->getY());
			me.target.pos_x = this->getX();
			me.target.pos_y = this->getY();
			me.target.id = this->getId();
			me.setId(this->getId());
			me.setShieldLife(this->getShieldLife());
			me.base.mana = this->base.mana;
			for (int j = 0; j < op_hero.size(); j++) {
				if (this->canSpellTarget(me)
						&& (this->getDistFromPoint(op_hero[j].getX(), op_hero[j].getY()) < 2200 && this->getShieldLife() == 0)
						&& this->getX() != 12030 * this->base.side) {
					me.upScoreShield(100);
					monsters.push_back(me);
				}
			}
		}

		void    dontDoNothingGan(vector<Monster> &monsters) {
			int action = checkNoAction(monsters);
			if (action == 0){
				Monster	tmp;

				tmp.resetTarget();
				tmp.upScoreAttack(200);
				if (g_iga == 0 && this->getDistFromPoint(this->checkpoint[g_iga].target.pos_x,
							this->checkpoint[g_iga].target.pos_y) < 300)
					g_iga = 1;
				else if (g_iga == 1 && this->getDistFromPoint(this->checkpoint[g_iga].target.pos_x,
							this->checkpoint[g_iga].target.pos_y) < 300)
					g_iga = 0;
				tmp.target.pos_x = this->checkpoint[g_iga].target.pos_x;
				tmp.target.pos_y = this->checkpoint[g_iga].target.pos_y;
				monsters.push_back(tmp);
			}
		}

		void    dontDoNothingGim(vector<Monster> &monsters) {
			int action = checkNoAction(monsters);
			if (action == 0) {
				Monster	tmp;

				tmp.resetTarget();
				tmp.upScoreAttack(200);
				if (g_igi == 0 && this->getDistFromPoint(this->checkpoint[g_igi].target.pos_x,
							this->checkpoint[g_igi].target.pos_y) < 300)
					g_igi = 1;
				else if (g_igi == 1 && this->getDistFromPoint(this->checkpoint[g_igi].target.pos_x,
							this->checkpoint[g_igi].target.pos_y) < 300)
					g_igi = 0;
				tmp.target.pos_x = this->checkpoint[g_igi].target.pos_x;
				tmp.target.pos_y = this->checkpoint[g_igi].target.pos_y;
				monsters.push_back(tmp);
			}
		}

		void    dontDoNothingAra(vector<Monster> &monsters) {
			int action = checkNoAction(monsters);
			if (action == 0){
				Monster	tmp;

				tmp.resetTarget();
				tmp.upScoreAttack(200);
				if (g_i == 0 && this->getDistFromPoint(this->checkpoint[g_i].target.pos_x,
							this->checkpoint[g_i].target.pos_y) < 300) {
					g_i = 1;
				} else if (g_i == 1 && this->getDistFromPoint(this->checkpoint[g_i].target.pos_x,
							this->checkpoint[g_i].target.pos_y) < 300) {
					g_i = 0;
				}
				tmp.target.pos_x = this->checkpoint[g_i].target.pos_x;
				tmp.target.pos_y = this->checkpoint[g_i].target.pos_y;
				monsters.push_back(tmp);
			}
		}

		void    dontDoNothingBigPush(vector<Monster> &monsters) {
			int action = checkNoAction(monsters);
			if (action == 0){
				Monster	tmp;

				tmp.resetTarget();
				tmp.upScoreAttack(200);
				tmp.target.pos_x = this->checkpoint[0].target.pos_x;
				tmp.target.pos_y = this->checkpoint[0].target.pos_y;
				monsters.push_back(tmp);
			}
		}

		void    dontGoToFarAway(vector<Monster> monsters, int dist) {
			for (int j = 0; j < monsters.size(); j++) {
				if (monsters[j].dist_from_base > dist) {
					monsters[j].resetAllScore();
				}
			}
		}

		void    dontAttackSameTarget(vector<Monster> &monsters, vector<Hero> &hero) {
			for (int k = 0; k < monsters.size(); k++) {
				for (int j  = 0; j < hero.size(); j++) {
					if (j != GIMLI) {
						if (hero[j].target.id == monsters[k].target.id
								&& hero[j].dist_from_base > 1280 && monsters[k].nbr_arround <= 2)
							if (monsters[k].target.score_attack > 0)
								monsters[k].upScoreAttack(-100);
					}
				}
			}
		}

		int closest(vector<Monster> &monsters) {
			int closest = monsters[0].dist_from_base;
			for (int j = 0; j < monsters.size(); j++) {
				if (monsters[j].getDistFromPoint(this->getX(), this->getY()) < closest)
					closest = monsters[j].getDistFromPoint(this->getX(), this->getY());
			}
			return closest;
		}

		void	gandoulfAction(vector<Monster> &monsters, vector<Hero> &hero, vector<Hero> &op_hero, vector<Base> &base) {
			int nbr_closes = 0;

			if (monsters.size() > 0) {
				int closest = this->closest(monsters);

				for (int j = 0; j < monsters.size(); j++) {
					if (closest == monsters[j].getDistFromPoint(this->getX(), this->getY())) {
						if (this->base.side == 1 && monsters[j].getY() > 4500 && monsters[j].getX() < 7500)
							monsters[j].upScoreAttack(1);
						else if (this->base.side == -1 && monsters[j].getY() < 4500 && monsters[j].getX() > XMAX - 7500)
							monsters[j].upScoreAttack(1);
					}
				}
				this->dontDoNothingGan(monsters);
				this->choseBestAction(monsters, this->base);
			} else {
				this->target.pos_x = this->checkpoint[g_iga].target.pos_x;
				this->target.pos_y = this->checkpoint[g_iga].target.pos_y;
				this->move();
			}
		}

		void	gimliEarly(vector<Monster> &monsters, vector<Hero> &hero, vector<Hero> &op_hero, vector<Base> &base) {
			if (monsters.size() == 0) {
				this->target.pos_x = this->checkpoint[g_igi].target.pos_x;
				this->target.pos_y = this->checkpoint[g_igi].target.pos_y;
				this->move();
			} else {
				int closest = this->closest(monsters);
				int closestBase = monsterClosest(monsters);
				cerr << "Dist =" << closestBase << endl;

//				if (this->getIsControlled() == 1)
//					goShield = 1;
//				if (goShield == 1)
				this->shieldMe(op_hero, monsters);
				for (int j = 0; j < monsters.size(); j++) {
					if (monsters[j].getThreatFor() == 1) {
						if (this->base.side == 1 && monsters[j].getY() < 4500 && monsters[j].getX() < 11000)
							monsters[j].upScoreAttack(1);
						else if (this->base.side == -1 && monsters[j].getY() > 4500 && monsters[j].getX() > 4200)
							monsters[j].upScoreAttack(1);
					}
					if (closest == monsters[j].getDistFromPoint(this->getX(), this->getY())) {
						if (this->base.side == 1 && monsters[j].getY() < 4500 && monsters[j].getX() < 11000)
							monsters[j].upScoreAttack(1);
						else if (this->base.side == -1 && monsters[j].getY() > 4500 && monsters[j].getX() > 4200)
							monsters[j].upScoreAttack(1);
					}
					if (monsters[j].getNearBase() == 1) {
						if (this->canWind(monsters[j]) && monsters[j].dist_from_base < 800)
							monsters[j].upScoreWind(5);
						monsters[j].upScoreAttack(1);
					}
				}
				for (int j = 0; j < monsters.size(); j++) {
					if (monsters[j].dist_from_base == closestBase) {
						if (this->base.side == 1 && monsters[j].getY() < 4500 && monsters[j].getX() < 11000)
							monsters[j].upScoreAttack(1);
						else if (this->base.side == -1 && monsters[j].getY() > 4500 && monsters[j].getX() > 4200)
							monsters[j].upScoreAttack(1);
					}
				}
				// Counter double canon
				int count = 0;
				for (int i = 0; i < op_hero.size(); i++) {
					if (this->getDistFromPoint(op_hero[i].getX(), op_hero[i].getY()) < 1280
							&& op_hero[i].getDistFromPoint(base[0].x + 6000 * base[0].side, base[0].y + 555 * base[0].side) < 500) {
						count++;
					}
				}
				if (count == 2) {
					for (int i = 0; i < op_hero.size(); i++) {
						if (this->getDistFromPoint(op_hero[i].getX(), op_hero[i].getY()) < 1280
								&& op_hero[i].getDistFromPoint(base[0].x + 6000 * base[0].side, base[0].y + 555 * base[0].side) < 500) {
							Monster tmp;

							tmp.resetTarget();
							tmp.setX(op_hero[i].getX());
							tmp.setY(op_hero[i].getY());
							tmp.setId(op_hero[i].getId());
							tmp.target.id = op_hero[i].getId();
							tmp.target.pos_x = tmp.getX();
							tmp.target.pos_y = tmp.getY();
							tmp.upScoreWind(10);

							monsters.push_back(tmp);
						}
					}
				}
				this->dontDoNothingGim(monsters);
				this->choseBestAction(monsters, this->base);
			}
		}

		void	aragornAction(vector<Monster> &monsters, vector<Hero> &hero, vector<Hero> &op_hero, vector<Base> base) {
			int near_base_ennemy = 0;

			if (monsters.size() > 0) {
				near_base_ennemy = monsters[0].dist_from_ennemy_base;
				for (int j = 0; j < monsters.size(); j++) {
					//if (monsters[j].dist_from_ennemy_base < 7000 && monsters[j].getThreatFor() != 2) {
					//	if (this->canSpellTarget(monsters[j]) && this->base.getMana() > 50)
					//		monsters[j].upScoreControl(2);
					//}
					if (monsters[j].dist_from_ennemy_base < 5000)
						if (this->canShield(monsters[j]))
							monsters[j].upScoreShield(20);
					if (monsters[j].dist_from_ennemy_base < 5000 && monsters[j].getThreatFor() == 2) {
						if (this->canShield(monsters[j]))
							monsters[j].upScoreShield(10);
						if (this->canWind(monsters[j]) && this->dist_from_ennemy_base < 7500)
							monsters[j].upScoreWind(10);
					}
					//if (this->canShield(monsters[j]))
					//	monsters[j].upScoreShield(10);
					if (this->canWind(monsters[j]) && this->dist_from_ennemy_base < 7500 && monsters[j].getHealth() > 11)
						monsters[j].upScoreWind(1);
					if (monsters[j].dist_from_ennemy_base < 7000) {
						if (monsters[j].dist_from_ennemy_base > 0 && monsters[j].dist_from_ennemy_base < 6500) {
							if ((monsters[j].getHealth() / 2) * 400 - monsters[j].getDistFromPoint(base[1].x, base[1].y)
									&& this->canShield(monsters[j]))
								monsters[j].upScoreShield(1);
							if (this->canWind(monsters[j]) && monsters[j].getHealth() > 11)
								monsters[j].upScoreWind(1);
							else if (monsters[j].getShieldLife() == 0 && this->canAttack(monsters[j]))
								monsters[j].upScoreAttack(1);
						}
						if (monsters[j].getShieldLife() == 0 && this->canAttack(monsters[j]))
							monsters[j].upScoreAttack(1);
						if (base[0].getMana() > 50 && this->canWind(monsters[j]) && this->dist_from_ennemy_base < 2200) {
							monsters[j].upScoreWind(50);
							//monsters[j].upScoreControl(1);
						}
						if (near_base_ennemy > monsters[j].dist_from_ennemy_base)
							near_base_ennemy = monsters[j].dist_from_ennemy_base;
						if (monsters[j].getThreatFor() == 2) {
							if (this->canWind(monsters[j]) && this->dist_from_ennemy_base < 7500 && monsters[j].getHealth() > 11)
								monsters[j].upScoreWind(1);
						}
					}
				}
				for (int j = 0; j < monsters.size(); j++) {
					if (monsters[j].dist_from_ennemy_base == near_base_ennemy) {
						if (monsters[j].getThreatFor() == 2) {
							if (this->canShield(monsters[j])
									&& (monsters[j].getHealth() / 2) * 400 - monsters[j].getDistFromPoint(base[1].x, base[1].y) && this->dist_from_ennemy_base < 7500)
								monsters[j].upScoreShield(10);
						} else {
							if (this->canWind(monsters[j]) && this->dist_from_ennemy_base < 7500 && monsters[j].getHealth() > 11)
								monsters[j].upScoreWind(5);
						}
					}
				}
				for (int j = 0; j < monsters.size(); j++) {
					if (monsters[j].getDistFromPoint(this->getX(), this->getY()) < 1280) {
						if (this->canWind(monsters[j]) && this->dist_from_ennemy_base < 7500 && monsters[j].getHealth() > 11)
							monsters[j].upScoreWind(5);
						monsters[j].upScoreAttack(1);
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
					if (monsters[j].getDistFromPoint(this->getX(), this->getY()) < 2200
							&& this->canAttack(monsters[j]) && monsters[j].dist_from_ennemy_base < 9000)
						monsters[j].upScoreAttack(1);
				}
				//Don't attack in opp base
				for (int j = 0; j < monsters.size(); j++) {
					if (monsters[j].dist_from_ennemy_base < 6000)
						monsters[j].upScoreAttack(-100);
				}
				//this->shieldMe(op_hero, monsters);
				//Don't do nothing
				this->dontDoNothingAra(monsters);
				this->choseBestAction(monsters, base[0]);
			} else {
				if (g_i == 0 && this->getDistFromPoint(this->checkpoint[g_i].target.pos_x,
							this->checkpoint[g_i].target.pos_y) < 300) {
					g_i = 1;
				} else if (g_i == 1 && this->getDistFromPoint(this->checkpoint[g_i].target.pos_x,
							this->checkpoint[g_i].target.pos_y) < 300) {
					g_i = 2;
				} else if (g_i == 2 && this->getDistFromPoint(this->checkpoint[g_i].target.pos_x,
							this->checkpoint[g_i].target.pos_y) < 300) {
					g_i = 0;
				}
				this->target.pos_x = this->checkpoint[g_i].target.pos_x;
				this->target.pos_y = this->checkpoint[g_i].target.pos_y;
				this->move();
			}
		}

		void	bigPush(vector<Monster> &monsters, vector<Hero> &hero, vector<Hero> &op_hero, vector<Base> base) {
			if (monsters.size() > 0) {
				//Shield hero adverse
				//for (int i = 0; i < op_hero.size(); i++) {
				//	if (op_hero[i].getDistFromPoint(this->getX(), this->getY()) < 1680) {
				//		Monster	tmp;

				//		tmp.resetTarget();
				//		tmp.setId(op_hero[i].getId());
				//		tmp.setX(op_hero[i].getX());
				//		tmp.setY(op_hero[i].getY());
				//		tmp.target.id = op_hero[i].getId();
				//		tmp.target.pos_x = op_hero[i].getX();
				//		tmp.target.pos_y = op_hero[i].getY();
				//		if (base[0].mana >= 10 && op_hero[i].getShieldLife() == 0) {
				//			tmp.upScoreShield(60);
				//			monsters.push_back(tmp);
				//		}
				//	}
				//}
				vector<Monster> depth_one_monsters;
				canControlDepthOne(monsters, depth_one_monsters, base);
				int found = 0;
				for (int i = 0; i < monsters.size(); i++) {

					int dist = monsters[i].getDistFromPoint(base[0].x + (12991 * base[0].side), base[0].y + (8999 * base[0].side));
					int dist_futur = depth_one_monsters[i].getDistFromPoint(base[0].x + (12991 * base[0].side), base[0].y + (8999 * base[0].side));
					
					for (int j = 0; j < alreadyControl.size(); j++) {
						if (alreadyControl[j] == monsters[i].getId())
							found = 1;
					}
//					if (!found && dist - dist_futur < 380) {
						if (!found && this->canControl(monsters[i]))
							monsters[i].upScoreControl(5);
//						}
//					}
					found = 0;
				}
//				this->shieldMe(op_hero, monsters);
				if (!g_preshot) {
					for (int i = 0; i < monsters.size(); i++) {
//						if (this->canControl(monsters[i]) && monsters[i].getDistFromPoint(this->checkpoint[0].target.pos_x + (2200 * base[0].side),
//									this->checkpoint[0].target.pos_y + (2200 * base[0].side)) > 1280 && monsters[i].getThreatFor() != 2)
//							monsters[i].upScoreControl(6);
						//DOUBLE WIND
						if (this->canWind(monsters[i]) && monsters[i].getDistFromPoint(this->checkpoint[0].target.pos_x + (2200 * base[0].side),
									this->checkpoint[0].target.pos_y + (2200 * base[0].side) < 1280)
									&& hero[1].getX() == base[0].x + 12030 * base[0].side
									&& hero[2].getX() == base[0].x + 12030 * base[0].side
									&& monsters[i].dist_from_ennemy_base < 4700) {
								monsters[i].upScoreWind(10);
						}
					}
				}

				//Preshot double wind
				depth_one_monsters.clear();
				canWindDepthOne(monsters, depth_one_monsters, base);
				if (base[0].getMana() > 29) {
					if (this->name == "GANDOULF" && hero[1].getX() == base[0].x + 12030 * base[0].side
							&& this->getDistFromPoint(hero[2].getX(), hero[2].getY()) < 800) {
						for (int i = 0; i < monsters.size(); i++) {
							if (monsters[i].getDistFromPoint(this->getX(), this->getY()) < 1280) {
								if (depth_one_monsters[i].getDistFromPoint(this->getX(), this->getY()) < 800) {
									cerr << monsters[i] << endl;
									monsters[i].upScoreWind(50);
									g_preshot = 1;
								}
							}
						}
					}
					if (g_preshot == 1 && this->name == "ARAGORN") {
						for (int i = 0; i < monsters.size(); i++) {
							monsters[i].target.score_wind = 0;
						}
					}
				}
				if (this->name == "ARAGORN") {
					int best_control = monsters[0].target.score_control;

					for (int i = 0; i < monsters.size(); i++) {
						if (best_control < monsters[i].target.score_control) {
							best_control = monsters[i].target.score_control;
						}
					}
					for (int i = 0; i < monsters.size(); i++) {
						if (best_control == monsters[i].target.score_control) {
							if (monsters[i].getId() == hero[1].target.id) {
								monsters[i].upScoreControl(-200);
							}
						}
					}
				}
				this->dontDoNothingBigPush(monsters);
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
			name = entity.name;
			dist_from_base = entity.dist_from_base;
			dist_from_ennemy_base = entity.dist_from_ennemy_base;
			checkpoint = entity.checkpoint;
			base = entity.base;
			nbr_arround = 0;
			menace = 0;

			return *this;
		}
};

void	resetScoreTarget(vector<Monster> &entity) {
	for (int i = 0; i < entity.size(); i++) {
		entity[i].resetAllScore();
		entity[i].nbr_arround = 0;
	}
}

void	resetScoreTarget(vector<Hero> &entity) {
	for (int i = 0; i < entity.size(); i++) {
		entity[i].resetAllScore();
		entity[i].nbr_arround = 0;
	}
}

int checkNoAction(vector<Monster> &monsters) {
	int action = 0;
	for (int j = 0; j < monsters.size(); j++) {
		if (monsters[j].target.score_attack > 0
				|| monsters[j].target.score_wind > 0
				|| monsters[j].target.score_control > 0
				|| monsters[j].target.score_shield > 0)
			action++;
	}
	return action;
}

int monsterClosest(vector<Monster> &monster) {
	Monster	tmp;
	int		res = 0;

	tmp = monster[0];
	for (int i = 0; i < monster.size(); i++) {
		if (tmp.dist_from_base > monster[i].dist_from_base)
			tmp.dist_from_base = monster[i].dist_from_base;
	}
	return tmp.dist_from_base;
}

bool isEnnemyIn(vector<Hero> &hero, Base &base) {
	for (int i = 0; i < hero.size(); i++) {
		if (hero[i].dist_from_base < 6000)
			return true;
	}
	return false;
}

void    countEnnemyIn(vector<Monster> &monsters, Base &base) {
	for (int j = 0; j < monsters.size(); j++) {
		if (monsters[j].getNearBase() == 1) {
			base.ennemy_in++;
		}
	}
}

void    countOpHeroIn(vector<Hero> &hero, Base &base) { 
	for(int i = 0; i < hero.size(); i++) {
		if (hero[i].dist_from_base < 5000)
			base.op_hero_in++;
	}
}

void    isPack(vector<Monster> &monsters, int dist) {
	for (int j = 0; j < monsters.size(); j++) {
		for (int k = 0; k < monsters.size(); k++) {
			if (monsters[j].getDistFromPoint(monsters[k].getX(), monsters[k].getY()) < dist) {
				monsters[j].nbr_arround++;
			}
		}
	}
}

int biggestPack(vector<Monster> &monsters, int dist) {
	int	pack = 0;

	isPack(monsters, dist);
	for (int j = 0; j < monsters.size(); j++) {
		if (pack < monsters[j].nbr_arround)
			pack = monsters[j].nbr_arround;
	}
	return pack;
}

void	canWindDepthOne(vector<Monster> &monsters, vector<Monster> &dst, vector<Base> &base) {
	Monster			tmp;

	for (int i = 0; i < monsters.size(); i++) {
		tmp = monsters[i];
		tmp.resetTarget();
		tmp.setX(monsters[i].getX() + (1500 * base[0].side));
		tmp.setY(monsters[i].getY() + (500 * base[0].side));
		if (tmp.getX() < 0)
			tmp.setX(0);
		else if (tmp.getX() > XMAX)
			tmp.setX(XMAX);
		if (tmp.getY() < 0)
			tmp.setY(0);
		else if (tmp.getY() > YMAX)
			tmp.setY(YMAX);
		tmp.target.id = monsters[i].getId();
		tmp.target.pos_x = tmp.getX();
		tmp.target.pos_y = tmp.getY();
		tmp.dist_from_ennemy_base = tmp.getDistFromPoint(base[1].x, base[1].y);

		dst.push_back(tmp);
	}
}

void	canControlDepthOne(vector<Monster> &monsters, vector<Monster> &dst, vector<Base> &base) {
	Monster			tmp;

	for (int i = 0; i < monsters.size(); i++) {
		tmp = monsters[i];
		tmp.resetTarget();
		tmp.setX(monsters[i].getX() + monsters[i].getVx());
		tmp.setY(monsters[i].getY() + monsters[i].getVy());
		tmp.target.id = monsters[i].getId();
		tmp.target.pos_x = tmp.getX();
		tmp.target.pos_y = tmp.getY();
		tmp.dist_from_ennemy_base = tmp.getDistFromPoint(base[1].x, base[1].y);

		dst.push_back(tmp);
	}
}

int main() {
	int base_x; // The corner of the map representing your base
	int base_y;
	cin >> base_x >> base_y;
	cin.ignore();
	int heroes_per_player; // Always 3
	cin >> heroes_per_player;
	cin.ignore();

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

		countEnnemyIn(monsters, base[0]);
		countOpHeroIn(opp_heroes, base[0]);
		if (base[0].getMana() >= MANABIGPUSH)
			g_big_push = 1;
		g_goal = 0;
		g_preshot = 0;
		for (int i = 0; i < heroes_per_player; i++) {
			// In the first league: MOVE <x> <y> | WAIT; In later leagues: | SPELL <spellParams>;
			my_heroes[i].base.ennemy_in = base[0].ennemy_in;
			Monster	tmp;
			tmp.resetTarget();
			if (i == GANDOULF) {
				if (!g_big_push) {
					tmp.target.pos_x = base_x + 3000 * side;
					tmp.target.pos_y = base_y + 8000 * side;
					my_heroes[GANDOULF].checkpoint.push_back(tmp);
					tmp.target.pos_x = base_x + 7000 * side;
					tmp.target.pos_y = base_y + 8500 * side;
					my_heroes[GANDOULF].checkpoint.push_back(tmp);
					my_heroes[GANDOULF].gandoulfAction(monsters, my_heroes, opp_heroes, base);
				} else {
					tmp.target.pos_x = base_x + 12030 * side;
					tmp.target.pos_y = base_y + 8700 * side;
					my_heroes[GANDOULF].checkpoint.push_back(tmp);
					my_heroes[GANDOULF].bigPush(monsters, my_heroes, opp_heroes, base);
				}
			} else if (i == GIMLI) {
				if (!g_big_push) {
					tmp.target.pos_x = base_x + 8000 * side;
					tmp.target.pos_y = base_y + 500 * side;
					my_heroes[GIMLI].checkpoint.push_back(tmp);
					tmp.target.pos_x = base_x + 5500 * side;
					tmp.target.pos_y = base_y + 300 * side;
					my_heroes[GIMLI].checkpoint.push_back(tmp);
					my_heroes[GIMLI].gimliEarly(monsters, my_heroes, opp_heroes, base);
				} else {
					tmp.target.pos_x = base_x + 6000 * side;
					tmp.target.pos_y = base_y + 400 * side;
					my_heroes[GIMLI].checkpoint.push_back(tmp);
					tmp.target.pos_x = base_x + 600 * side;
					tmp.target.pos_y = base_y + 600 * side;
					my_heroes[GIMLI].checkpoint.push_back(tmp);
					my_heroes[GIMLI].gimliEarly(monsters, my_heroes, opp_heroes, base);
				}
			} else if (i == ARAGORN) {
				if (!g_big_push) {
					tmp.target.pos_x = base_x + 7000 * side;
					tmp.target.pos_y = base_y + 8500 * side;
					my_heroes[ARAGORN].checkpoint.push_back(tmp);
					tmp.target.pos_x = base_x + 8500 * side;
					tmp.target.pos_y = base_y + 8500 * side;
					my_heroes[ARAGORN].checkpoint.push_back(tmp);
					my_heroes[ARAGORN].aragornAction(monsters, my_heroes, opp_heroes, base);
				} else {
					tmp.target.pos_x = base_x + 12030 * side;
					tmp.target.pos_y = base_y + 8700 * side;
					my_heroes[ARAGORN].checkpoint.push_back(tmp);
					my_heroes[ARAGORN].bigPush(monsters, my_heroes, opp_heroes, base);
				}
			}
			if (monsters.size() > 0) {
				resetScoreTarget(monsters);
			}
		}
		monsters.clear();
		my_heroes.clear();
		opp_heroes.clear();
		base.clear();
	}
}
