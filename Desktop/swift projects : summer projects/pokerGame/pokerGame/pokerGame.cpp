//
//  pokerGame.cpp
//  pokerGame
//
//  Created by Seth Saperstein on 8/20/17.
//  Copyright © 2017 Seth Saperstein. All rights reserved.
//

#include "pokerGame.h"

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
    
    int getRankNum() const {
        int num = cardName.find(rank)->second;
        return num;
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

//declare function for later use
pair<int, int> handEval(const Hand &hand, vector<Card> board);

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
        int winner = findWinner();
        
        for (int i = 0; i < 10; i++) {
            if (inHand[i]) {
                cout << players[i]->name << "'s hand: " << endl
                << players[i]->hand.first << endl << players[i]->hand.second
                << endl << endl;
            }
        }
        cout << players[winner]->name << " wins!" << endl << endl;
    }
    
    //return -1 if player2 is stronger, 0 if tied, 1 if player 1 is stronger
    int highCardComparison(const vector<Card> &player1Cards, const vector<Card> &player2Cards) {
        int counter = 0;
        auto it1 = player1Cards.end() - 1;
        auto it2 = player2Cards.end() - 1;
        while (counter < 5) {
            if (it1->getRankNum() < it2->getRankNum()) return -1;
            else if (it1->getRankNum() > it2->getRankNum()) return 1;
        }
        return 0;
    }
    
    void closerCompareOfHands(int positionOfP1, int positionOfP2) {
        //add players hand to board in new vec, then sort
        vector<Card> player1Cards = vector<Card>(board);
        player1Cards.push_back(players[positionOfP1]->hand.first);
        player1Cards.push_back(players[positionOfP1]->hand.second);
        sort(player1Cards.begin(), player1Cards.end());
        
        //add players hand to board in new vec, then sort
        vector<Card> player2Cards = vector<Card>(board);
        player2Cards.push_back(players[positionOfP2]->hand.first);
        player2Cards.push_back(players[positionOfP2]->hand.second);
        sort(player2Cards.begin(), player2Cards.end());
        
        int playerComp = 0;
        int nextP1Pair = -1;
        int nextP2Pair = -1;
        int firstP1Pair = 0;
        int firstP2Pair = 0;
        bool hitP1FirstPair = false;
        bool hitP2FirstPair = false;
        switch (handsThisRound[positionOfP1].first) {
            //high card case
            case 0:
                //-1 is p2 stronger, 0 for tie, 1 for p1 stronger
                playerComp = highCardComparison(player1Cards, player2Cards);
                if (playerComp == -1) {
                    rankAfterHandFinishes[positionOfP2] += 10;
                }
                else if (playerComp == 0) {
                    rankAfterHandFinishes[positionOfP1] += 1;
                    rankAfterHandFinishes[positionOfP2] += 1;
                }
                else if (playerComp == 1) {
                    rankAfterHandFinishes[positionOfP1] += 10;
                }
                else {
                    exit(1);
                }
                break;
            //one pair case
            case 1:
                //-1 is p2 stronger, 0 for tie, 1 for p1 stronger
                playerComp = highCardComparison(player1Cards, player2Cards);
                if (playerComp == -1) {
                    rankAfterHandFinishes[positionOfP2] += 10;
                }
                else if (playerComp == 0) {
                    rankAfterHandFinishes[positionOfP1] += 1;
                    rankAfterHandFinishes[positionOfP2] += 1;
                }
                else if (playerComp == 1) {
                    rankAfterHandFinishes[positionOfP1] += 10;
                }
                else {
                    exit(1);
                }
                break;
            //two pair case. second pair must be checked and next highest card
            case 2:

                for (unsigned int i = (int)(player1Cards.size()) - 1; i > 0; i--) {
                    if (player1Cards[i].getRank() == player1Cards[i - 1].getRank()) {
                        if (!hitP1FirstPair) {
                            hitP1FirstPair = true;
                            firstP1Pair = player1Cards[i].getRankNum();
                        }
                        else if (nextP1Pair == -1) {
                            nextP1Pair = player1Cards[i].getRankNum();
                        }
                    }
                    if (player2Cards[i].getRank() == player2Cards[i - 1].getRank()) {
                        if (!hitP2FirstPair) {
                            hitP2FirstPair = true;
                            firstP2Pair = player2Cards[i].getRankNum();
                        }
                        else if (nextP2Pair == -1) {
                            nextP2Pair = player2Cards[i].getRankNum();
                        }
                    }
                }
                if (nextP1Pair > nextP2Pair) {
                    rankAfterHandFinishes[positionOfP1] += 10;
                }
                else if (nextP2Pair > nextP1Pair) {
                    rankAfterHandFinishes[positionOfP2] += 10;
                }
                //down to 5th card high card
                else {
                    int highCardP1 = 0;
                    int highCardP2 = 0;
                    for (int i = (int)(player1Cards.size()) - 1; i >= 0; i++) {
                        if ((player1Cards[i].getRankNum() != firstP1Pair) &&
                            (player1Cards[i].getRankNum() != nextP1Pair)) {
                            highCardP1 = player1Cards[i].getRankNum();
                            break;
                        }
                    }
                    for (int i = (int)(player2Cards.size()) - 1; i >= 0; i++) {
                        if ((player2Cards[i].getRankNum() != firstP2Pair) &&
                            (player2Cards[i].getRankNum() != nextP2Pair)) {
                            highCardP2 = player2Cards[i].getRankNum();
                            break;
                        }
                    }
                    if (highCardP1 > highCardP2) {
                        rankAfterHandFinishes[positionOfP1] += 10;
                    }
                    else if (highCardP2 > highCardP1) {
                        rankAfterHandFinishes[positionOfP2] += 10;
                    }
                    else {
                        rankAfterHandFinishes[positionOfP1] += 1;
                        rankAfterHandFinishes[positionOfP2] += 1;
                    }
                }
                break;
            //straight. straight to same card. auto tie
            case 4:
                rankAfterHandFinishes[positionOfP1] += 1;
                rankAfterHandFinishes[positionOfP2] += 1;
                break;
            //full house. must check smaller pair
            case 6:
                //finds first pair that isn't trips starting at highest rank
                for (int i = (int)(player1Cards.size()) - 1; i > 1; i--) {
                    if ((player1Cards[i].getRank() == player1Cards[i - 1].getRank()) &&
                        (player1Cards[i - 1].getRank() != player1Cards[i - 2].getRank())) {
                        firstP1Pair = player1Cards[i].getRankNum();
                        break;
                    }
                }
                for (int i = (int)(player2Cards.size()) - 1; i > 1; i--) {
                    if ((player2Cards[i].getRank() == player2Cards[i - 1].getRank()) &&
                        (player2Cards[i - 1].getRank() != player2Cards[i - 2].getRank())) {
                        firstP2Pair = player2Cards[i].getRankNum();
                        break;
                    }
                }
                if (firstP1Pair > firstP2Pair) {
                    rankAfterHandFinishes[positionOfP1] += 10;
                }
                else if (firstP2Pair > firstP1Pair) {
                    rankAfterHandFinishes[positionOfP2] += 10;
                }
                else {
                    rankAfterHandFinishes[positionOfP2] += 1;
                    rankAfterHandFinishes[positionOfP1] += 1;
                }
                break;
            default:
                cerr << "something wrong in closer compare of hands function" << endl;
                exit(1);
                break;
        }
    }
    
    
    int findWinner() {
        for (int i = 0; i < 10; i++) {
            if (inHand[i]) {
                pair<int, int> handForRound = handEval(players[i]->hand, board);
                handsThisRound[i] = handForRound;
            }
            else {
                rankAfterHandFinishes[i] = 0;
            }
        }
        //sort-like algorithm to compare everyone in hand to everyone else
        //in the hand. awarding 10 pts for the winner of the 2 hands
        //and in the case of a tie 1 to both
        for (int i = 0; i < 9; i++) {
            if (!inHand[i]) break;
            for (int j = i + 1; j < 10; j++) {
                if (!inHand[j]) break;
                if (handsThisRound[i].first > handsThisRound[j].first) {
                    rankAfterHandFinishes[i] += 10;
                }
                else if (handsThisRound[j].first > handsThisRound[i].first) {
                    rankAfterHandFinishes[j] += 10;
                }
                else if (handsThisRound[i].second > handsThisRound[j].second) {
                    rankAfterHandFinishes[i] += 10;
                }
                else if (handsThisRound[j].second > handsThisRound[i].second) {
                    rankAfterHandFinishes[j] += 10;
                }
                else {
                    closerCompareOfHands(i, j);
                }
            }
        }
        int highestRank = 0;
        int positionHighestRank = 0;
        for (int i = 0; i < 10; i++) {
            if (inHand[i]) {
                if (rankAfterHandFinishes[i] > highestRank) {
                    positionHighestRank = i;
                }
            }
        }
        cout << positionHighestRank << " wins" << endl;
        return 0;
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
    pair<int, int> handsThisRound[10];
    int rankAfterHandFinishes[10];
};

void endGame(vector<player*> &allPlayers) {
    while (!allPlayers.empty()) {
        delete allPlayers.back();
        allPlayers.pop_back();
    }
}

//return true if the first is greater than the second
bool compareMultiplesOf(const pair<int, int> &mult1, const pair<int, int> &mult2) {
    //compare multiples (.second)
    if (mult1.second > mult2.second) {
        return true;
    }
    else if (mult2.second > mult1.second) {
        return false;
    }
    //compare rank if both have same num of one card
    else if (mult1.first > mult2.first) {
        return true;
    }
    else {
        return false;
    }
}

//return true if card 1 is of the suit and stronger
struct straightFlushCompHelper {
    straightFlushCompHelper(string suit_) : suit(suit_) {}
    bool operator()(const Card &card1, const Card &card2) {
        if (card1.getSuit() == suit && card2.getSuit() != suit) return true;
        else if (card2.getSuit() == suit && card1.getSuit() != suit) return false;
        //other suits don't matter here
        else if (card2.getSuit() != suit && card1.getSuit() != suit) return false;
        //both are of the suit, compare ranks
        else {
            if (card1.getRankNum() > card2.getRankNum()) return true;
            else {
                return false;
            }
        }
    }
    string suit;
};

string handRanks[] = {
    "high card",
    "1 pair",
    "2 pair",
    "three of a kind",
    "straight",
    "flush",
    "full house",
    "four of a kind",
    "straight flush"
};

//first int returns rank of hand, second int is rank of card relative to that hand rank
//0 is high card, 1 is pair, 2 is 2 pair, 3 is trips
//4 is straight, 5 is flush, 6 is boat, 7 is quads, 8 is straight flush
pair<int, int> handEval(const Hand &hand, vector<Card> board) {
    board.push_back(hand.first);
    board.push_back(hand.second);
    
    bool isFlush = false;
    bool isStraight = false;
    bool isQuads = false;
    bool isFullHouse = false;
    
    
    sort(board.begin(), board.end());
    //set best pair to high card
    pair<int, int> bestHand = {0, board.back().getRankNum()};
    
    //check for pairs, trips, quads
    //multiples of is a pair of <card rank num, num times on board/hand>
    vector<pair<int, int>> multiplesOf;
    int counter = 1;
    for (unsigned int i = 0; i < board.size() - 1; i++) {
        if (board[i].getRank() == board[i + 1].getRank()) {
            ++counter;
        }
        else if (counter != 1) {
            pair<int, int> pairOrMore = {board[i].getRankNum(), counter};
            multiplesOf.push_back(pairOrMore);
            //reset counter back to 1
            counter = 1;
        }
    }
    //if the last cards were a pair or more, must add to vector
    if (counter != 1) {
        pair<int, int> pairOrMore = {board.back().getRankNum(), counter};
        multiplesOf.push_back(pairOrMore);
    }
    //sort multiplesOf based on number of number of that card (.second)
    //and then by card rank (.first). only if there is at least one pair
    if (!multiplesOf.empty()) {
        sort(multiplesOf.begin(), multiplesOf.end(), compareMultiplesOf);
        //figure out if boat, quads, trips, two pair, or one pair
        //checks quads
        if (multiplesOf.back().second == 4) {
            bestHand = {7, multiplesOf.back().first};
            isQuads = true;
        }
        //checks trips and full house
        else if (multiplesOf.back().second == 3) {
            //check for full boat
            if (multiplesOf.size() > 1) {
                for (int i = (int)(multiplesOf.size()) - 2; i >= 0; --i) {
                    if (multiplesOf[i].second == 2) {
                        bestHand = {6, multiplesOf.back().first};
                        isFullHouse = true;
                        break;
                    }
                }
            }
            else if (!isFullHouse) {
                bestHand = {3, multiplesOf.back().first};
            }
        }
        //checks for pair and two pair
        else if (multiplesOf.back().second == 2) {
            //check for one pair vs two pair
            if (multiplesOf.size() > 1) {
                bestHand = {2, multiplesOf.back().first};
            }
            else {
                bestHand = {1, multiplesOf.back().first};
            }
        }
    }
    
    //check for straight and erase duplicate numbers in temp board
    //fix this later to be written better to erase non unique cards
    //but only when comparing ranks, not suits
    vector<Card> preEraseDuplicates = board;
    vector<Card> tempBoard;
    auto it = preEraseDuplicates.begin();
    auto it2 = preEraseDuplicates.begin() + 1;
    while (it != preEraseDuplicates.end() - 1) {
        if (it->getRank() != it2->getRank()) {
            tempBoard.push_back(*it);
        }
        ++it;
        ++it2;
    }
    tempBoard.push_back(preEraseDuplicates.back());
    int startingCounter = 0;
    //loop that mandates it must be checking at least 5 cards
    while ((tempBoard.size() - startingCounter) > 4) {
        int straightCounter = 0;
        for (int i = startingCounter; i < 4 + startingCounter; i++) {
            if (tempBoard[i].getRankNum() == tempBoard[i + 1].getRankNum() - 1) {
                ++straightCounter;
            }
        }
        if (straightCounter == 5) {
            isStraight = true;
            //hand is only better if not already have quads
            if (!isQuads && !isFullHouse) {
                bestHand.first = 4;
                bestHand.second = 4 + startingCounter;
            }
        }
        ++startingCounter;
    }
    
    //check for wheel straight if user does not already have a straight
    if (!isStraight && (tempBoard.end() - 1)->getRankNum() == 13) {
        auto ptr = tempBoard.begin();
        int wheelStraightNum = 2;
        //while not at the end of board and board num is equal to straight num
        //and the wheelStraightNum less than 6 go to the next number
        while (ptr != tempBoard.end() && (wheelStraightNum == ptr->getRankNum())
               && wheelStraightNum < 6) {
            ++wheelStraightNum;
            ++ptr;
        }
        if (wheelStraightNum == 6) {
            isStraight = true;
            //hand only better if don't already have quads
            if (!isQuads && !isFullHouse) {
                bestHand.first = 4;
                bestHand.second = 5;
            }
        }
    }
    
    
    //check for flush
    string flushSuit;
    for (int i = 0; i < 4; i++) {
        int numInRow = 0;
        string lastOfSuit;
        for (int j = 0; j < 7; j++) {
            if (board[j].getSuit() == suits[i]) {
                ++numInRow;
                lastOfSuit = board[j].getRank();
                flushSuit = board[j].getSuit();
            }
        }
        if (numInRow == 5) {
            isFlush = true;
            //hand only better if they don't already have quads
            if (!isQuads && !isFullHouse) {
                bestHand.first = 5;
                bestHand.second = (cardName.find(lastOfSuit))->second;
            }
            break;
        }
    }
    
    //check for straight flush
    if (isStraight && isFlush) {
        int straightFlushCounter = 1;
        int currentStraightFlushRank = board.back().getRankNum();
        straightFlushCompHelper flushHelp = straightFlushCompHelper(flushSuit);
        sort(board.begin(), board.end(), flushHelp);
        auto it = board.end() - 1;
        auto it2 = board.end() - 2;
        //start at end and compare with the one before it, leaves out the first
        //and keep going until it2 is not of the flush suit and not a straight flush
        while (it != board.begin() && (it2->getSuit() == flushSuit) && straightFlushCounter != 5) {
            if (it->getRankNum() == it2->getRankNum() + 1) {
                ++straightFlushCounter;
            }
            else {
                //resets top card of straight to the next card and counter to 1
                currentStraightFlushRank = it2->getRankNum();
                straightFlushCounter = 1;
            }
        }
        //if the user has a straight flush
        if (straightFlushCounter == 5) {
            bestHand = {8, currentStraightFlushRank};
        }
    }

    return bestHand;
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



