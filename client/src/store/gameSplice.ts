import { createSlice, PayloadAction } from "@reduxjs/toolkit";
import { GameState } from "../types/gamestate.ts";

const BOARD_SIZE = 8;
const COLORS = ["black", "yellow", "pink", "blue", "green", "purple"];

const generateBoard = () => {
    const board: string[][] = [];
    for (let row = 0; row < BOARD_SIZE; row++) {
        const currentRow: string[] = [];

        for (let col = 0; col < BOARD_SIZE; col++) {
            const disallowed = new Set<string>();
            // Check top
            if (row > 0) {
                disallowed.add(board[row - 1][col]);
            }
            // Check left
            if (col > 0) {
                disallowed.add(currentRow[col - 1]);
            }
            // Choose a random allowed color
            const allowedColors = COLORS.filter(
                (color) => !disallowed.has(color)
            );
            const color =
                allowedColors[Math.floor(Math.random() * allowedColors.length)];
            currentRow.push(color);
        }
        board.push(currentRow);
    }
    return board;
};

const BOARD = generateBoard();

const initialState: GameState = {
    board: BOARD,
    currentPlayer: 1,
    winner: -1, // -1 means no winner yet
    playerBlob: [[7, 0]],
    aiBlob: [[0, 7]],
    playerColor: BOARD[7][0],
    aiColor: BOARD[0][7],
    socketConnected: false,
};

const gameSlice = createSlice({
    name: "game",
    initialState,
    reducers: {
        setBoard(state, action: PayloadAction<string[][]>) {
            state.board = action.payload;
        },
        setCurrentPlayer(state, action: PayloadAction<number>) {
            state.currentPlayer = action.payload;
        },
        setWinner(state, action: PayloadAction<number>) {
            state.winner = action.payload;
        },
        setPlayerBlob(state, action: PayloadAction<[number, number][]>) {
            state.playerBlob = action.payload;
        },
        setAiBlob(state, action: PayloadAction<[number, number][]>) {
            state.aiBlob = action.payload;
        },
        setPlayerColor(state, action: PayloadAction<string>) {
            state.playerColor = action.payload;
        },
        setAiColor(state, action: PayloadAction<string>) {
            state.aiColor = action.payload;
        },
        setSocketConnected(state, action: PayloadAction<boolean>) {
            state.socketConnected = action.payload;
        },
    },
});

export const {
    setBoard,
    setCurrentPlayer,
    setWinner,
    setPlayerBlob,
    setAiBlob,
    setPlayerColor,
    setAiColor,
    setSocketConnected,
} = gameSlice.actions;

export default gameSlice.reducer;
