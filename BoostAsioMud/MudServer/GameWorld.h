#pragma once

#include "Player.h"
#include "Room.h"

#include <map>
#include <string>
#include <vector>

struct GameMessage {
    int playerId;
    std::string text;
};

struct CommandResult {
    std::vector<GameMessage> messages;
    bool quit = false;
};

class GameWorld {
public:
    std::vector<GameMessage> addPlayer(int playerId);
    std::vector<GameMessage> removePlayer(int playerId);
    CommandResult execute(int playerId, const std::string& input);

private:
    std::string describeRoom(int playerId) const;
    void sendToRoom(std::vector<GameMessage>& messages, int roomId,
                    const std::string& text, int exceptPlayerId = 0) const;

    std::map<int, Room> rooms_;
    std::map<int, Player> players_;
};
