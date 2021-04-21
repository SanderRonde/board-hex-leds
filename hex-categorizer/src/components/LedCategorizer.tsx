import { HexCls } from "./HexCreator";
import * as React from "react";

export function LedCategorizer(props: { hexes: HexCls[] }) {
    const highestLed = Math.max(
        ...props.hexes.map((hex) => Math.max(...hex.leds))
    );
    const [ledIdx, setLedIdx] = React.useState<number>(
        Number.isFinite(highestLed) ? highestLed + 1 : 0
    );

    const turnOffLed = async (ledNumber: number) => {
        await fetch(
            `http://192.168.1.85:80/set_led?num=${ledNumber}&power=off`,
            {
                mode: "no-cors",
                method: "POST",
            }
        );
    };

    const turnOnLed = async (ledNumber: number) => {
        await fetch(
            `http://192.168.1.85:80/set_led?num=${ledNumber}&color=%23FF00FF`,
            {
                mode: "no-cors",
                method: "POST",
            }
        );
    };

    props.hexes.forEach(
        (hex) =>
            (hex.onClicked = async () => {
                hex.leds.push(ledIdx);
                hex.save();
                setLedIdx(ledIdx + 1);
                await turnOffLed(ledIdx);
                await turnOnLed(ledIdx + 1);
            })
    );

    return (
        <div>
            <h1>LED categorizer</h1>
            Currently at LED <b>{ledIdx}</b>
            <br />
            <button onClick={() => turnOnLed(ledIdx)}>Turn on</button>
            <button onClick={() => turnOffLed(ledIdx)}>Turn off</button>
        </div>
    );
}
