#include "game_logic.h"
#include <iostream>
#include <algorithm> // For std::find, std::max
#include <utility>   // For std::pair
#include <queue>
#include <set>
#include <map>   // For frequency counting of colors
#include <limits> // For numeric_limits
#include <random> // For random number generation
#include <chrono> // For seeding random number generator


const double WEIGHT_AI_BLOB_SIZE = 3.0;
const double WEIGHT_PLAYER_BLOB_SIZE = -2.0; // Negative because smaller is better for AI
const double WEIGHT_ADJACENT_ENEMY_TILES = 1.0; // Encourage capturing enemy cells
const double WEIGHT_AVAILABLE_COLORS = 0.5; // Encourage having more color choices


// std::vector<std::pair<int, int>> playerBlob;

GameState GameState::from_json(const nlohmann::json& j) {
    GameState state;

    try {
        state.board = j.at("board").get<std::vector<std::vector<std::string>>>();
        state.currentPlayer = j.at("currentPlayer").get<int>();
        state.winner = j.at("winner").get<int>();
        state.move = j.at("move").get<std::string>();
        state.playerBlob = j.at("playerBlob").get<std::vector<std::pair<int, int>>>();
        state.aiBlob = j.at("aiBlob").get<std::vector<std::pair<int, int>>>();
        state.playerColor = j.at("playerColor").get<std::string>();
        state.aiColor = j.at("aiColor").get<std::string>();
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] JSON parsing error: " << e.what() << "\n";
        throw;  // Rethrow so caller can catch and handle
    }

    return state;
}

nlohmann::json GameState::to_json() const {
    return {
        {"board", board},
        {"currentPlayer", currentPlayer},
        {"playerBlob", playerBlob},
        {"aiBlob", aiBlob},
        {"playerColor", playerColor},
        {"aiColor", aiColor},
        {"move", move},
        {"winner", winner}
    };
}


// --- Core Blob Application Logic ---
// This function contains the common logic for applying a color move for *any* player.
// It will be called by both applyPlayerMove and applyAIMove (via Monte Carlo/Expectimax).
void GameState::applyColorMove(const std::string& newColor, int player_id) {
    // Determine which blob and color to modify based on player_id
    std::vector<std::pair<int, int>>* targetBlob = (player_id == 0) ? &this->playerBlob : &this->aiBlob;
    std::string* targetColor = (player_id == 0) ? &this->playerColor : &this->aiColor;

    // A game can be initialized with empty blobs for both player/AI.
    // If a blob is empty, there's nothing to expand, so we cannot apply a move.
    if (targetBlob->empty()) {
        std::cerr << "Error: Target blob is empty. Cannot apply move for player_id: " << player_id << "\n";
        // Optionally, set winner or mark game as invalid if this is a critical error
        return;
    }

    // Check for invalid move (choosing current color)
    if (newColor == *targetColor) {
        // This case should ideally be filtered out by getPossibleMoves().
        // In a simulation, if this happens, it signifies an invalid path.
        return;
    }

    // Temporarily store old color if needed (not directly used for logic but good for debugging)
    // std::string oldColorOfBlob = *targetColor;

    // Update the player's actual color for the blob
    *targetColor = newColor;

    std::set<std::pair<int, int>> nextBlobCoords;
    std::queue<std::pair<int, int>> q;

    // 1. Add all existing blob cells to the new set and queue, and recolor them on the board
    for (const auto& coord : *targetBlob) {
        // No need to check if in set here if targetBlob always contains unique coords
        nextBlobCoords.insert(coord);
        q.push(coord);
        this->board[coord.first][coord.second] = newColor;
    }

    // Directions for neighbors
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    // 2. Perform a BFS/expansion to find all newly connected cells of `newColor`.
    while (!q.empty()) {
        std::pair<int, int> current = q.front();
        q.pop();

        for (int i = 0; i < 4; ++i) {
            int r = current.first + dr[i];
            int c = current.second + dc[i];

            if (r >= 0 && r < this->board.size() && c >= 0 && c < this->board[0].size()) {
                if (this->board[r][c] == newColor && nextBlobCoords.find({r, c}) == nextBlobCoords.end()) {
                    nextBlobCoords.insert({r, c});
                    q.push({r, c});
                    // board[r][c] is already newColor, so no need to change here again.
                }
            }
        }
    }

    // 3. Update the actual blob with the collected unique coordinates.
    targetBlob->clear();
    for (const auto& coord : nextBlobCoords) {
        targetBlob->emplace_back(coord.first, coord.second);
        // Ensure all cells that were *added* to the blob are explicitly set to the new color.
        // This is mostly done in BFS, but acts as a final safeguard.
        this->board[coord.first][coord.second] = newColor;
    }

    // 4. Set the move for this state (which color was chosen)
    this->move = newColor;

    // 5. Switch current player
    this->currentPlayer = 1 - this->currentPlayer;
}

// --- Specific Player Move Application ---
// This function will now simply call the generalized applyColorMove.
void GameState::applyPlayerMove() {
    std::cout << "Applying player move with color: " << this->move << " (Filler rules)...\n";

    // Player ID is 0 for the human player
    this->applyColorMove(this->move, 0);

    std::cout << "Player blob size after move: " << this->playerBlob.size() << "\n";
    std::cout << "Board state after player move:\n";
    for (const auto& row : this->board) {
        for (const auto& cell : row) {
            std::cout << cell << " ";
        }
        std::cout << "\n";
    }
}


// --- Deep Copy Implementation ---
GameState GameState::copy() const {
    GameState newState;
    newState.board = this->board;
    newState.currentPlayer = this->currentPlayer;
    newState.winner = this->winner;
    newState.move = this->move;
    newState.playerBlob = this->playerBlob;
    newState.aiBlob = this->aiBlob;
    newState.playerColor = this->playerColor;
    newState.aiColor = this->aiColor;
    return newState;
}

// --- Helper to get all possible color choices for the current player ---
std::vector<std::string> GameState::getPossibleMoves() const {
    std::set<std::string> possibleColors;
    const std::vector<std::pair<int, int>>& currentBlob = (this->currentPlayer == 0) ? this->playerBlob : this->aiBlob;
    const std::string& currentColor = (this->currentPlayer == 0) ? this->playerColor : this->aiColor;
    const std::string& oppositeColor = (this->currentPlayer == 0) ? this->aiColor : this->playerColor;

    // If the current blob is empty, there are no moves possible from it
    if (currentBlob.empty()) {
        return {}; // Return an empty vector
    }

    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    for (const auto& coord : currentBlob) {
        int r = std::get<0>(coord);
        int c = std::get<1>(coord);

        for (int i = 0; i < 4; ++i) {
            int nr = r + dr[i];
            int nc = c + dc[i];

            if (nr >= 0 && nr < board.size() && nc >= 0 && nc < board[0].size()) {
                std::string neighborColor = board[nr][nc];
                if (neighborColor != currentColor && neighborColor != oppositeColor) { // Cannot choose current blob color
                    possibleColors.insert(neighborColor);
                }
            }
        }
    }
    return std::vector<std::string>(possibleColors.begin(), possibleColors.end());
}


// --- Game Over Check ---
bool GameState::isGameOver() const {
    // Implement your game over logic here.
    // Examples:
    // 1. One blob occupies the entire board (or almost all of it).
    // 2. One player runs out of valid moves.
    // 3. A fixed number of turns is reached (if applicable for your game type).

    // Simple example: If either player's blob is 0, or if all cells are taken
    if (playerBlob.empty() || aiBlob.empty()) return true;

    int totalCells = board.size() * board[0].size();
    if (playerBlob.size() + aiBlob.size() >= totalCells) {
        // All cells are claimed.
        return true;
    }

    // Also check if any player has no valid moves left.
    // If a player has no possible moves, they lose or the game ends.
    GameState playerTemp = this->copy();
    playerTemp.currentPlayer = 0; // Temporarily set to player to check their moves
    if (playerTemp.getPossibleMoves().empty()) {
        // Player has no moves, AI wins or game ends.
        return true;
    }
    GameState aiTemp = this->copy();
    aiTemp.currentPlayer = 1; // Temporarily set to AI to check their moves
    if (aiTemp.getPossibleMoves().empty()) {
        // AI has no moves, player wins or game ends.
        return true;
    }

    return false;
}

int GameState::determineWinner() const {
    int board_rows = board.size();
    int board_cols = board[0].size();
    int total_cells = board_rows * board_cols;

    if (playerBlob.size() + aiBlob.size() == total_cells) {
        if (playerBlob.size() > aiBlob.size()) {
            std::cout << "Player wins by occupying more cells.\n";
            return 0; // Player wins
        } else if (aiBlob.size() > playerBlob.size()) {
            std::cout << "AI wins by occupying more cells.\n";
            return 1; // AI wins
        } else {
            std::cout << "Game ends in a draw (both occupy same number of cells).\n";
            return 2; // Draw
        }
    }

    // Check if player 0 has no moves
    GameState tempPlayerState = this->copy(); // Copy to check moves without altering current state
    tempPlayerState.currentPlayer = 0;
    if (tempPlayerState.getPossibleMoves().empty()) {
        return 1; // AI wins because player 0 is stuck
    }

    // Check if player 1 (AI) has no moves
    GameState tempAIState = this->copy(); // Copy to check moves
    tempAIState.currentPlayer = 1;
    if (tempAIState.getPossibleMoves().empty()) {
        return 0; // Player 0 wins because AI is stuck
    }

    return -1; // No winner yet
}

// The existing checkWinner function, which updates the 'winner' member
void GameState::checkWinner() {
    this->winner = determineWinner(); // Now uses the const helper
}

// In GameState class
double GameState::evaluateState() const {
    // If game is over, return definitive scores
    int current_winner = this->determineWinner();
    if (current_winner == 1) return 1000000.0; // AI wins (very high score)
    if (current_winner == 0) return -1000000.0; // Player wins (very low score)
    if (current_winner == 2) return 0.0; // Draw

    double score = 0.0;

    // 1. Blob Size Difference (Primary factor)
    score += (double)aiBlob.size() * WEIGHT_AI_BLOB_SIZE;
    score += (double)playerBlob.size() * WEIGHT_PLAYER_BLOB_SIZE; // Negative weight for player's blob

    // 2. Number of Available Moves for AI (Encourage flexibility)
    // Temporarily set current player to AI to get AI's possible moves
    GameState tempAIState = this->copy();
    tempAIState.currentPlayer = 1; // AI's turn
    score += tempAIState.getPossibleMoves().size() * WEIGHT_AVAILABLE_COLORS;

    // 3. Proximity to Enemy Tiles (Encourage capturing)
    // We need to find tiles adjacent to the AI's blob that are currently player's color
    int adjacentEnemyTiles = 0;
    std::set<std::pair<int, int>> visitedNeighbors; // To avoid recounting cells
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    for (const auto& coord : aiBlob) {
        int r = coord.first;
        int c = coord.second;

        for (int i = 0; i < 4; ++i) {
            int nr = r + dr[i];
            int nc = c + dc[i];

            if (nr >= 0 && nr < board.size() && nc >= 0 && nc < board[0].size()) {
                if (board[nr][nc] == playerColor && visitedNeighbors.find({nr, nc}) == visitedNeighbors.end()) {
                    adjacentEnemyTiles++;
                    visitedNeighbors.insert({nr, nc});
                }
            }
        }
    }
    score += adjacentEnemyTiles * WEIGHT_ADJACENT_ENEMY_TILES;

    // You can add more heuristics here, e.g.,
    // - Centrality of blob (if applicable for your board)
    // - Preventing player from getting large blobs

    return score;
}


// --- Monte Carlo Simulation Helper ---
// Simulates a game to completion (or max turns) with random moves
void simulateRandomGame(GameState& state_to_simulate, int max_simulation_turns) {
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count()); // Seed RNG

    for (int turns = 0; turns < max_simulation_turns && !state_to_simulate.isGameOver(); ++turns) {
        std::vector<std::string> possibleMoves = state_to_simulate.getPossibleMoves();

        if (possibleMoves.empty()) {
            // No moves available, game ends prematurely for this path
            state_to_simulate.winner = (state_to_simulate.currentPlayer == 0) ? 1 : 0; // Current player has no moves, so opponent wins
            return;
        }

        // Choose a random move
        std::uniform_int_distribution<int> dist(0, possibleMoves.size() - 1);
        std::string chosenMove = possibleMoves[dist(rng)];

        // Apply the random move
        state_to_simulate.applyColorMove(chosenMove, state_to_simulate.currentPlayer);
    }
    // After simulation, check winner (if not already set by no moves)
    state_to_simulate.checkWinner();
}


// --- applyAIMove function using Monte Carlo ---
// In GameState class
void GameState::applyAIMove() {
    std::cout << "AI is thinking (Minimax with Alpha-Beta Pruning)...\n";

    std::vector<std::string> possibleAIMoves = this->getPossibleMoves();
    if (possibleAIMoves.empty()) {
        std::cout << "AI has no possible moves.\n";
        this->winner = 0; // AI loses if it has no moves
        this->currentPlayer = 0; // Switch to player's turn (though game is over)
        return;
    }

    // Call minimax from the AI's perspective
    // AI is the maximizing player (true), initial alpha is neg infinity, beta is pos infinity
    std::pair<double, std::string> result = minimax(*this, MAX_SEARCH_DEPTH, true,
                                                    -std::numeric_limits<double>::infinity(),
                                                    std::numeric_limits<double>::infinity());

    std::string bestMove = result.second;

    // Apply the best move found
    if (!bestMove.empty()) {
        this->applyColorMove(bestMove, 1); // AI is player_id 1
        std::cout << "AI chose move: " << bestMove << "\n";
    } else {
        // Fallback: This should ideally not happen if possibleAIMoves is not empty
        // Choose the first possible move as a default
        this->applyColorMove(possibleAIMoves[0], 1);
        std::cout << "AI: Fallback to first possible move: " << possibleAIMoves[0] << "\n";
    }

    std::cout << "Board state after AI move:\n";
    for (const auto& row : this->board) {
        for (const auto& cell : row) {
            std::cout << cell << " ";
        }
        std::cout << "\n";
    }

    // Switch to player's turn is already handled by applyColorMove
    this->currentPlayer = 1 - this->currentPlayer; // Switch back to player
}

// Add to GameState class, typically private or a helper function
// Prototype:
// std::pair<double, std::string> minimax(GameState state, int depth, bool maximizingPlayer, double alpha, double beta);

// In GameState class
std::pair<double, std::string> GameState::minimax(GameState state, int depth, bool maximizingPlayer, double alpha, double beta) {
    // Base case: If depth is 0 or game is over, evaluate the current state
    if (depth == 0 || state.isGameOver()) {
        return {state.evaluateState(), ""}; // Return the score and an empty move
    }

    std::vector<std::string> possibleMoves;
    std::string bestMove = "";

    if (maximizingPlayer) { // AI's turn (maximizing player)
        state.currentPlayer = 1; // Temporarily set to AI for move generation
        possibleMoves = state.getPossibleMoves();
        if (possibleMoves.empty()) {
            return {state.evaluateState(), ""};
        }
        bestMove = possibleMoves[0]; // Initialize with a default move

        double maxEval = -std::numeric_limits<double>::infinity();
        for (const std::string& move : possibleMoves) {
            GameState newState = state.copy();
            newState.applyColorMove(move, 1); // AI is player_id 1
            std::pair<double, std::string> eval = minimax(newState, depth - 1, false, alpha, beta);

            if (eval.first > maxEval) {
                maxEval = eval.first;
                bestMove = move;
            }
            alpha = std::max(alpha, maxEval);
            if (beta <= alpha) {
                break; // Alpha-beta cutoff
            }
        }
        return {maxEval, bestMove};
    } else { // Player's turn (minimizing player)
        state.currentPlayer = 0; // Temporarily set to Player for move generation
        possibleMoves = state.getPossibleMoves();
        if (possibleMoves.empty()) {
            return {state.evaluateState(), ""};
        }
        bestMove = possibleMoves[0]; // Initialize with a default move

        double minEval = std::numeric_limits<double>::infinity();
        for (const std::string& move : possibleMoves) {
            GameState newState = state.copy();
            newState.applyColorMove(move, 0); // Player is player_id 0
            std::pair<double, std::string> eval = minimax(newState, depth - 1, true, alpha, beta);

            if (eval.first < minEval) {
                minEval = eval.first;
                bestMove = move;
            }
            beta = std::min(beta, minEval);
            if (beta <= alpha) {
                break; // Alpha-beta cutoff
            }
        }
        return {minEval, bestMove};
    }
}