// src/hooks/useWebSocket.ts
import { useEffect, useRef } from "react";
import { useAppDispatch } from "../store/hooks.ts";
import {
    setBoard,
    setCurrentPlayer,
    setWinner,
    setSocketConnected,
} from "../store/gameSplice.ts";

export const useWebSocket = () => {
    const dispatch = useAppDispatch();
    const ws = useRef<WebSocket | null>(null);

    useEffect(() => {
        ws.current = new WebSocket("ws://localhost:9001");

        ws.current.onopen = () => dispatch(setSocketConnected(true));
        ws.current.onmessage = (event) => {
            const data = JSON.parse(event.data);
            dispatch(setBoard(data.board));
            dispatch(setCurrentPlayer(data.currentPlayer));
            dispatch(setWinner(data.winner));
        };

        return () => ws.current?.close();
    }, []);

    const sendMove = (moveData: object) => {
        console.log("Sending move data:", moveData);
        ws.current?.send(JSON.stringify(moveData));
    };

    return { sendMove };
};
