//
//  main.cpp
//  pokerGame
//
//  Created by Seth Saperstein on 9/5/17.
//  Copyright © 2017 Seth Saperstein. All rights reserved.
//

#include "pokerGame.h"

int main(int argc, const char **argv) {
    int playerNameCounter = 0;
    table table1 = table();
    vector<player*> allPlayers;
    while(table1.openSeatAtTable()) {
        player* p1 = playerFactory(to_string(playerNameCounter), "Human");
        allPlayers.push_back(p1);
        table1.addPlayerToTable(p1);
        ++playerNameCounter;
    }
    table1.playTable();
    endGame(allPlayers);
    return 0;
}
