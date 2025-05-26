// src/hooks/useWebSocket.ts
import { useEffect, useRef } from "react";
import { useAppDispatch } from "../store/hooks.ts";
import {
    setBoard,
    setCurrentPlayer,
    setWinner,
    setSocketConnected,
    setPlayerBlob,
    setAiBlob,
    setPlayerColor,
    setAiColor,
} from "../store/gameSplice.ts";

export const useWebSocket = () => {
    const dispatch = useAppDispatch();
    const ws = useRef<WebSocket | null>(null);

    useEffect(() => {
        ws.current = new WebSocket("ws://localhost:9001");

        ws.current.onopen = () => dispatch(setSocketConnected(true));
        ws.current.onmessage = (event) => {
            const data = JSON.parse(event.data);
            console.log(`CLIENT: Message received at ${Date.now()}ms`, data);
            dispatch(setBoard(data.board));
            dispatch(setCurrentPlayer(data.currentPlayer));
            dispatch(setWinner(data.winner));
            dispatch(setPlayerBlob(data.playerBlob));
            dispatch(setAiBlob(data.aiBlob));
            dispatch(setPlayerColor(data.playerColor));
            dispatch(setAiColor(data.aiColor));
            dispatch(setSocketConnected(true));
        };

        return () => ws.current?.close();
    }, []);

    const sendMove = (moveData: object) => {
        console.log("Sending move data:", moveData);
        ws.current?.send(JSON.stringify(moveData));
    };

    return { sendMove };
};
