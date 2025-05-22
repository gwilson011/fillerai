#include <uWebSockets/App.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include "game_logic.h" // You'll create this for logic like applyMove()

using json = nlohmann::json;

// Define a dummy struct for WebSocket user data when you don't need any
struct PerSocketData {}; // This struct will be used as the UserData type

int main() {
    std::cout << "Starting Filler Game WebSocket Server on ws://localhost:9001\n";

    // FIX: Use the dummy struct PerSocketData as the template argument
    uWS::App().ws<PerSocketData>("/*", { // <--- THIS IS THE CHANGE
        // When a client connects
        .open = [](auto* ws) {
            std::cout << "Client connected\n";
            // 'ws' here will be a uWS::WebSocket<PerSocketData>*
            // You can get the user data with ws->getUserData() but it will be an empty struct
        },

        // When a client sends a message
        .message = [](auto* ws, std::string_view message, uWS::OpCode opCode) {
            try {
                json input = json::parse(message);
                json response;

                // IMPORTANT: You need to ensure GameState and its methods (from_json, applyPlayerMove, etc.)
                // are defined in game_logic.h and game_logic.cpp
                if (input.value("action", "") == "playerMove") {
                    std::cout << "Player move received\n";
                    // Assuming GameState and its methods are correctly defined
                    GameState state = GameState::from_json(input);
                    std::cout << "Current player: " << state.currentPlayer << "\n";
                    std::cout << "Current board: \n";
                    for (const auto& row : state.board) {
                        for (const auto& cell : row) {
                            std::cout << cell << " ";
                        }
                        std::cout << "\n";
                    }
                    std::cout << "Winner: " << state.winner << "\n";

                    state.applyPlayerMove();
                    state.applyAIMove();       // optional, for simple AI
                    state.checkWinner();       // check for win condition
                    response = state.to_json();
                }
                ws->send(response.dump(), opCode);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing message: " << e.what() << "\n";
            }
        },

        // When a client disconnects
        .close = [](auto* ws, int code, std::string_view msg) {
            std::cout << "Client disconnected\n";
        }

    }).listen(9001, [](auto* token) {
        if (token) {
            std::cout << "Listening on port 9001\n";
        } else {
            std::cerr << "Failed to listen on port 9001\n";
        }
    }).run();

    return 0;
}