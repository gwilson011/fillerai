#pragma once
#include <vector>
#include <nlohmann/json.hpp>

struct GameState {
    // In game_logic.h or a global constant
    const int MAX_SEARCH_DEPTH = 4; // Adjust based on board size and performance
    std::vector<std::vector<std::string>> board;
    int currentPlayer;
    int winner;
    std::string move;
    std::vector<std::pair<int, int>> playerBlob;
    std::vector<std::pair<int, int>> aiBlob;
    bool flipped;
    std::string playerColor;
    std::string aiColor;

    static GameState from_json(const nlohmann::json& j);
    nlohmann::json to_json() const;

    void applyPlayerMove();
    void applyAIMove();

    int determineWinner() const; // New name, now const
    void checkWinner();

    GameState copy() const;

    bool isGameOver() const;

    // Helper to get all possible color choices for the current player
    std::vector<std::string> getPossibleMoves() const;

    // Helper to apply a color choice move for a given player (AI or Human)
    // This is a generalized version of your applyPlayerMove logic.
    // It takes the player_id (0 for player, 1 for AI)
    void applyColorMove(const std::string& newColor, int player_id);

    // Determines the score for a finished game from the AI's perspective
    double evaluateState() const;

    std::pair<double, std::string> minimax(GameState state, int depth, bool maximizingPlayer, double alpha, double beta);
};
