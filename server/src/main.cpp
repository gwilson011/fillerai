#include <uWebSockets/App.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <thread>     // For std::this_thread::sleep_for
#include <chrono>     // For std::chrono::milliseconds
#include "game_logic.h"

using json = nlohmann::json;

// Define a dummy struct for WebSocket user data when you don't need any
struct PerSocketData {};

int main() {
    std::cout << "Starting Filler Game WebSocket Server on ws://localhost:9001\n";

    uWS::App().ws<PerSocketData>("/*", {
        .open = [](auto* ws) {
            std::cout << "Client connected\n";
        },

        .message = [](uWS::WebSocket<false, true, PerSocketData>* ws, std::string_view message, uWS::OpCode opCode) {
            try {
                json input = json::parse(message);
                json response;

                if (input.value("action", "") == "playerMove") {
                    std::cout << "--- Player move received ---\n";
                    
                    GameState state = GameState::from_json(input);

                    // --- 1. Apply Player's Move ---
                    state.applyPlayerMove();
                    std::cout << "Player move applied.\n";

                    // --- 2. Check for game over after player's move ---
                    state.checkWinner();
                    bool gameOverAfterPlayerMove = state.isGameOver();

                    // --- 3. Send First Response (Player's Move) ---
                    response = state.to_json();
                    if (!ws->send(response.dump(), opCode)) { // Always check send() return
                        std::cerr << "Failed to send player move response (first send). Client likely disconnected.\n";
                        return; // Exit early if send fails
                    }
                    std::cout << "Response 1 (Player's move) sent.\n";

                    // // If game is over, no AI move needed.
                    // if (gameOverAfterPlayerMove) {
                    //     std::cout << "Game over after player's move. No AI move needed.\n";
                    //     return; // Exit here.
                    // }

                    // --- Schedule AI Move Asynchronously ---
                    GameState state_for_ai = state.copy(); 
                    uWS::WebSocket<false, true, PerSocketData>* captured_ws = ws; 

                    uWS::Loop::get()->defer([state_for_ai, captured_ws, opCode]() mutable {
                        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Visual pause

                        std::cout << "AI is calculating and making its move...\n";
                        
                        state_for_ai.applyAIMove(); 
                        std::cout << "AI move applied.\n";
                        
                        state_for_ai.checkWinner(); 
                        if (state_for_ai.isGameOver()) {
                            std::cout << "Game over after AI's move.\n";
                        }

                        // --- 6. Send Second Response (AI's Move) ---
                        // The most reliable check is to simply attempt the send and check its return value.
                        json ai_response = state_for_ai.to_json();
                        if (!captured_ws->send(ai_response.dump(), opCode)) {
                            std::cerr << "Failed to send AI move response (second send). Client likely disconnected.\n";
                        } else {
                            std::cout << "Response 2 (AI's move) sent.\n";
                        }
                    });

                } else {
                    std::cerr << "Unknown action received: " << input.value("action", "N/A") << "\n";
                    response = {{"error", "Unknown action."}};
                    ws->send(response.dump(), opCode);
                }
                
            } catch (const std::exception& e) {
                std::cerr << "Error processing message: " << e.what() << "\n";
                json error_response = {{"error", "Server processing error: " + std::string(e.what())}};
                ws->send(error_response.dump(), opCode);
            }
        },

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