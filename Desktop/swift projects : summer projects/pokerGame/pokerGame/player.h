//
//  player.h
//  pokerGame, poker player interface
//
//  Created by Seth Saperstein on 9/5/17.
//  Copyright © 2017 Seth Saperstein. All rights reserved.
//

#ifndef player_h
#define player_h

#include <string>
#include <stdio.h>
#include "hand.h"

class player {
public:
    //gets player decision when asked to call fold or raise
    virtual std::string callFoldRaiseDecision() const = 0;
    
    //gets player decision when asked all in or fold
    virtual std::string allInOrFoldDecision() const = 0;
    
    //gets the amount a player wishes to raise
    virtual int raiseAmountDecision() const = 0;
    
    //gets the decision when asked to check fold or bet
    virtual std::string checkFoldOrBetDecision() const = 0;
    
    //gets decision of how much when player decides to bet
    virtual int getBetAmount() const = 0;
    
    virtual const std::string & getName() const = 0;
    
    virtual const int getStackSize() const = 0;
    
    virtual void setStackSize(int size) = 0;
    
    virtual const Card getHandFirst() const = 0;
    
    virtual const Card getHandSecond() const = 0;
    
    virtual void setHandFirst(const Card &card) = 0;
    
    virtual void setHandSecond(const Card &card) = 0;
    
    virtual const int getAmtBetTotal() const = 0;
    
    virtual void setAmtBetTotal(int amount) = 0;
    
    virtual const int getRankAfterHandFinishes() const = 0;
    
    virtual void setRankAfterHandFinishes(int rank) = 0;
    
    virtual ~player() {}
    
};

player * playerFactory(const std::string &name, const std::string &strategy);

#endif /* player_h */
