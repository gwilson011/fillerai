import { use } from "react";
import { useWebSocket } from "../hooks/useWebSocket.ts";
import { setPlayerBlob } from "../store/gameSplice.ts";
import { useAppSelector } from "../store/hooks.ts";
import { Gamestate } from "../types.ts";

interface ColorProps {
    state: Gamestate;
}

export const ColorSelect: React.FC<ColorProps> = ({ state }) => {
    const colors = [
        { name: "black", hex: "#383838" },
        { name: "pink", hex: "#d24259" },
        { name: "yellow", hex: "#d8bf1d" },
        { name: "blue", hex: "#3f98d2" },
        { name: "green", hex: "#8cb64a" },
        { name: "purple", hex: "#5f458c" },
    ];
    const { board } = useAppSelector((state) => state.game);
    const currentPlayer = useAppSelector((state) => state.game.currentPlayer);
    const winner = useAppSelector((state) => state.game.winner);
    const playerBlob = useAppSelector((state) => state.game.playerBlob);
    const aiBlob = useAppSelector((state) => state.game.aiBlob);
    const playerColor = useAppSelector((state) => state.game.playerColor);
    const aiColor = useAppSelector((state) => state.game.aiColor);
    const { sendMove } = useWebSocket();

    const handleColorClick = (color: string) => {
        console.log(`Selected color: ${color}`);

        sendMove({
            action: "playerMove",
            currentPlayer: currentPlayer,
            playerBlob: playerBlob,
            aiBlob: aiBlob,
            playerColor: playerColor,
            aiColor: aiColor,
            winner: winner == null ? 0 : winner, //TODO: handle winner logic
            move: color,
            board: board,
        });
    };

    return (
        <div className="flex space-x-4">
            {colors.map((color) =>
                color.name === playerColor || color.name === aiColor ? (
                    <div
                        key={color.name}
                        className={`w-10 h-10 cursor-pointer bg-[${color.hex}] opacity-50`}
                    />
                ) : (
                    <div
                        key={color.name}
                        className={`w-10 h-10 cursor-pointer bg-[${color.hex}]`}
                        onClick={() => handleColorClick(color.name)}
                    />
                )
            )}
        </div>
    );
};
