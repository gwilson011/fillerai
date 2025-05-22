#pragma once
#include <vector>
#include <nlohmann/json.hpp>

struct GameState {
    std::vector<std::vector<int>> board;
    int currentPlayer;
    int winner;

    static GameState from_json(const nlohmann::json& j);
    nlohmann::json to_json() const;

    void applyPlayerMove();
    void applyAIMove();
    void checkWinner();
};
