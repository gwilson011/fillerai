import { createSlice, PayloadAction } from "@reduxjs/toolkit";

interface GameState {
    board: number[][];
    currentPlayer: number;
    winner: number | null;
    aiThinking: boolean;
    socketConnected: boolean;
}

const initialState: GameState = {
    board: Array(10).fill(Array(10).fill(0)),
    currentPlayer: 1,
    winner: null,
    aiThinking: false,
    socketConnected: false,
};

const gameSlice = createSlice({
    name: "game",
    initialState,
    reducers: {
        setBoard(state, action: PayloadAction<number[][]>) {
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
