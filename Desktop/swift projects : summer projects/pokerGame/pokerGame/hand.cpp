//
//  hand.cpp
//  pokerGame
//
//  Created by Seth Saperstein on 9/5/17.
//  Copyright © 2017 Seth Saperstein. All rights reserved.
//

#include "hand.h"
#include <string>

using namespace std;

Card::Card() {
    suit = suits[0];
    rank = ranksOfCards[0];
}
Card::Card(string suit_, string rank_) : suit(suit_), rank(rank_) {}

string Card::getSuit() const {
    return suit;
}
string Card::getRank() const {
    return rank;
}

int Card::getRankNum() const {
    int num = cardName.find(rank)->second;
    return num;
}

//to overload << to write out a card
ostream& operator <<(ostream& os, const Card &card) {
    os << card.getRank() << " of " << card.getSuit();
    return os;
}
