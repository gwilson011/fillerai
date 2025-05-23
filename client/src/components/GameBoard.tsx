import { useAppSelector } from "../store/hooks.ts";
import { useWebSocket } from "../hooks/useWebSocket.ts";

export const GameBoard = () => {
    const board = useAppSelector((state) => state.game.board);
    const currentPlayer = useAppSelector((state) => state.game.currentPlayer);
    const winner = useAppSelector((state) => state.game.winner);
    const { sendMove } = useWebSocket();

    const colorMap: Record<string, string> = {
        black: "bg-[#383838]",
        pink: "bg-[#d24259]",
        yellow: "bg-[#d8bf1d]",
        blue: "bg-[#3f98d2]",
        green: "bg-[#8cb64a]",
        purple: "bg-[#5f458c]",
    };

    const handleClick = (row: number, col: number) => {
        sendMove({
            action: "playerMove",
            currentPlayer: currentPlayer,
            winner: winner == null ? 0 : winner, //TODO: handle winner logic
            move: { row, col },
            board: board,
        });
    };

    return (
        <div>
            {board.map((row, rIdx) => (
                <div className="flex" key={rIdx}>
                    {row.map((tile, cIdx) => (
                        <div
                            key={cIdx}
                            onClick={() => handleClick(rIdx, cIdx)}
                            className={`w-[50px] h-[50px] cursor-pointer ${colorMap[tile]}`}
                        />
                    ))}
                </div>
            ))}
        </div>
    );
};
