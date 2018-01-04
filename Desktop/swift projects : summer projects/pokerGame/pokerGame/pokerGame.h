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
#include "player.h"
#include "hand.h"

using namespace std;




class deckOfCards {
public:
    deckOfCards();
    
    void resetDeck();
    
    Card getCard();
    
private:
    deque<Card> deck;
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
    int smallBlind;
    int bigBlind;
    int initialBetThisRound;
    vector<Card> board;
    pair<int, int> handsThisRound[10];
    int round;
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
pair<int, int> handEval(const Hand &hand, vector<Card> &board);
pair<int, int> botHandEval(const Hand &hand, vector<Card> &board, bool withHand);
const static deckOfCards freshDeck = deckOfCards();

#endif
