#include "GameWorld.h"

#include <sstream>
#include <utility>

namespace {
constexpr int kStartingRoomId = 1;

std::string oppositeDirection(const std::string& direction) {
    if (direction == "north") return "south";
    if (direction == "south") return "north";
    if (direction == "east") return "west";
    return "east";
}

std::string normalizeCommand(const std::string& command) {
    if (command == "보기" || command == "살펴보기") return "look";
    if (command == "도움" || command == "도움말") return "help";
    if (command == "말" || command == "말하기") return "say";
    if (command == "북" || command == "북쪽") return "north";
    if (command == "남" || command == "남쪽") return "south";
    if (command == "동" || command == "동쪽") return "east";
    if (command == "서" || command == "서쪽") return "west";
    if (command == "종료" || command == "나가기") return "quit";
    return command;
}

std::string koreanDirection(const std::string& direction) {
    if (direction == "north") return "북쪽";
    if (direction == "south") return "남쪽";
    if (direction == "east") return "동쪽";
    return "서쪽";
}
} // namespace

std::vector<GameMessage> GameWorld::addPlayer(int playerId) {
    if (rooms_.empty()) {
        rooms_.emplace(1, Room{1, "오래된 문", "낡은 철문이 던전의 입구를 지키고 있습니다.",
                               {{"east", 3}, {"north", 2}}});
        rooms_.emplace(2, Room{2, "무너진 회랑", "금이 간 기둥들이 고요한 회랑에 늘어서 있습니다.",
                               {{"south", 1}}});
        rooms_.emplace(3, Room{3, "횃불 방", "외로운 횃불 하나가 돌벽을 밝히고 있습니다.",
                               {{"west", 1}}});
    }

    Player player{playerId, "Player" + std::to_string(playerId), kStartingRoomId};
    players_.emplace(playerId, player);

    std::vector<GameMessage> messages;
    messages.push_back({playerId, "환영합니다, " + player.name + "님!\n명령어를 보려면 '도움말'을 입력하세요.\n"});
    messages.push_back({playerId, describeRoom(playerId)});
    sendToRoom(messages, kStartingRoomId, player.name + "님이 세계에 들어왔습니다.\n", playerId);
    return messages;
}

std::vector<GameMessage> GameWorld::removePlayer(int playerId) {
    std::vector<GameMessage> messages;
    const auto player = players_.find(playerId);
    if (player == players_.end()) {
        return messages;
    }

    const int roomId = player->second.roomId;
    const std::string name = player->second.name;
    players_.erase(player);
    sendToRoom(messages, roomId, name + "님이 세계를 떠났습니다.\n");
    return messages;
}

CommandResult GameWorld::execute(int playerId, const std::string& input) {
    CommandResult result;
    auto player = players_.find(playerId);
    if (player == players_.end()) {
        return result;
    }

    std::istringstream words(input);
    std::string command;
    words >> command;
    command = normalizeCommand(command);

    if (command == "look") {
        result.messages.push_back({playerId, describeRoom(playerId)});
    } else if (command == "help") {
        result.messages.push_back({playerId,
            "명령어: 보기, 북쪽, 남쪽, 동쪽, 서쪽, 말 <내용>, 도움말, 종료\n"
            "영어 명령어도 사용할 수 있습니다.\n"});
    } else if (command == "say") {
        std::string message;
        std::getline(words >> std::ws, message);
        if (message.empty()) {
            result.messages.push_back({playerId, "무슨 말을 할까요? 사용법: 말 <내용>\n"});
        } else {
            sendToRoom(result.messages, player->second.roomId,
                       player->second.name + "님의 말: " + message + "\n");
        }
    } else if (command == "north" || command == "south" ||
               command == "east" || command == "west") {
        const Room& oldRoom = rooms_.at(player->second.roomId);
        const auto exit = oldRoom.exits.find(command);
        if (exit == oldRoom.exits.end()) {
            result.messages.push_back({playerId, "그쪽으로는 갈 수 없습니다.\n"});
        } else {
            const int oldRoomId = player->second.roomId;
            sendToRoom(result.messages, oldRoomId,
                       player->second.name + "님이 " + koreanDirection(command) + "으로 떠났습니다.\n", playerId);
            player->second.roomId = exit->second;
            sendToRoom(result.messages, player->second.roomId,
                       player->second.name + "님이 " + koreanDirection(oppositeDirection(command)) + "에서 들어왔습니다.\n",
                       playerId);
            result.messages.push_back({playerId, describeRoom(playerId)});
        }
    } else if (command == "quit") {
        result.messages.push_back({playerId, "안녕히 가세요.\n"});
        result.quit = true;
    } else {
        result.messages.push_back({playerId, "알 수 없는 명령어입니다. '도움말'을 입력해 보세요.\n"});
    }

    return result;
}

std::string GameWorld::describeRoom(int playerId) const {
    const Player& player = players_.at(playerId);
    const Room& room = rooms_.at(player.roomId);

    std::ostringstream text;
    text << "\n[" << room.name << "]\n" << room.description << "\n출구:";
    for (const auto& exit : room.exits) {
        text << ' ' << koreanDirection(exit.first);
    }
    text << "\n플레이어:";
    for (const auto& entry : players_) {
        if (entry.second.roomId == room.id) {
            text << ' ' << entry.second.name;
        }
    }
    // TODO: Ollama may enrich this text later; C++ must still own room state and rules.
    text << "\n\n";
    return text.str();
}

void GameWorld::sendToRoom(std::vector<GameMessage>& messages, int roomId,
                           const std::string& text, int exceptPlayerId) const {
    for (const auto& entry : players_) {
        if (entry.second.roomId == roomId && entry.first != exceptPlayerId) {
            messages.push_back({entry.first, text});
        }
    }
}
