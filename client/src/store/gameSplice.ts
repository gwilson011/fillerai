import { createSlice, PayloadAction } from "@reduxjs/toolkit";

const BOARD_SIZE = 8;
const COLORS = ["black", "yellow", "pink", "blue", "green", "purple"];

interface GameState {
    board: string[][];
    currentPlayer: number;
    winner: number | null;
    aiThinking: boolean;
    socketConnected: boolean;
}

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

const initialState: GameState = {
    board: generateBoard(),
    currentPlayer: 1,
    winner: null,
    aiThinking: false,
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
        setThinking(state, action: PayloadAction<boolean>) {
            state.aiThinking = action.payload;
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
    setThinking,
    setSocketConnected,
} = gameSlice.actions;

export default gameSlice.reducer;
