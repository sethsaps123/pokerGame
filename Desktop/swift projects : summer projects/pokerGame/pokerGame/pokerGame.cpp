//
//  main.cpp
//  pokerGame
//
//  Created by Seth Saperstein on 8/20/17.
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

struct Card {
public:
    Card() {
        suit = suits[0];
        rank = ranksOfCards[0];
    }
    Card(string suit_, string rank_) : suit(suit_), rank(rank_) {}
    string getSuit() const {
        return suit;
    }
    string getRank() const {
        return rank;
    }

private:
    string suit;
    string rank;
};

//to overload << to write out a card
ostream& operator <<(ostream& os, const Card &card) {
    os << card.getRank() << " of " << card.getSuit();
    return os;
}

class deckOfCards {
public:
    deckOfCards() {
        for (int i = 0; i < 13; i++) {
            for (int j = 0; j < 4; j++) {
                deck.push_back(Card(suits[j], ranksOfCards[i]));
            }
        }
        random_shuffle(deck.begin(), deck.end());
    }
    
    void resetDeck() {
        deck.clear();
        for (int i = 0; i < 13; i++) {
            for (int j = 0; j < 4; j++) {
                deck.push_back(Card(suits[j], ranksOfCards[i]));
            }
        }
        random_shuffle(deck.begin(), deck.end());
    }
    
    Card getCard() {
        Card chosen = deck.front();
        deck.pop_front();
        return chosen;
    }
private:
    deque<Card> deck;
};

struct Hand {
public:
    Card first;
    Card second;
};

class player {
public:
    player() : name("unkown"), stackSize(1500), amtBetTotal(0){ }
    player(string name_) : name(name_), stackSize(1500), amtBetTotal(0) {}
    
    string name;
    int stackSize;
    Hand hand;
    int amtBetTotal;
};

bool operator >(const Card &card1, const Card &card2) {
    if ((cardName.find(card1.getRank())->second >
         (cardName.find(card2.getRank())->second))) {
        return true;
    }
    return false;
}

bool operator <(const Card &card1, const Card &card2) {
    if ((cardName.find(card1.getRank())->second <
         (cardName.find(card2.getRank())->second))) {
        return true;
    }
    return false;
}

bool operator ==(const Card &card1, const Card &card2) {
    if ((cardName.find(card1.getRank())->second >
         (cardName.find(card2.getRank())->second))) {
        return true;
    }
    return false;
}


class table {
public:
    table() : smallBlind(10), bigBlind(20), numPlayers(0), pot(0), maxBetTotal(0),
    initialBetThisRound(0), numPlayersFolded(0), numPlayersInHand(0) {
        for (int i = 0; i < 10; i++) {
            players[i] = nullptr;
        }
        
    }
    
    void playHand() {
        chooseDealer();
        preFlopActivity();
        //only show the board if there is more than 1 person in
        //whether they're all in or just betting
        int playersInPot = numPlayers - numPlayersFolded;
        if (playersInPot > 1) {
            flop();
        }
        playersInPot = numPlayers - numPlayersFolded;
        if (playersInPot > 1) {
            turn();
        }
        playersInPot = numPlayers - numPlayersFolded;
        if (playersInPot > 1) {
            river();
        }
        cout << "End hand." << endl;
        
        for (int i = 0; i < 10; i++) {
            cout << players[i]->name << " has " << players[i]->stackSize << endl;
        }
        cout << "Pot: " << pot << endl;
        //findWinner();
    }
    
    void chooseDealer() {
        deckOfCards dealerDeck = deckOfCards();
        cout << endl << "High card for deal." << endl;
        Card maxCard = Card("spades", "two");
        for (int i = 0; i < 10; i++) {
            if (players[i] != nullptr) {
                inHand[i] = true;
                Card forDeal = dealerDeck.getCard();
                players[i]->hand.first = forDeal;
                cout << players[i]->name << " gets a " <<
                forDeal.getRank() << "." << endl;
                if (players[i]->hand.first > maxCard) {
                    dealer = i;
                    maxCard = players[i]->hand.first;
                }
            }
        }
        cout << players[dealer]->name << " deals." << endl << endl;
    }
    
    void addPlayerToTable(player *person) {
        for (int i = 0; i < 10; i++) {
            if (!players[i]) {
                players[i] = person;
                ++numPlayers;
                cout << "Added " << person->name << " to the table." << endl;
                return;
            }
        }
        cout << "cannot add person to table" << endl;
        return;
    }
    
    bool openSeatAtTable() {
        if (numPlayers != 10) return true;
        return false;
    }
    
    void dealCards() {
        cout << "Dealing cards." << endl;
        Deck.resetDeck();
        for (int i = 0; i < 10; i++) {
            if (players[i] != nullptr) players[i]->hand.first = Deck.getCard();
        }
        for (int i = 0; i < 10; i++) {
            if (players[i] != nullptr) players[i]->hand.second = Deck.getCard();
        }
    }
    
    int getNumPlayers() const {
        return numPlayers;
    }
    
    void next() {
        if (currentlyOn == 9) {
            currentlyOn = 0;
            while (!(inHand[currentlyOn])) {
                ++currentlyOn;
            }
            return;
        }
        else {
            ++currentlyOn;
            if (inHand[currentlyOn]) return;
            else{
                next();
            }
        }
        return;
    }
    
    void nextDeal() {
        if (dealer == 9) {
            dealer = 0;
            while (!(inHand[dealer])) {
                ++dealer;
            }
            return;
        }
        else {
            ++dealer;
            if (inHand[dealer]) return;
            else{
                nextDeal();
            }
        }
        return;
    }
    
    void fold() {
        cout << players[currentlyOn]->name << " folds." << endl << endl;
        --numPlayersInHand;
        ++numPlayersFolded;
        inHand[currentlyOn] = false;
    }
    
    string callDecision() {
        cout << "call, fold, or raise?" << endl;
        string decision;
        cin >> decision;
        if (decision == "call") {
            return "call";
        }
        else if (decision == "fold") {
            fold();
            return "fold";
        }
        else if (decision == "raise") {
            return "raise";
        }
        else {
            cout << "That is invalid. Try again." << endl;
            return callDecision();
        }
    }
    
    void allInOrFold() {
        cout << "call or fold?" << endl;
        string decision;
        cin >> decision;
        if (decision == "call") {
            allIn();
            return;
        }
        else if (decision == "fold") {
            fold();
            return;
        }
        else {
            cout << "That is invalid. Try again." << endl;
            return allInOrFold();
        }
    }
    
    void allIn() {
        --numPlayersInHand;
        cout << players[currentlyOn]->name << " is all in for "
        << players[currentlyOn]->stackSize << endl << endl;
        pot += players[currentlyOn]->stackSize;
        players[currentlyOn]->amtBetTotal += players[currentlyOn]->stackSize;
        players[currentlyOn]->stackSize = 0;
        maxBetTotal = players[currentlyOn]->amtBetTotal;
    }
    
    void raiseAmount() {
        assert(inHand[currentlyOn]);
        assert(players[currentlyOn]->stackSize > 0);
        cout << "Stacksize: " << players[currentlyOn]->stackSize << endl;
        cout << "How much would you like to raise?" << endl;
        int betSize;
        cin >> betSize;
        //if the player decides to go all in on top
        if ((maxBetTotal + betSize) >= players[currentlyOn]->stackSize) {
            allIn();
            return;
        }
        //if the player bets less than the min raise ask them again
        else if (betSize < initialBetThisRound) {
            cout << "Minimum raise is " << initialBetThisRound << endl;
            return raiseAmount();
        }
        //when the player bets min or above and less than all in
        else if (betSize >= initialBetThisRound && betSize < players[currentlyOn]->stackSize) {
            cout << players[currentlyOn]->name << " raises " << betSize << endl << endl;
            pot += maxBetTotal - players[currentlyOn]->amtBetTotal + betSize;
            players[currentlyOn]->stackSize -= maxBetTotal - players[currentlyOn]->amtBetTotal + betSize;
            players[currentlyOn]->amtBetTotal = maxBetTotal + betSize;
            maxBetTotal = players[currentlyOn]->amtBetTotal;
        }
        else {
            assert(false);
        }
    }
    
    void readPlayerHand() {
        cout << players[currentlyOn]->name << "'s hand: " << endl;
        cout << players[currentlyOn]->hand.first.getRank() << " of "
        << players[currentlyOn]->hand.first.getSuit() << endl;
        cout << players[currentlyOn]->hand.second.getRank() << " of "
        << players[currentlyOn]->hand.second.getSuit() << endl;
        cout << "Stack size: " << players[currentlyOn]->stackSize << endl;
        cout << "Pot: " << pot << endl << endl;
    }
    
    void checkFoldOrBet() {
        cout << "check, fold, or bet?" << endl;
        string decision;
        cin >> decision;
        if (decision == "check") {
            cout << players[currentlyOn]->name << " checks." << endl << endl;
            return;
        }
        else if (decision == "fold") {
            fold();
            return;
        }
        else if (decision == "bet") {
            cout << "How much?" << endl;
            int betAmount = 0;
            cin >> betAmount;
            cout << endl;
            if (betAmount >= players[currentlyOn]->stackSize) {
                betInRound = true;
                initialBetThisRound = betAmount;
                allIn();
                return;
            }
            else if (betAmount < bigBlind) {
                cout << "min bet is " << bigBlind << endl;
                return checkFoldOrBet();
            }
            //normal bet above min and not all in
            else {
                betInRound = true;
                initialBetThisRound = betAmount;
                pot += betAmount;
                maxBetTotal += betAmount;
                players[currentlyOn]->amtBetTotal += betAmount;
                players[currentlyOn]->stackSize -= betAmount;
                return;
            }
        }
        else {
            cout << "that is not an option." << endl;
            return checkFoldOrBet();
        }
    }
    
    void bettingRound() {
        //num players in hand is greater than 1 and either there hasn't
        //been a bet yet or the maxbettotal > amtbettotal of player
        int startingActionPlayer = currentlyOn;
        while (((players[currentlyOn]->amtBetTotal < maxBetTotal)
               || (!betInRound)) && (numPlayersInHand > 1)) {
            //if the players in the hand and not all in
            if (inHand[currentlyOn] && players[currentlyOn]->stackSize != 0) {
                readPlayerHand();
                //if the player needs to call someone's bet or the blind
                if (maxBetTotal > players[currentlyOn]->amtBetTotal) {
                    int toCall = maxBetTotal - players[currentlyOn]->amtBetTotal;
                    cout << players[currentlyOn]->name << " to call " << toCall
                    << endl;
                    string decision;
                    //if the player will not be all in if they call.
                    //fold taken care of in callDecision()
                    if (toCall < players[currentlyOn]->stackSize) {
                        decision = callDecision();
                        if (decision == "call") {
                            cout << players[currentlyOn]->name << " calls." << endl << endl;
                            pot += toCall;
                            players[currentlyOn]->stackSize -= toCall;
                            players[currentlyOn]->amtBetTotal += toCall;
                        }
                        else if (decision == "raise") {
                            raiseAmount();
                        }
                        else if (decision != "fold") {
                            //something wrong. get rid of this at the end
                            assert(false);
                        }
                    }
                    //if the player will be all in if they call
                    else if (toCall >= players[currentlyOn]->stackSize) {
                        allInOrFold();
                    }
                }
                //else if no one has bet yet
                else if (!betInRound) {
                    checkFoldOrBet();
                }
            }
            next();
            if (!betInRound && (startingActionPlayer == currentlyOn)) {
                break;
            }
        }
    }
    
    void preFlopActivity() {
        pot = 0;
        numPlayersInHand = numPlayers;
        currentlyOn = dealer;
        betInRound = true;
        next();
        //if sidepot is needed for SB
        if (players[currentlyOn]->stackSize < smallBlind) {
            pot += players[currentlyOn]->stackSize;
            players[currentlyOn]->amtBetTotal = players[currentlyOn]->stackSize;
            players[currentlyOn]->stackSize = 0;
            maxBetTotal = pot;
            cout << "sidepot small blind" << endl;
        }
        else {
            players[currentlyOn]->stackSize -= smallBlind;
            players[currentlyOn]->amtBetTotal = smallBlind;
            maxBetTotal = smallBlind;
            pot += smallBlind;
            cout << players[currentlyOn]->name << " posts the small blind." << endl;
        }
        next();
        //if sidepot is needed for BB
        if (players[currentlyOn]->stackSize < bigBlind) {
            pot += players[currentlyOn]->stackSize;
            players[currentlyOn]->amtBetTotal = players[currentlyOn]->stackSize;
            if (players[currentlyOn]->stackSize > maxBetTotal) {
                maxBetTotal = players[currentlyOn]->stackSize;
            }
            players[currentlyOn]->stackSize = 0;
            cout << "sidepot big blind" << endl;
        }
        else {
            players[currentlyOn]->stackSize -= bigBlind;
            players[currentlyOn]->amtBetTotal += bigBlind;
            pot += bigBlind;
            maxBetTotal = bigBlind;
            cout << players[currentlyOn]->name << " posts the big blind." << endl;
        }
        initialBetThisRound = bigBlind;
        next();
        bettingRound();
    }
    
    void flop() {
        cout << "Board: " << endl;
        for (int i = 0; i < 3; i++) {
            board.push_back(Deck.getCard());
            cout << board[i] << endl;
        }
        cout << endl;
        //only do the betting round if 2 people in hand that aren't all in
        if (numPlayersInHand > 1) {
            betInRound = false;
            currentlyOn = dealer;
            next();
            bettingRound();
        }
    }
    
    void turn() {
        cout << "Board: " << endl;
        board.push_back(Deck.getCard());
        for (int i = 0; i < 4; i++) {
            cout << board[i] << endl;
        }
        cout << endl;
        //only do the betting round if 2 people in hand that aren't all in
        if (numPlayersInHand > 1) {
            betInRound = false;
            currentlyOn = dealer;
            next();
            bettingRound();
        }
    }
    
    void river() {
        cout << "Board: " << endl;
        board.push_back(Deck.getCard());
        for (int i = 0; i < 5; i++) {
            cout << board[i] << endl;
        }
        cout << endl;
        //only do the betting round if 2 people in hand that aren't all in
        if (numPlayersInHand > 1) {
            betInRound = false;
            currentlyOn = dealer;
            next();
            bettingRound();
        }
    }
    
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
};

void endGame(vector<player*> &allPlayers) {
    while (!allPlayers.empty()) {
        delete allPlayers.back();
        allPlayers.pop_back();
    }
}
int main(int argc, const char **argv) {
    int playerNameCounter = 0;
    table table1 = table();
    vector<player*> allPlayers;
    while(table1.openSeatAtTable()) {
        player* p1 =  new player(to_string(playerNameCounter));
        allPlayers.push_back(p1);
        table1.addPlayerToTable(p1);
        ++playerNameCounter;
    }
    table1.playHand();
    endGame(allPlayers);
    return 0;
}



