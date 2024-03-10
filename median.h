// Project identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043

#include <vector>
#include <queue>
#include "zomb.h"

using namespace std;

class medianZombQueue {  // median specifically for zomb rds active
private: 
	priority_queue<zomb*, vector<zomb*>, revZombMedCMP> uppHalf;
	priority_queue<zomb*, vector<zomb*>, zombMedCMP> lowHalf;

public:
	void push(zomb* newZomb) {
		if (uppHalf.empty() && lowHalf.empty()) {
			uppHalf.push(newZomb);
		}
		else if (newZomb->rdsAct > uppHalf.top()->rdsAct) {
			uppHalf.push(newZomb);
		}
		else {
			lowHalf.push(newZomb);
		}

		// Rebalancing queue sizes if need be
		if ((uppHalf.size() - lowHalf.size()) == 2) {
			lowHalf.push(uppHalf.top());
			uppHalf.pop();
		}
		else if ((lowHalf.size() - uppHalf.size()) == 2) {
			uppHalf.push(lowHalf.top());
			lowHalf.pop();
		}
	}

	uint32_t getMed() const {
		if (uppHalf.size() == lowHalf.size()) {
			return ((uppHalf.top()->rdsAct + lowHalf.top()->rdsAct) / 2);
		}
		else if (uppHalf.size() > lowHalf.size()) {
			return uppHalf.top()->rdsAct;
		}
		else {
			return lowHalf.top()->rdsAct;
		}
	}

	bool empty() const {
		return (uppHalf.empty() && lowHalf.empty());
	}

	size_t size() const {
		return (uppHalf.size() + lowHalf.size());
	}
};

// Edit above class for p2b medianQueue part. (edit getMed and template it)