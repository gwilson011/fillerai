export interface Gamestate {
    board: string[][];
    currentPlayer: number;
    winner: number | null;
    playerBlob: [number, number][];
    aiBlob: [number, number][];
    playerColor: string;
    aiColor: string;
}
