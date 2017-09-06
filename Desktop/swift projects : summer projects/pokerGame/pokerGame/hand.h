//
//  hand.h
//  pokerGame
//
//  Created by Seth Saperstein on 9/5/17.
//  Copyright © 2017 Seth Saperstein. All rights reserved.
//

#ifndef hand_h
#define hand_h

#include <stdio.h>
#include <iostream>
#include <map>

const std::map<std::string, int> cardName = {
    {"two", 1},
    {"three", 2},
    {"four", 3},
    {"five", 4},
    {"six", 5},
    {"seven", 6},
    {"eight", 7},
    {"nine", 8},
    {"ten", 9},
    {"jack", 10},
    {"queen", 11},
    {"king", 12},
    {"ace", 13}
};

const std::string ranksOfCards[13] = {"two", "three", "four", "five", "six",
    "seven", "eight", "nine", "ten", "jack", "queen", "king", "ace"};

const std::string suits[4] = {"clubs", "spades", "hearts", "diamonds"};


class Card {
public:
    Card();
    
    Card(std::string suit_, std::string rank_);
    
    std::string getSuit() const;
    
    std::string getRank() const;
    
    int getRankNum() const;
    
private:
    std::string suit;
    std::string rank;
};

struct Hand {
    Card first;
    Card second;
};

//to overload << to write out a card
std::ostream& operator <<(std::ostream& os, const Card &card);

#endif /* hand_h */
