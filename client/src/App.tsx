import React from "react";
import { useAppSelector } from "./store/hooks.ts"; // Adjust the path
import { GameBoard } from "./components/GameBoard.tsx"; // Adjust the path

export default function App() {
    const board = useAppSelector((state) => state.game.board);

    return (
        <div>
            <GameBoard />
            <div style={{ color: "black" }}>Board length: {board.length}</div>
        </div>
    );
}
