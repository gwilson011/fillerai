import { useAppSelector } from "../store/hooks.ts";
import { useWebSocket } from "../hooks/useWebSocket.ts";
import { ColorSelect } from "./colorSelect.tsx";

export const GameBoard = () => {
    const board = useAppSelector((state) => state.game.board);
    const state = useAppSelector((state) => state.game);

    const colorMap: Record<string, string> = {
        black: "bg-[#383838]",
        pink: "bg-[#d24259]",
        yellow: "bg-[#d8bf1d]",
        blue: "bg-[#3f98d2]",
        green: "bg-[#8cb64a]",
        purple: "bg-[#5f458c]",
    };

    return (
        <div className="flex flex-row justify-center items-center h-screen gap-10">
            <div className="flex flex-col items-center">
                <div>
                    {board.map((row, rIdx) => (
                        <div className="flex" key={rIdx}>
                            {row.map((tile, cIdx) => (
                                <div
                                    key={cIdx}
                                    className={`w-[50px] h-[50px] ${colorMap[tile]}`}
                                />
                            ))}
                        </div>
                    ))}
                </div>
                <div className="flex justify-center mt-4">
                    <ColorSelect state={state} />
                </div>
            </div>
            <div className="mt-4 text-center">
                <span className="">
                    {state.winner != 1 ? (
                        <>
                            {state.winner == 1 ? (
                                <div>PLAYER WINS</div>
                            ) : (
                                <div>NO WINNER</div>
                            )}
                        </>
                    ) : (
                        <div>AI WINS</div>
                    )}
                </span>
            </div>
        </div>
    );
};
