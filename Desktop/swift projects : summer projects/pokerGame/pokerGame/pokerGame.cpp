//
//  pokerGame.cpp
//  pokerGame
//
//  Created by Seth Saperstein on 8/20/17.
//  Copyright © 2017 Seth Saperstein. All rights reserved.
//

#include "pokerGame.h"

using namespace std;

deckOfCards::deckOfCards() {
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 4; j++) {
            deck.push_back(Card(suits[j], ranksOfCards[i]));
        }
    }
    random_shuffle(deck.begin(), deck.end());
}

void deckOfCards::resetDeck() {
    deck.clear();
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 4; j++) {
            deck.push_back(Card(suits[j], ranksOfCards[i]));
        }
    }
    random_shuffle(deck.begin(), deck.end());
}

Card deckOfCards::getCard() {
    Card chosen = deck.front();
    deck.pop_front();
    return chosen;
}

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


table::table() : smallBlind(10), bigBlind(20), numPlayers(0), pot(0), maxBetTotal(0),
initialBetThisRound(0), numPlayersFolded(0), numPlayersInHand(0) {
    for (int i = 0; i < 10; i++) {
        players[i] = nullptr;
    }
    
}

void table::playTable() {
    chooseDealer();
    while(numPlayers > 1) {
        numPlayersInHand = numPlayers;
        numPlayersFolded = 0;
        board.clear();
        playHand();
        //remove players who went broke at end of hand
        for (int i = 0; i < 10; i++) {
            if (players[i] && players[i]->getStackSize() == 0) {
                players[i] = nullptr;
                --numPlayers;
                inHand[i] = false;
            }
        }
        
        nextDeal();
    }
    for (int i = 0; i < 10; i++) {
        if (players[i]) {
            cout << players[i]->getName() << " wins the game!" << endl << endl;
        }
    }
}
void table::playHand() {
    dealCards();
    round = 0;
    preFlopActivity();
    //only show the board if there is more than 1 person in
    //whether they're all in or just betting
    int playersInPot = numPlayers - numPlayersFolded;
    if (playersInPot > 1) {
        ++round;
        flop();
    }
    playersInPot = numPlayers - numPlayersFolded;
    if (playersInPot > 1) {
        ++round;
        turn();
    }
    playersInPot = numPlayers - numPlayersFolded;
    if (playersInPot > 1) {
        ++round;
        river();
    }
    cout << "End hand." << endl;
    
    for (int i = 0; i < 10; i++) {
        if (players[i]) {
            cout << players[i]->getName() << " has " << players[i]->getStackSize() << endl;
        }
    }
    cout << "Pot: " << pot << endl << endl;
    findWinner();
    
    for (int i = 0; i < 10; i++) {
        if (inHand[i]) {
            cout << players[i]->getName() << "'s hand: " << endl
            << players[i]->getHandFirst() << endl << players[i]->getHandSecond()
            << endl << endl;
        }
    }
    
    splitPotBasedOnRank();
    
    for (int i = 0; i < 10; i++) {
        if (players[i]) {
            cout << players[i]->getName() << " 's stack: " << players[i]->getStackSize() << endl;
        }
    }
}

void table::splitPotBasedOnRank() {
    //create temp vector to sort to payout from pot
    deque<player*> tempPlayers;
    //initialize temp vector with array of players that are not nullptr
    for (int i = 0; i < 10; i++) {
        if (players[i]) {
            tempPlayers.push_back(players[i]);
        }
    }
    
    //check to see if everyone else folded
    if ((int)(tempPlayers.size()) - numPlayersFolded == 1) {
        (tempPlayers.front())->setStackSize(pot + (tempPlayers.front())->getStackSize());
        pot = 0;
        cout << (tempPlayers.front())->getName() << " wins!" << endl << endl;
        return;
    }
    //sort based on playerRankComparator. inHand and nullptr taken care of already
    sort(tempPlayers.begin(), tempPlayers.end(), playerRankComparator());
    
    cout << (tempPlayers.front())->getName() << " wins!" << endl << endl;
    
    int splitNumWays = 1;
    
    while (pot != 0) {
        //check to see if there will be a split pot
        for (int i = 0; i < tempPlayers.size(); i++) {
            if ((i != (int)(tempPlayers.size()) - 1) &&
                tempPlayers[i]->getRankAfterHandFinishes() == tempPlayers[i + 1]->getRankAfterHandFinishes()) {
                ++splitNumWays;
            }
            else {
                break;
            }
        }
        //subtract the num of front person's from all players. add to player in front
        //if tied, subtract then divide then add to x players that split
        int total = 0;
        int amountToTake = tempPlayers[0]->getAmtBetTotal();
        for (int i = 0; i < tempPlayers.size(); i++) {
            //if player doesn't have the amtBetTotal of first, take what they have
            if (tempPlayers[i]->getAmtBetTotal() < amountToTake) {
                total += tempPlayers[i]->getAmtBetTotal();
                pot -= tempPlayers[i]->getAmtBetTotal();
                tempPlayers[i]->setAmtBetTotal(0);
            }
            //else take the amtBetTotal amount from player
            else {
                total += amountToTake;
                tempPlayers[i]->setAmtBetTotal(tempPlayers[i]->getAmtBetTotal() - amountToTake);
                pot -= amountToTake;
            }
        }
        int amtPerPlayer = total / splitNumWays;
        //if more than one person splits the amt between them
        for (int i = 0; i < splitNumWays; i++) {
            tempPlayers[i]->setStackSize(tempPlayers[i]->getStackSize() + amtPerPlayer);
        }
        //get rid of players in front who can't make any more
        while (!tempPlayers.empty() && (tempPlayers.front())->getAmtBetTotal() == 0) {
            tempPlayers.pop_front();
        }
    }
}

//return -1 if player2 is stronger, 0 if tied, 1 if player 1 is stronger
int table::highCardComparison(const vector<Card> &player1Cards, const vector<Card> &player2Cards) {
    int counter = 0;
    auto it1 = player1Cards.end() - 1;
    auto it2 = player2Cards.end() - 1;
    while (counter < 5) {
        if (it1->getRankNum() < it2->getRankNum()) return -1;
        else if (it1->getRankNum() > it2->getRankNum()) return 1;
        ++counter;
        --it1;
        --it2;
    }
    return 0;
}

void table::closerCompareOfHands(int positionOfP1, int positionOfP2) {
    //add players hand to board in new vec, then sort
    vector<Card> player1Cards = vector<Card>(board);
    player1Cards.push_back(players[positionOfP1]->getHandFirst());
    player1Cards.push_back(players[positionOfP1]->getHandSecond());
    sort(player1Cards.begin(), player1Cards.end());
    
    //add players hand to board in new vec, then sort
    vector<Card> player2Cards = vector<Card>(board);
    player2Cards.push_back(players[positionOfP2]->getHandFirst());
    player2Cards.push_back(players[positionOfP2]->getHandSecond());
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
                //rankAfterHandFinishes[positionOfP2] += 10;
                players[positionOfP2]->setRankAfterHandFinishes(players[positionOfP2]->getRankAfterHandFinishes() + 10);
            }
            else if (playerComp == 0) {
                //rankAfterHandFinishes[positionOfP1] += 1;
                //rankAfterHandFinishes[positionOfP2] += 1;
                players[positionOfP1]->setRankAfterHandFinishes(players[positionOfP1]->getRankAfterHandFinishes() + 1);
                players[positionOfP2]->setRankAfterHandFinishes(players[positionOfP2]->getRankAfterHandFinishes() + 1);
            }
            else if (playerComp == 1) {
                //rankAfterHandFinishes[positionOfP1] += 10;
                players[positionOfP1]->setRankAfterHandFinishes(players[positionOfP1]->getRankAfterHandFinishes() + 10);
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
                //rankAfterHandFinishes[positionOfP2] += 10;
                players[positionOfP2]->setRankAfterHandFinishes(players[positionOfP2]->getRankAfterHandFinishes() + 10);
            }
            else if (playerComp == 0) {
                //rankAfterHandFinishes[positionOfP1] += 1;
                //rankAfterHandFinishes[positionOfP2] += 1;
                players[positionOfP1]->setRankAfterHandFinishes(players[positionOfP1]->getRankAfterHandFinishes() + 1);
                players[positionOfP2]->setRankAfterHandFinishes(players[positionOfP2]->getRankAfterHandFinishes() + 1);
            }
            else if (playerComp == 1) {
                //rankAfterHandFinishes[positionOfP1] += 10;
                players[positionOfP1]->setRankAfterHandFinishes(players[positionOfP1]->getRankAfterHandFinishes() + 10);
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
                //rankAfterHandFinishes[positionOfP1] += 10;
                players[positionOfP1]->setRankAfterHandFinishes(players[positionOfP1]->getRankAfterHandFinishes() + 10);
            }
            else if (nextP2Pair > nextP1Pair) {
                //rankAfterHandFinishes[positionOfP2] += 10;
                players[positionOfP2]->setRankAfterHandFinishes(players[positionOfP2]->getRankAfterHandFinishes() + 10);
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
                    //rankAfterHandFinishes[positionOfP1] += 10;
                    players[positionOfP1]->setRankAfterHandFinishes(players[positionOfP1]->getRankAfterHandFinishes() + 10);
                }
                else if (highCardP2 > highCardP1) {
                    //rankAfterHandFinishes[positionOfP2] += 10;
                    players[positionOfP2]->setRankAfterHandFinishes(players[positionOfP2]->getRankAfterHandFinishes() + 10);
                }
                else {
                    //rankAfterHandFinishes[positionOfP1] += 1;
                    //rankAfterHandFinishes[positionOfP2] += 1;
                    players[positionOfP1]->setRankAfterHandFinishes(players[positionOfP1]->getRankAfterHandFinishes() + 1);
                    players[positionOfP2]->setRankAfterHandFinishes(players[positionOfP2]->getRankAfterHandFinishes() + 1);
                }
            }
            break;
            //straight. straight to same card. auto tie
        case 4:
            //rankAfterHandFinishes[positionOfP1] += 1;
            //rankAfterHandFinishes[positionOfP2] += 1;
            players[positionOfP1]->setRankAfterHandFinishes(players[positionOfP1]->getRankAfterHandFinishes() + 1);
            players[positionOfP2]->setRankAfterHandFinishes(players[positionOfP2]->getRankAfterHandFinishes() + 1);
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
                //rankAfterHandFinishes[positionOfP1] += 10;
                players[positionOfP1]->setRankAfterHandFinishes(players[positionOfP1]->getRankAfterHandFinishes() + 10);
            }
            else if (firstP2Pair > firstP1Pair) {
                //rankAfterHandFinishes[positionOfP2] += 10;
                players[positionOfP2]->setRankAfterHandFinishes(players[positionOfP2]->getRankAfterHandFinishes() + 10);
            }
            else {
                //rankAfterHandFinishes[positionOfP2] += 1;
                //rankAfterHandFinishes[positionOfP1] += 1;
                players[positionOfP1]->setRankAfterHandFinishes(players[positionOfP1]->getRankAfterHandFinishes() + 1);
                players[positionOfP2]->setRankAfterHandFinishes(players[positionOfP2]->getRankAfterHandFinishes() + 1);
            }
            break;
        default:
            cerr << "something wrong in closer compare of hands function" << endl;
            exit(1);
            break;
    }
}


void table::findWinner() {
    for (int i = 0; i < 10; i++) {
        if (inHand[i]) {
            Hand tempHand = Hand();
            tempHand.first = players[i]->getHandFirst();
            tempHand.second = players[i]->getHandSecond();
            pair<int, int> handForRound = handEval(tempHand, board);
            handsThisRound[i] = handForRound;
        }
        //rankAfterHandFinishes[i] = 0;
        if (players[i]) {
            players[i]->setRankAfterHandFinishes(0);
        }
    }
    //sort-like algorithm to compare everyone in hand to everyone else
    //in the hand. awarding 10 pts for the winner of the 2 hands
    //and in the case of a tie 1 to both
    for (int i = 0; i < 9; i++) {
        if (inHand[i]) {
            for (int j = i + 1; j < 10; j++) {
                if (inHand[j]) {
                    if (handsThisRound[i].first > handsThisRound[j].first) {
                        //rankAfterHandFinishes[i] += 10;
                        players[i]->setRankAfterHandFinishes(players[i]->getRankAfterHandFinishes() + 10);
                    }
                    else if (handsThisRound[j].first > handsThisRound[i].first) {
                        //rankAfterHandFinishes[j] += 10;
                        players[j]->setRankAfterHandFinishes(players[j]->getRankAfterHandFinishes() + 10);
                    }
                    else if (handsThisRound[i].second > handsThisRound[j].second) {
                        //rankAfterHandFinishes[i] += 10;
                        players[i]->setRankAfterHandFinishes(players[i]->getRankAfterHandFinishes() + 10);
                    }
                    else if (handsThisRound[j].second > handsThisRound[i].second) {
                        //rankAfterHandFinishes[j] += 10;
                        players[j]->setRankAfterHandFinishes(players[j]->getRankAfterHandFinishes() + 10);
                    }
                    else {
                        closerCompareOfHands(i, j);
                    }
                }
            }
        }
    }
}

void table::chooseDealer() {
    deckOfCards dealerDeck = deckOfCards();
    cout << endl << "High card for deal." << endl;
    Card maxCard = Card("spades", "two");
    for (int i = 0; i < 10; i++) {
        if (players[i]) {
            inHand[i] = true;
            Card forDeal = dealerDeck.getCard();
            players[i]->setHandFirst(forDeal);
            cout << players[i]->getName() << " gets a " <<
            forDeal.getRank() << "." << endl;
            if (players[i]->getHandFirst() > maxCard) {
                dealer = i;
                maxCard = players[i]->getHandFirst();
            }
        }
    }
    cout << players[dealer]->getName() << " deals." << endl << endl;
}

void table::addPlayerToTable(player *person) {
    for (int i = 0; i < 10; i++) {
        if (!players[i]) {
            players[i] = person;
            ++numPlayers;
            cout << "Added " << person->getName() << " to the table." << endl;
            return;
        }
    }
    cout << "cannot add person to table" << endl;
    return;
}

bool table::openSeatAtTable() {
    if (numPlayers != 10) return true;
    return false;
}

//reset things for hand here
void table::dealCards() {
    cout << "Dealing cards." << endl;
    Deck.resetDeck();
    for (int i = 0; i < 10; i++) {
        if (players[i]) {
            players[i]->setHandFirst(Deck.getCard());
            inHand[i] = true;
            players[i]->setAmtBetTotal(0);
        }
    }
    for (int i = 0; i < 10; i++) {
        if (players[i]) players[i]->setHandSecond(Deck.getCard());
    }
}

int table::getNumPlayers() const {
    return numPlayers;
}

void table::next() {
    if (currentlyOn == 9) {
        currentlyOn = 0;
        while (!(inHand[currentlyOn]) || !players[currentlyOn]) {
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

void table::nextDeal() {
    if (dealer == 9) {
        dealer = 0;
        while (!players[dealer]) {
            ++dealer;
        }
        return;
    }
    else {
        ++dealer;
        if (players[dealer]) return;
        else{
            nextDeal();
        }
    }
    return;
}

void table::fold() {
    cout << players[currentlyOn]->getName() << " folds." << endl << endl;
    --numPlayersInHand;
    ++numPlayersFolded;
    inHand[currentlyOn] = false;
}

string table::callDecision() {
    cout << "call, fold, or raise?" << endl;
    string decision = players[currentlyOn]->callFoldRaiseDecision(botHandEval(players[currentlyOn]->getHand(), board, false), botHandEval(players[currentlyOn]->getHand(), board, true), round);
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

void table::allInOrFold() {
    cout << "call or fold?" << endl;
    string decision;
    decision = players[currentlyOn]->allInOrFoldDecision(botHandEval(players[currentlyOn]->getHand(), board, false), botHandEval(players[currentlyOn]->getHand(), board, true), round);
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

void table::allIn() {
    --numPlayersInHand;
    cout << players[currentlyOn]->getName() << " is all in for "
    << players[currentlyOn]->getStackSize() << endl << endl;
    pot += players[currentlyOn]->getStackSize();
    players[currentlyOn]->setAmtBetTotal(players[currentlyOn]->getAmtBetTotal()
                                         + players[currentlyOn]->getStackSize());
    players[currentlyOn]->setStackSize(0);
    if (players[currentlyOn]->getAmtBetTotal() > maxBetTotal) {
        maxBetTotal = players[currentlyOn]->getAmtBetTotal();
    }
}

void table::raiseAmount() {
    assert(inHand[currentlyOn]);
    assert(players[currentlyOn]->getStackSize() > 0);
    cout << "Stacksize: " << players[currentlyOn]->getStackSize() << endl;
    cout << "How much would you like to raise?" << endl;
    int betSize;
    betSize = players[currentlyOn]->raiseAmountDecision(botHandEval(players[currentlyOn]->getHand(), board, false), botHandEval(players[currentlyOn]->getHand(), board, true), round);
    //if the player decides to go all in on top
    if ((maxBetTotal + betSize) >= players[currentlyOn]->getStackSize()) {
        allIn();
        return;
    }
    //if the player bets less than the min raise ask them again
    else if (betSize < initialBetThisRound) {
        cout << "Minimum raise is " << initialBetThisRound << endl;
        return raiseAmount();
    }
    //when the player bets min or above and less than all in
    else if (betSize >= initialBetThisRound && betSize < players[currentlyOn]->getStackSize()) {
        cout << players[currentlyOn]->getName() << " raises " << betSize << endl << endl;
        pot += maxBetTotal - players[currentlyOn]->getAmtBetTotal() + betSize;
        players[currentlyOn]->setStackSize(players[currentlyOn]->getStackSize() - maxBetTotal - players[currentlyOn]->getAmtBetTotal() - betSize);
        players[currentlyOn]->setAmtBetTotal(maxBetTotal + betSize);
        maxBetTotal = players[currentlyOn]->getAmtBetTotal();
    }
    else {
        assert(false);
    }
}

void table::readPlayerHand() {
    cout << players[currentlyOn]->getName() << "'s hand: " << endl;
    cout << players[currentlyOn]->getHandFirst().getRank() << " of "
    << players[currentlyOn]->getHandFirst().getSuit() << endl;
    cout << players[currentlyOn]->getHandSecond().getRank() << " of "
    << players[currentlyOn]->getHandSecond().getSuit() << endl;
    cout << "Stack size: " << players[currentlyOn]->getStackSize() << endl;
    cout << "Pot: " << pot << endl << endl;
}

void table::checkFoldOrBet() {
    cout << "check, fold, or bet?" << endl;
    string decision;
    decision = players[currentlyOn]->checkFoldOrBetDecision(botHandEval(players[currentlyOn]->getHand(), board, false), botHandEval(players[currentlyOn]->getHand(), board, true), round);
    if (decision == "check") {
        cout << players[currentlyOn]->getName() << " checks." << endl << endl;
        return;
    }
    else if (decision == "fold") {
        fold();
        return;
    }
    else if (decision == "bet") {
        cout << "How much?" << endl;
        int betAmount = 0;
        betAmount = players[currentlyOn]->getBetAmount();
        cout << endl;
        if (betAmount >= players[currentlyOn]->getStackSize()) {
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
            players[currentlyOn]->setAmtBetTotal(players[currentlyOn]->getAmtBetTotal() + betAmount);
            players[currentlyOn]->setStackSize(players[currentlyOn]->getStackSize() - betAmount);
            return;
        }
    }
    else {
        cout << "that is not an option." << endl;
        return checkFoldOrBet();
    }
}

void table::bettingRound() {
    //num players in hand is greater than 1 and either there hasn't
    //been a bet yet or the maxbettotal > amtbettotal of player
    int startingActionPlayer = currentlyOn;
    while (((players[currentlyOn]->getAmtBetTotal() < maxBetTotal)
            || (!betInRound)) && (numPlayersInHand >= 1)) {
        //change the starting action player if the player folds to know
        //when to end when it gets back around
        if (!inHand[startingActionPlayer]) {
            startingActionPlayer = currentlyOn;
        }
        //if the players in the hand and not all in
        if (inHand[currentlyOn] && players[currentlyOn]->getStackSize() != 0) {
            readPlayerHand();
            //if the player needs to call someone's bet or the blind
            if (maxBetTotal > players[currentlyOn]->getAmtBetTotal()) {
                int toCall = maxBetTotal - players[currentlyOn]->getAmtBetTotal();
                cout << players[currentlyOn]->getName() << " to call " << toCall
                << endl;
                string decision;
                //if the player will not be all in if they call.
                //fold taken care of in callDecision()
                if (toCall < players[currentlyOn]->getStackSize()) {
                    decision = callDecision();
                    if (decision == "call") {
                        cout << players[currentlyOn]->getName() << " calls." << endl << endl;
                        pot += toCall;
                        players[currentlyOn]->setStackSize(players[currentlyOn]->getStackSize() - toCall);
                        players[currentlyOn]->setAmtBetTotal(players[currentlyOn]->getAmtBetTotal() + toCall);
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
                else if (toCall >= players[currentlyOn]->getStackSize()) {
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

void table::preFlopActivity() {
    pot = 0;
    numPlayersInHand = numPlayers;
    currentlyOn = dealer;
    betInRound = true;
    next();
    //if sidepot is needed for SB
    if (players[currentlyOn]->getStackSize() < smallBlind) {
        pot += players[currentlyOn]->getStackSize();
        players[currentlyOn]->setAmtBetTotal(players[currentlyOn]->getStackSize());
        players[currentlyOn]->setStackSize(0);
        maxBetTotal = pot;
        cout << "sidepot small blind" << endl;
    }
    else {
        players[currentlyOn]->setStackSize(players[currentlyOn]->getStackSize() - smallBlind);
        players[currentlyOn]->setAmtBetTotal(smallBlind);
        maxBetTotal = smallBlind;
        pot += smallBlind;
        cout << players[currentlyOn]->getName() << " posts the small blind." << endl;
    }
    next();
    //if sidepot is needed for BB
    if (players[currentlyOn]->getStackSize() < bigBlind) {
        pot += players[currentlyOn]->getStackSize();
        players[currentlyOn]->setAmtBetTotal(players[currentlyOn]->getStackSize());
        if (players[currentlyOn]->getStackSize() > maxBetTotal) {
            maxBetTotal = players[currentlyOn]->getStackSize();
        }
        players[currentlyOn]->setStackSize(0);
        cout << "sidepot big blind" << endl;
    }
    else {
        players[currentlyOn]->setStackSize(players[currentlyOn]->getStackSize() - bigBlind);
        players[currentlyOn]->setAmtBetTotal(players[currentlyOn]->getAmtBetTotal() + bigBlind);
        pot += bigBlind;
        maxBetTotal = bigBlind;
        cout << players[currentlyOn]->getName() << " posts the big blind." << endl << endl;
    }
    initialBetThisRound = bigBlind;
    next();
    bettingRound();
}

void table::flop() {
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

void table::turn() {
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

void table::river() {
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
bool playerRankComparator::operator()(const player* player1, const player* player2) {
    if (player1->getRankAfterHandFinishes() > player2->getRankAfterHandFinishes()) {
        return true;
    }
    else if (player2->getRankAfterHandFinishes() > player1->getRankAfterHandFinishes()) {
        return false;
    }
    else if (player1->getAmtBetTotal() < player2->getAmtBetTotal()) {
        return true;
    }
    else {
        return false;
    }
}

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
straightFlushCompHelper::straightFlushCompHelper(string suit_) : suit(suit_) {}
bool straightFlushCompHelper::operator()(const Card &card1, const Card &card2) {
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

//first int returns rank of hand, second int is rank of card relative to that hand rank
//0 is high card, 1 is pair, 2 is 2 pair, 3 is trips
//4 is straight, 5 is flush, 6 is boat, 7 is quads, 8 is straight flush
pair<int, int> handEval(const Hand &hand, vector<Card> &board) {
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
        if (multiplesOf.front().second == 4) {
            bestHand = {7, multiplesOf.front().first};
            isQuads = true;
        }
        //checks trips and full house
        else if (multiplesOf.front().second == 3) {
            //check for full boat
            if (multiplesOf.size() > 1) {
                for (int i = 1; i < multiplesOf.size(); i++) {
                    if (multiplesOf[i].second >= 2) {
                        bestHand = {6, multiplesOf.front().first};
                        isFullHouse = true;
                        break;
                    }
                }
            }
            else if (!isFullHouse) {
                bestHand = {3, multiplesOf.front().first};
            }
        }
        //checks for pair and two pair
        else if (multiplesOf.front().second == 2) {
            //check for one pair vs two pair
            if (multiplesOf.size() > 1) {
                bestHand = {2, multiplesOf.front().first};
            }
            else {
                bestHand = {1, multiplesOf.front().first};
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
        if (straightCounter == 4) {
            isStraight = true;
            //hand is only better if not already have quads
            if (!isQuads && !isFullHouse) {
                bestHand.first = 4;
                bestHand.second = tempBoard[4 + startingCounter].getRankNum();
            }
        }
        ++startingCounter;
    }
    
    //check for wheel straight if user does not already have a straight
    if (!isStraight && (tempBoard.end() - 1)->getRankNum() == 13) {
        auto ptr = tempBoard.begin();
        int wheelStraightNum = 1;
        //while not at the end of board and board num is equal to straight num
        //and the wheelStraightNum less than 6 go to the next number
        while (ptr != tempBoard.end() && (wheelStraightNum == ptr->getRankNum())
               && wheelStraightNum < 5) {
            ++wheelStraightNum;
            ++ptr;
        }
        if (wheelStraightNum == 5) {
            isStraight = true;
            //hand only better if don't already have quads
            if (!isQuads && !isFullHouse) {
                bestHand.first = 4;
                bestHand.second = 4;
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
        straightFlushCompHelper flushHelp = straightFlushCompHelper(flushSuit);
        sort(board.begin(), board.end(), flushHelp);
        int counter = 0;
        while (board[counter + 4].getSuit() == flushSuit) {
            int straightFlushCounter = 1;
            for (int i = 0; i < 4; i++) {
                if (board[i].getRankNum() == board[i+1].getRankNum() + 1) {
                    ++straightFlushCounter;
                }
            }
            if (straightFlushCounter == 5) {
                bestHand = {8, board[counter].getRankNum()};
            }
            ++counter;
        }
    }
    
    return bestHand;
}

//FOR BOT USE
//first int returns rank of hand, second int is rank of card relative to that hand rank
//0 is high card, 1 is pair, 2 is 2 pair, 3 is trips
//4 is straight, 5 is flush, 6 is boat, 7 is quads, 8 is straight flush
pair<int, int> botHandEval(const Hand &hand, vector<Card> &board, bool withHand) {
    if (withHand) {
        board.push_back(hand.first);
        board.push_back(hand.second);
    }
    
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
        if (multiplesOf.front().second == 4) {
            bestHand = {7, multiplesOf.front().first};
            isQuads = true;
        }
        //checks trips and full house
        else if (multiplesOf.front().second == 3) {
            //check for full boat
            if (multiplesOf.size() > 1) {
                for (int i = 1; i < multiplesOf.size(); i++) {
                    if (multiplesOf[i].second >= 2) {
                        bestHand = {6, multiplesOf.front().first};
                        isFullHouse = true;
                        break;
                    }
                }
            }
            else if (!isFullHouse) {
                bestHand = {3, multiplesOf.front().first};
            }
        }
        //checks for pair and two pair
        else if (multiplesOf.front().second == 2) {
            //check for one pair vs two pair
            if (multiplesOf.size() > 1) {
                bestHand = {2, multiplesOf.front().first};
            }
            else {
                bestHand = {1, multiplesOf.front().first};
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
        if (straightCounter == 4) {
            isStraight = true;
            //hand is only better if not already have quads
            if (!isQuads && !isFullHouse) {
                bestHand.first = 4;
                bestHand.second = tempBoard[4 + startingCounter].getRankNum();
            }
        }
        ++startingCounter;
    }
    
    //check for wheel straight if user does not already have a straight
    if (!isStraight && (tempBoard.end() - 1)->getRankNum() == 13) {
        auto ptr = tempBoard.begin();
        int wheelStraightNum = 1;
        //while not at the end of board and board num is equal to straight num
        //and the wheelStraightNum less than 6 go to the next number
        while (ptr != tempBoard.end() && (wheelStraightNum == ptr->getRankNum())
               && wheelStraightNum < 5) {
            ++wheelStraightNum;
            ++ptr;
        }
        if (wheelStraightNum == 5) {
            isStraight = true;
            //hand only better if don't already have quads
            if (!isQuads && !isFullHouse) {
                bestHand.first = 4;
                bestHand.second = 4;
            }
        }
    }
    
    
    //check for flush if enough cards
    string flushSuit;
    if (board.size() >= 3) {
        for (int i = 0; i < 4; i++) {
            int numInRow = 0;
            string lastOfSuit;
            for (int j = 0; j < 2 + (int)(board.size()); j++) {
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
    }
    
    //check for straight flush
    if (isStraight && isFlush) {
        bool straightFlush = false;
        straightFlushCompHelper flushHelp = straightFlushCompHelper(flushSuit);
        sort(board.begin(), board.end(), flushHelp);
        int counter = 0;
        while (board[counter + 4].getSuit() == flushSuit) {
            int straightFlushCounter = 1;
            for (int i = 0; i < 4; i++) {
                if (board[i].getRankNum() == board[i+1].getRankNum() + 1) {
                    ++straightFlushCounter;
                }
            }
            if (straightFlushCounter == 5) {
                straightFlush = true;
                bestHand = {8, board[counter].getRankNum()};
            }
            ++counter;
        }
        
        //check straight flush from a2345
        counter = 0;
        int straightCounter = 4;
        if (!straightFlush && board.front().getRankNum() == 13) {
            auto it = board.begin();
            while (it != board.end() && it->getRankNum() != 4) {
                ++it;
            }
            while (it != board.end() && it->getRankNum() == straightCounter
                && it->getSuit() == flushSuit) {
                --straightCounter;
                ++it;
            }
            if (straightCounter == 0) {
                bestHand = {8, 4};
            }
        }
    }
    
    
    return bestHand;
}
