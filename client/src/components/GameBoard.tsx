import { useAppSelector } from "../store/hooks.ts";
import { useWebSocket } from "../hooks/useWebSocket.ts";

export const GameBoard = () => {
    const board = useAppSelector((state) => state.game.board);
    const currentPlayer = useAppSelector((state) => state.game.currentPlayer);
    const winner = useAppSelector((state) => state.game.winner);
    const { sendMove } = useWebSocket();

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
                <div key={rIdx}>
                    {row.map((tile, cIdx) => (
                        <button
                            key={cIdx}
                            onClick={() => handleClick(rIdx, cIdx)}
                        >
                            {tile}
                        </button>
                    ))}
                </div>
            ))}
        </div>
    );
};
