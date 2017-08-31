//  testPokerEval.cpp
//  pokerGame
//
//  Created by Seth Saperstein on 8/25/17.
//  Copyright © 2017 Seth Saperstein. All rights reserved.
//

#include "pokerGame.h"


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

pair<int, int> pairTest(const Hand &pairHand);
pair<int, int> multiPairTest(const Hand &pairHand);
pair<int, int> straightTest(const Hand& straightHand);
pair<int, int> wheelStraight(const Hand &straightHand);
pair<int, int> fullHouseTest(const Hand &fullHouseHand);

int main() {
    Hand pairHand = Hand();
    pairHand.first = Card("spades", "four");
    pairHand.second = Card("clubs", "four");
    pair<int, int> test = pairTest(pairHand);
    assert(test.first == 1);
    assert(test.second == 3);
    //should print 1 pair, 3
    pairHand.first = Card("clubs", "five");
    pairHand.second = Card("dimaonds", "eight");
    test = pairTest(pairHand);
    assert(test.first == 2);
    assert(test.second == 7);
    //should print 2 pair, 7
    pairHand.first = Card("diamonds", "king");
    pairHand.second = Card("diamonds", "ten");
    test = pairTest(pairHand);
    assert(test.first == 2);
    assert(test.second == 12);
    //should print 2 pair, 12
    pairHand.first = Card("diamonds", "nine");
    pairHand.second = Card("spades", "four");
    test = multiPairTest(pairHand);
    assert(test.first == 2);
    assert(test.second == 12);
    //should print 2 pair, 12
    pairHand.first = Card("spades", "king");
    pairHand.second = Card("spades", "three");
    test = multiPairTest(pairHand);
    assert(test.first == 3);
    assert(test.second == 12);
    //should print 3 of a kind, 12
    pairHand.first = Card("spades", "king");
    pairHand.second = Card("clubs", "king");
    test = multiPairTest(pairHand);
    assert(test.first == 7);
    assert(test.second == 12);
    //should print 4 of a kind, 12
    pairHand.first = Card("spades", "king");
    pairHand.second = Card("clubs", "four");
    test = multiPairTest(pairHand);
    assert(test.first == 6);
    assert(test.second == 12);
    //should print full house, 12
    pairHand.first = Card("spades", "queen");
    pairHand.second = Card("clubs", "four");
    test = straightTest(pairHand);
    assert(test.first == 4);
    assert(test.second == 12);
    //should print straight, 12
    pairHand.first = Card("spades", "queen");
    pairHand.second = Card("spades", "ace");
    test = straightTest(pairHand);
    assert(test.first == 4);
    assert(test.second == 13);
    //should print straight, 13
    pairHand.first = Card("spades", "queen");
    pairHand.second = Card("clubs", "king");
    test = straightTest(pairHand);
    assert(test.first == 4);
    assert(test.second == 12);
    //should print straight flush, 12
    pairHand.first = Card("hearts", "queen");
    pairHand.second = Card("hearts", "ten");
    test = straightTest(pairHand);
    assert(test.first == 8);
    assert(test.second == 12);
    //should print straight flush, 12
    pairHand.first = Card("clubs", "queen");
    pairHand.second = Card("hearts", "ten");
    test = wheelStraight(pairHand);
    assert(test.first == 4);
    assert(test.second == 4);
    //should print straight, 5
    pairHand.first = Card("clubs", "queen");
    pairHand.second = Card("hearts", "six");
    test = wheelStraight(pairHand);
    assert(test.first == 4);
    assert(test.second == 5);
    //should print straight, 6
    pairHand.first = Card("spades", "two");
    pairHand.second = Card("spades", "six");
    test = fullHouseTest(pairHand);
    assert(test.first == 6);
    assert(test.second == 1);
    //should print full house, 2
    pairHand.first = Card("spades", "two");
    pairHand.second = Card("spades", "three");
    test = fullHouseTest(pairHand);
    assert(test.first == 6);
    assert(test.second == 2);
    //should print full house, 2
    
    return 0;
}

pair<int, int> pairTest(const Hand &pairHand) {
    //test simple pair hand/board
    
    Card c1 = Card("diamonds", "five");
    Card c2 = Card("clubs", "eight");
    Card c3 = Card("hearts", "king");
    Card c4 = Card("hearts", "nine");
    Card c5 = Card("hearts", "ten");
    
    vector<Card> board;
    board.push_back(c1);
    board.push_back(c2);
    board.push_back(c3);
    board.push_back(c4);
    board.push_back(c5);
    pair<int, int> pair = handEval(pairHand, board);
    cout << handRanks[pair.first] << endl << pair.second << endl;
    return pair;
}

pair<int, int> multiPairTest(const Hand &pairHand) {
    Card c1 = Card("diamonds", "king");
    Card c2 = Card("clubs", "ten");
    Card c3 = Card("hearts", "king");
    Card c4 = Card("hearts", "nine");
    Card c5 = Card("hearts", "four");
    
    vector<Card> board;
    board.push_back(c1);
    board.push_back(c2);
    board.push_back(c3);
    board.push_back(c4);
    board.push_back(c5);
    pair<int, int> pair = handEval(pairHand, board);
    cout << handRanks[pair.first] << endl << pair.second << endl;
    return pair;
}

pair<int, int> straightTest(const Hand &straightHand) {
    Card c1 = Card("diamonds", "king");
    Card c2 = Card("clubs", "ten");
    Card c3 = Card("hearts", "king");
    Card c4 = Card("hearts", "nine");
    Card c5 = Card("hearts", "jack");
    
    vector<Card> board;
    board.push_back(c1);
    board.push_back(c2);
    board.push_back(c3);
    board.push_back(c4);
    board.push_back(c5);
    pair<int, int> pair = handEval(straightHand, board);
    cout << handRanks[pair.first] << endl << pair.second << endl;
    return pair;
}

pair<int, int> wheelStraight(const Hand &straightHand) {
    Card c1 = Card("diamonds", "ace");
    Card c2 = Card("clubs", "two");
    Card c3 = Card("hearts", "three");
    Card c4 = Card("hearts", "four");
    Card c5 = Card("hearts", "five");
    
    vector<Card> board;
    board.push_back(c1);
    board.push_back(c2);
    board.push_back(c3);
    board.push_back(c4);
    board.push_back(c5);
    pair<int, int> pair = handEval(straightHand, board);
    cout << handRanks[pair.first] << endl << pair.second << endl;
    return pair;
}

pair<int, int> fullHouseTest(const Hand &fullHouseHand) {
    Card c1 = Card("diamonds", "two");
    Card c2 = Card("clubs", "two");
    Card c3 = Card("hearts", "three");
    Card c4 = Card("hearts", "three");
    Card c5 = Card("hearts", "five");
    
    vector<Card> board;
    board.push_back(c1);
    board.push_back(c2);
    board.push_back(c3);
    board.push_back(c4);
    board.push_back(c5);
    pair<int, int> pair = handEval(fullHouseHand, board);
    cout << handRanks[pair.first] << endl << pair.second << endl;
    return pair;
}
