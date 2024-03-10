// Project identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043

#pragma once

#include <string>

using namespace std;

struct zomb {
	// zomb constructor
	zomb(string nm, uint32_t dst, uint32_t spd, uint32_t hlth)
		: name(nm), dist(dst), speed(spd), health(hlth) {}

	string name;
	int dist;
	int speed;
	uint32_t health;
	bool isAlive = true;
	uint32_t rdsAct = 1; // Will have to update all zombs every round
};

// Zombie comparison functor w/ less || need to template for p2b ?
struct zombCMP {
	bool operator()(const zomb* left, const zomb* right) const {
		int leftETA = (left->dist / left->speed);
		int rightETA = (right->dist / right->speed);
		if (leftETA == rightETA) {
			if (left->health == right->health) {
				return left->name < right->name;
			}
			else {
				return left->health < right->health;
			}
		}
		else {
			return leftETA < rightETA;
		}
	}
};

// Zombie comparison functor w/ greater || need to template for p2b ?
struct revZombCMP {
	bool operator()(const zomb* left, const zomb* right) const {
		uint32_t leftETA = (left->dist / left->speed);
		uint32_t rightETA = (right->dist / right->speed);
		if (leftETA == rightETA) {
			if (left->health == right->health) {
				return right->name < left->name;
			}
			else {
				return right->health < left->health;
			}
		}
		else {
			return rightETA < leftETA;
		}
	}
};

// Zombie comparator specifically for median zomb rds active
struct zombMedCMP {
	bool operator()(const zomb* left, const zomb* right) const {
		return (left->rdsAct < right->rdsAct);
	}
};

// Zombie comparator specifically for median zomb rds active
struct revZombMedCMP {
	bool operator()(const zomb* left, const zomb* right) const {
		return (right->rdsAct < left->rdsAct);
	}
};

struct mostActCMP {
	bool operator()(const zomb* left, const zomb* right) const {
		if (left->rdsAct == right->rdsAct) {
			return right->name < left->name;
		}
		else {
			return (left->rdsAct < right->rdsAct);
		}
	}
};

struct leastActCMP {
	bool operator()(const zomb* left, const zomb* right) const {
		if (left->rdsAct == right->rdsAct) {
			return right->name < left->name;
		}
		else {
			return (right->rdsAct < left->rdsAct);
		}
	}
};