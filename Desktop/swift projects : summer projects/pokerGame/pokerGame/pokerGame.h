#ifndef POKERGAME_H
#define POKERGAME_H
//
//  pokerGame.h
//  pokerGame
//
//  Created by Seth Saperstein on 8/25/17.
//  Copyright © 2017 Seth Saperstein. All rights reserved.
//

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <deque>
#include <cassert>
#include <unordered_map>
#include <queue>

using namespace std;

const map<string, int> cardName = {
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

const string ranksOfCards[13] = {"two", "three", "four", "five", "six",
    "seven", "eight", "nine", "ten", "jack", "queen", "king", "ace"};

const string suits[4] = {"clubs", "spades", "hearts", "diamonds"};

class Card {
public:
    Card();
    
    Card(string suit_, string rank_);
    
    string getSuit() const;
    
    string getRank() const;
    
    int getRankNum() const;
    
    
    
private:
    string suit;
    string rank;
};

//to overload << to write out a card
ostream& operator <<(ostream& os, const Card &card);

class deckOfCards {
public:
    deckOfCards();
    
    void resetDeck();
    
    Card getCard();
    
private:
    deque<Card> deck;
};

struct Hand {
    Card first;
    Card second;
};

class player {
public:
    player();
    
    player(string name_);
    
    string name;
    int stackSize;
    Hand hand;
    int amtBetTotal;
    int rankAfterHandFinishes;
};

bool operator >(const Card &card1, const Card &card2);

bool operator <(const Card &card1, const Card &card2);

bool operator ==(const Card &card1, const Card &card2);

class table {
public:
    table();
    
    void playTable();
    
    void playHand();
    
    //return -1 if player2 is stronger, 0 if tied, 1 if player 1 is stronger
    int highCardComparison(const vector<Card> &player1Cards, const vector<Card> &player2Cards);
    
    void closerCompareOfHands(int positionOfP1, int positionOfP2);
    
    void findWinner();
    
    void chooseDealer();
    
    void addPlayerToTable(player *person);
    
    bool openSeatAtTable();
    
    void dealCards();
    
    int getNumPlayers() const;
    
    void next();
    
    void nextDeal();
    
    void fold();
    
    string callDecision();
    
    void allInOrFold();
    
    void allIn();
    
    void raiseAmount();
    
    void readPlayerHand();
    
    void checkFoldOrBet();
    
    void bettingRound();
    
    void preFlopActivity();
    
    void flop();
    
    void turn();
    
    void river();
    
    void splitPotBasedOnRank();
    
private:
    player* players[10];
    bool inHand[10];
    int dealer;
    int currentlyOn;
    deckOfCards Deck;
    int numPlayers;
    int numPlayersInHand;
    int numPlayersFolded;
    int pot;
    int maxBetTotal;
    bool betInRound;
    //vector of sidepots with pairs <amount, <position of people in sidepot>>
    vector<pair<int, vector<int>>> sidepots;
    int smallBlind;
    int bigBlind;
    int initialBetThisRound;
    vector<Card> board;
    pair<int, int> handsThisRound[10];
    //int rankAfterHandFinishes[10];
};

struct playerRankComparator {
    bool operator()(const player* player1, const player* player2);
};

void endGame(vector<player*> &allPlayers);

//return true if the first is greater than the second
bool compareMultiplesOf(const pair<int, int> &mult1, const pair<int, int> &mult2);

//return true if card 1 is of the suit and stronger
struct straightFlushCompHelper {
    straightFlushCompHelper(string suit_);
    
    bool operator()(const Card &card1, const Card &card2);
    
    string suit;
};


//first int returns rank of hand, second int is rank of card relative to that hand rank
//0 is high card, 1 is pair, 2 is 2 pair, 3 is trips
//4 is straight, 5 is flush, 6 is boat, 7 is quads, 8 is straight flush
pair<int, int> handEval(const Hand &hand, vector<Card> board);

const static deckOfCards freshDeck = deckOfCards();

#endif
