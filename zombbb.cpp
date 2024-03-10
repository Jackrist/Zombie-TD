// Project identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043

#include "P2random.h"
#include "median.h"
#include "zomb.h"
#include <getopt.h>

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <deque>
#include <queue>
#include <utility>

using namespace std;

int main(int argc, char* argv[]) {
    std::ios::sync_with_stdio(false);
    
    bool isVerbose = false;
    bool isStats = false;
    bool isMedian = false;
    size_t statsNum = 0;
    uint32_t quivCap = 0;
    uint32_t seed = 0;
    uint32_t maxRandDist = 0;
    uint32_t maxRandSpd = 0;
    uint32_t maxRandHlth = 0;

    // Command Line Processing
    opterr = false; // Let us handle all error output for command line options
    int choice = 0;
    int option_index = 0;
    option long_options[] = {
        { "verbose",    no_argument,       nullptr, 'v' },
        { "median",     no_argument,       nullptr, 'm' },
        { "statistics", required_argument, nullptr, 's' },
        { "help",       no_argument,       nullptr, 'h' },
        { nullptr,      0,                 nullptr, '\0'}
    };

    while ((choice = getopt_long(argc, argv, "vms:h", long_options, &option_index)) != -1) {
        switch (choice) {
        case 'h':
            cout << "This program simulates a game of The Walking Deadline. Please specify which flags "
                    "you would like to be active for informational output. Possible flags are v(erbose)"
                    ", s(tatistics), and m(edian).\n";
            exit(0);

        case 'v':
            isVerbose = true;
            break;

        case 'm':
            isMedian = true;
            break;

        case 's':
            isStats = true;
            statsNum = atoi(optarg);
            break;
        } // switch
    } // while
    // Command Line Processing

    // File Header Info Handling
    string junk;
    getline(cin, junk);
    cin >> junk >> quivCap >> junk >> seed >> junk >> maxRandDist
        >> junk >> maxRandSpd >> junk >> maxRandHlth;
    getline(cin, junk);

    //initializes random zombie creation based on file header info
	P2random::initialize(seed, maxRandDist, maxRandSpd, maxRandHlth); 

    // Do game.
    uint32_t quiv = 0;
    uint32_t round = 1;
    uint32_t numKilled = 0;
    uint32_t numCreated = 0;
    string nxtNm;
    uint32_t nxtDst;
    uint32_t nxtSpd;
    uint32_t nxtHlth;

    deque<zomb> masterList;
    vector<zomb*> masterPtrs;  // use this vec to range-base initialize most/least active at the end of program
    vector<zomb*> deadZombs;   // use this to print first/last killed at the end
    priority_queue<zomb*, vector<zomb*>, revZombCMP> liveZombs;
    medianZombQueue median;    // push dead zombs in here
    bool hasKilled = false;    // track whether or not to output median at end of round

    uint32_t currFileRound = 0;
    string defeatZomb = "alive";
    int newDist = 0;
    bool playerAlive = true;

    while (cin >> junk) {
        quiv = quivCap;
        cin >> junk >> currFileRound;

        while (round != currFileRound) {
            quiv = quivCap;
            if (isVerbose) {
                cout << "Round: " << round << "\n";
            }
            if (!liveZombs.empty()) {
                for (uint32_t i = 0; i < masterPtrs.size(); ++i) {    // Move zombs forward if not 1st round
                    if (masterPtrs[i]->isAlive == true) {
                        newDist = max(0, (masterPtrs[i]->dist - masterPtrs[i]->speed));
                        masterPtrs[i]->dist = newDist;
                        if (isVerbose) {
                            cout << "Moved: " << masterPtrs[i]->name << " (distance: " << masterPtrs[i]->dist
                                << ", speed: " << masterPtrs[i]->speed << ", health: " << masterPtrs[i]->health << ")\n";
                        }
                        if (masterPtrs[i]->dist == 0 && defeatZomb == "alive") {
                            defeatZomb = masterPtrs[i]->name;
                            playerAlive = false;
                        }
                    }
                }
            }
            if (!playerAlive) {
                break;
            }
            zomb* currZomb; // zomb killing block
            uint32_t zombHlth;
            while (quiv != 0 && !liveZombs.empty()) {
                currZomb = liveZombs.top();
                zombHlth = currZomb->health;
                if (quiv >= zombHlth) {
                    quiv -= zombHlth;
                    if (isVerbose) {
                        cout << "Destroyed: " << currZomb->name << " (distance: "
                            << currZomb->dist << ", speed: " << currZomb->speed << ", health: 0)\n";
                    }
                    hasKilled = true;
                    currZomb->isAlive = false;
                    deadZombs.push_back(currZomb);
                    median.push(currZomb);
                    liveZombs.pop();
                    ++numKilled;
                }
                else {
                    currZomb->health -= quiv;
                    quiv = 0;
                }
            }
            if (isMedian && hasKilled) {
                cout << "At the end of round " << round << ", the median zombie lifetime is " << median.getMed() << "\n";
            }
            for (uint32_t i = 0; i < masterPtrs.size(); ++i) {
                if (masterPtrs[i]->isAlive) {
                    masterPtrs[i]->rdsAct += 1;
                }
            }
            ++round;
        }
        if (!playerAlive) {
            break;
        }

        if (isVerbose) {
            cout << "Round: " << round << "\n";
        }

        if (!liveZombs.empty()) {
            for (uint32_t i = 0; i < masterPtrs.size(); ++i) {    // Move zombs forward if not 1st round
                if (masterPtrs[i]->isAlive == true) {
                    newDist = max(0, (masterPtrs[i]->dist - masterPtrs[i]->speed));
                    masterPtrs[i]->dist = newDist;
                    if (isVerbose) {
                        cout << "Moved: " << masterPtrs[i]->name << " (distance: " << masterPtrs[i]->dist
                        << ", speed: " << masterPtrs[i]->speed << ", health: " << masterPtrs[i]->health << ")\n";
                    }
                    if (masterPtrs[i]->dist == 0 && defeatZomb == "alive") {
                        defeatZomb = masterPtrs[i]->name;
                        playerAlive = false;
                    }
                }
            }
        }
        if (!playerAlive) {
            break; 
        }

        uint32_t numRand = 0; // zomb spawning block begins here
        uint32_t numCust = 0;
        cin >> junk >> numRand >> junk >> numCust;
        for (uint32_t i = 0; i < numRand; ++i) {
            nxtNm = P2random::getNextZombieName();
            nxtDst = P2random::getNextZombieDistance();
            nxtSpd = P2random::getNextZombieSpeed();
            nxtHlth = P2random::getNextZombieHealth();
            zomb newZomb(nxtNm, nxtDst, nxtSpd, nxtHlth);
            masterList.push_back(newZomb);
            zomb* newZombPtr = &masterList.back();
            liveZombs.push(newZombPtr);
            masterPtrs.push_back(newZombPtr);
            ++numCreated;
            if (isVerbose) {
                cout << "Created: " << nxtNm << " (distance: " << nxtDst
                << ", speed: " << nxtSpd << ", health: " << nxtHlth << ")\n";
            }
        }   // random zomb creation loop
        for (uint32_t i = 0; i < numCust; ++i) {
            cin >> nxtNm >> junk >> nxtDst >> junk >> nxtSpd >> junk >> nxtHlth;
            zomb newZomb(nxtNm, nxtDst, nxtSpd, nxtHlth);
            masterList.push_back(newZomb);
            zomb* newZombPtr = &masterList.back();
            liveZombs.push(newZombPtr);
            masterPtrs.push_back(newZombPtr);
            ++numCreated;
            if (isVerbose) {
                cout << "Created: " << nxtNm << " (distance: " << nxtDst
                << ", speed: " << nxtSpd << ", health: " << nxtHlth << ")\n";
            }
        }   // custom zomb creation loop

        quiv = quivCap;
        zomb* currZomb; // zomb killing block
        uint32_t zombHlth;
        while (quiv != 0 && !liveZombs.empty()) { 
            currZomb = liveZombs.top();
            zombHlth = currZomb->health;
            if (quiv >= zombHlth) {
                quiv -= zombHlth;
                if (isVerbose) {
                    cout << "Destroyed: " << currZomb->name << " (distance: " 
                    << currZomb->dist << ", speed: " << currZomb->speed << ", health: 0)\n";
                }
                hasKilled = true;
                currZomb->isAlive = false;
                deadZombs.push_back(currZomb);
                median.push(currZomb);
                liveZombs.pop();
                ++numKilled;
            }
            else {
                currZomb->health -= quiv;
                quiv = 0;
            }
        } 

        if (isMedian && hasKilled) {
            cout << "At the end of round " << round << ", the median zombie lifetime is " << median.getMed() << "\n";
        }

        for (uint32_t i = 0; i < masterPtrs.size(); ++i) {
            if (masterPtrs[i]->isAlive) {
                masterPtrs[i]->rdsAct += 1;
            }
        }
        ++round;
    }

    while (playerAlive && !liveZombs.empty()) {
        quiv = quivCap;
        if (isVerbose) {
            cout << "Round: " << round << "\n";
        }
        if (!liveZombs.empty()) {
            for (uint32_t i = 0; i < masterPtrs.size(); ++i) {    // Move zombs forward if not 1st round
                if (masterPtrs[i]->isAlive == true) {
                    newDist = max(0, (masterPtrs[i]->dist - masterPtrs[i]->speed));
                    masterPtrs[i]->dist = newDist;
                    if (isVerbose) {
                        cout << "Moved: " << masterPtrs[i]->name << " (distance: " << masterPtrs[i]->dist
                            << ", speed: " << masterPtrs[i]->speed << ", health: " << masterPtrs[i]->health << ")\n";
                    }
                    if (masterPtrs[i]->dist == 0 && defeatZomb == "alive") {
                        defeatZomb = masterPtrs[i]->name;
                        playerAlive = false;
                    }
                }
            }
        }
        if (!playerAlive) {
            break;
        }
        zomb* currZomb; // zomb killing block
        uint32_t zombHlth;
        while (quiv != 0 && !liveZombs.empty()) {
            currZomb = liveZombs.top();
            zombHlth = currZomb->health;
            if (quiv >= zombHlth) {
                quiv -= zombHlth;
                if (isVerbose) {
                    cout << "Destroyed: " << currZomb->name << " (distance: "
                        << currZomb->dist << ", speed: " << currZomb->speed << ", health: 0)\n";
                }
                hasKilled = true;
                currZomb->isAlive = false;
                deadZombs.push_back(currZomb);
                median.push(currZomb);
                liveZombs.pop();
                ++numKilled;
            }
            else {
                currZomb->health -= quiv;
                quiv = 0;
            }
        }
        if (isMedian && hasKilled) {
            cout << "At the end of round " << round << ", the median zombie lifetime is " << median.getMed() << "\n";
        }
        for (uint32_t i = 0; i < masterPtrs.size(); ++i) {
            if (masterPtrs[i]->isAlive) {
                masterPtrs[i]->rdsAct += 1;
            }
        }
        ++round;
    }
    
    string lastZomb;
    if (!deadZombs.empty()) {
        lastZomb = deadZombs.back()->name;
    }
    
    // Survival or demise messages here
    if (playerAlive) {
        cout << "VICTORY IN ROUND " << round - 1 << "! " << lastZomb << " was the last zombie.\n";
    }
    else {
        cout << "DEFEAT IN ROUND " << round << "! " << defeatZomb << " ate your brains!\n";
    }

    // stats output & most/least active priority queues
    if (isStats) {
        uint32_t numActive = numCreated - numKilled;
        cout << "Zombies still active: " << numActive << "\n";

        size_t loop = statsNum;
        if (numKilled < statsNum) { 
            loop = numKilled; // Handles situations where we don't have N zombs to print
        }
        cout << "First zombies killed:\n";
        size_t ctr = 1;
        for (uint32_t i = 0; i < loop; ++i) {
            cout << deadZombs[i]->name << " " << ctr << "\n";
            ++ctr;
        }
        cout << "Last zombies killed:\n";
        ctr = loop;
        if (numKilled < statsNum) {
            if (!deadZombs.empty()) {
                for (size_t i = (deadZombs.size() - 1); i > 0; --i) {
                    cout << deadZombs[i]->name << " " << ctr << "\n";
                    --ctr;
                }
                cout << deadZombs[0]->name << " " << ctr << "\n";
            }  
        }
        else {
            if (!deadZombs.empty()) {
                for (size_t i = (deadZombs.size() - 1); i > ((deadZombs.size() - 1) - loop); --i) {
                    cout << deadZombs[i]->name << " " << ctr << "\n";
                    --ctr;
                }
            }
        }
       
        priority_queue<zomb*, vector<zomb*>, mostActCMP> mostAct(masterPtrs.begin(), masterPtrs.end());
        priority_queue<zomb*, vector<zomb*>, leastActCMP> leastAct(masterPtrs.begin(), masterPtrs.end());
        loop = statsNum;
        zomb* currentZomb;
        if (masterPtrs.size() < statsNum) {
            loop = masterPtrs.size(); // Handles situations where we don't have N zombs to print
        }
        cout << "Most active zombies:\n";
        for (uint32_t i = 0; i < loop; ++i) {
            currentZomb = mostAct.top();
            cout << currentZomb->name << " " << currentZomb->rdsAct << "\n";
            mostAct.pop();
        }
        cout << "Least active zombies:\n";
        for (uint32_t i = 0; i < loop; ++i) {
            currentZomb = leastAct.top();
            cout << currentZomb->name << " " << currentZomb->rdsAct << "\n";
            leastAct.pop();
        }
    }

	return 0;
}