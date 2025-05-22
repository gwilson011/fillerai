#include "game_logic.h"
#include <iostream>

GameState GameState::from_json(const nlohmann::json& j) {
    GameState state;

    try {
        state.board = j.at("board").get<std::vector<std::vector<int>>>();
        state.currentPlayer = j.at("currentPlayer").get<int>();
        state.winner = j.at("winner").get<int>();
    } catch (const std::exception& e) {
        std::cerr << "❌ JSON parsing error: " << e.what() << "\n";
        throw;  // Rethrow so caller can catch and handle
    }

    return state;
}

nlohmann::json GameState::to_json() const {
    return {
        {"board", board},
        {"currentPlayer", currentPlayer},
        {"winner", winner}
    };
}

void GameState::applyPlayerMove() {
    // TODO: validate and apply player move
}

void GameState::applyAIMove() {
    // TODO: simple AI (e.g. choose first valid move)
}

void GameState::checkWinner() {
    // TODO: detect win condition
}
