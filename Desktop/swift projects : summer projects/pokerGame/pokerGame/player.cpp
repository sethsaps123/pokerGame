//
//  player.cpp
//  pokerGame
//
//  Created by Seth Saperstein on 9/5/17.
//  Copyright © 2017 Seth Saperstein. All rights reserved.
//

#include "player.h"
#include <iostream>
#include <cassert>
#include "hand.h"

using namespace std;

//helper func for simple decision
string getDecision() {
    string decision;
    cin >> decision;
    return decision;
}

int getAmount() {
    int amount;
    cin >> amount;
    return amount;
}

class Human : public player {
public:
    Human(string name_) : name(name_), stackSize(1500), amtBetTotal(0)  {}
    
    string callFoldRaiseDecision() const {
        return getDecision();
    }
    
    string allInOrFoldDecision() const {
        return getDecision();
    }
    
    int raiseAmountDecision() const {
        return getAmount();
    }
    
    string checkFoldOrBetDecision() const {
        return getDecision();
    }
    
    int getBetAmount() const {
        return getAmount();
    }
    
    //begin getters and setters for private variables
    
    const std::string & getName() const {
        return name;
    }
    
    const int getStackSize() const {
        return stackSize;
    }
    
    void setStackSize(int size) {
        stackSize = size;
    }
    
    const Card getHandFirst() const {
        return hand.first;
    }
    
    const Card getHandSecond() const {
        return hand.second;
    }
    
    void setHandFirst(const Card &card) {
        hand.first = card;
    }
    
    void setHandSecond(const Card &card) {
        hand.second = card;
    }
    
    const int getAmtBetTotal() const {
        return amtBetTotal;
    }
    
    void setAmtBetTotal(int amount) {
        amtBetTotal = amount;
    }
    
    const int getRankAfterHandFinishes() const {
        return rankAfterHandFinishes;
    }
    
    void setRankAfterHandFinishes(int rank) {
        rankAfterHandFinishes = rank;
    }

private:
    std::string name;
    int stackSize;
    Hand hand;
    int amtBetTotal;
    int rankAfterHandFinishes;
};

player * playerFactory(const string &name, const string &strategy) {
    if (strategy == "Human") {
        return new Human(name);
    }
    else {
        assert(false);
    }
}
