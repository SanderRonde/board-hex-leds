import { HexGetter } from "./HexCreator";
import * as React from "react";
import { HEX_IP } from "../config";

export function LedCategorizer(props: {
    hexGetters: HexGetter[];
    isOrderer: boolean;
}) {
    const highestLed = Math.max(
        ...props.hexGetters.map((hexGeter) => Math.max(...hexGeter.hex.leds))
    );
    const [ledIdx, setLedIdx] = React.useState<number>(
        props.isOrderer ? 0 : Number.isFinite(highestLed) ? highestLed + 1 : 0
    );

    const [currentHexGetter, setCurrentHexGetter] =
        React.useState<HexGetter | null>(null);
    const [currentList, setCurrentList] = React.useState<number[]>([]);

    const turnOffLed = async (ledNumber: number) => {
        await fetch(
            `http://${HEX_IP}:80/set_led?index=${ledNumber}&color=%23000000`,
            {
                mode: "no-cors",
                method: "POST",
            }
        );
    };

    const turnOnLed = async (ledNumber: number) => {
        await fetch(
            `http://${HEX_IP}:80/set_led?index=${ledNumber}&color=%23FF00FF`,
            {
                mode: "no-cors",
                method: "POST",
            }
        );
    };

    props.hexGetters.forEach(
        (hexGetter) =>
            (hexGetter.hex.onClicked = async () => {
                hexGetter.hex.leds.push(ledIdx);
                hexGetter.hex.save();
                setLedIdx(ledIdx + 1);
                await turnOffLed(ledIdx);
                await turnOnLed(ledIdx + 1);
            })
    );

    props.hexGetters.forEach(
        (hexGetter) =>
            (hexGetter.hex.onOrdering = async () => {
                setCurrentHexGetter(hexGetter);

                setLedIdx(hexGetter.hex.leds[0]);
                setCurrentList([]);
                await turnOnLed(hexGetter.hex.leds[0]);
            })
    );

    const nextLed = async () => {
        if (!currentHexGetter) {
            alert("No hex selected, click one above");
            return;
        }
        const totalSet = currentHexGetter.hex.leds;
        const availableLeds = totalSet.filter((i) => !currentList.includes(i));
        const currentIdx = availableLeds.indexOf(ledIdx);
        let newIdx = (currentIdx + 1) % availableLeds.length;
        if (availableLeds.length === 1) {
            newIdx = 0;
        }
        setLedIdx(availableLeds[newIdx]);

        await turnOffLed(ledIdx);
        await turnOnLed(availableLeds[newIdx]);
    };

    const prevLed = async () => {
        if (!currentHexGetter) {
            alert("No hex selected, click one above");
            return;
        }
        const totalSet = currentHexGetter.hex.leds;
        const availableLeds = totalSet.filter((i) => !currentList.includes(i));
        const currentIdx = availableLeds.indexOf(ledIdx);
        let newIdx =
            (currentIdx + availableLeds.length - 1) % availableLeds.length;

        if (availableLeds.length === 1) {
            newIdx = 0;
        }
        setLedIdx(availableLeds[newIdx]);

        await turnOffLed(ledIdx);
        await turnOnLed(availableLeds[newIdx]);
    };

    const pushLed = () => {
        prevLed();
        currentList.push(ledIdx);

        if (currentList.length === currentHexGetter?.hex.leds.length) {
            currentHexGetter.hex.leds = currentList;
            currentHexGetter.hex.save();

            alert("Done!");

            setCurrentList([]);
        }
    };

    window.onkeydown = (e: KeyboardEvent) => {
        if (e.key === "ArrowLeft") {
            prevLed();
        } else if (e.key === "ArrowRight") {
            nextLed();
        } else if (e.key === " ") {
            pushLed();
        }
    };

    return (
        <div>
            <h1>LED categorizer</h1>
            Currently at LED <b>{ledIdx}</b>
            <br />
            <button onClick={() => turnOnLed(ledIdx)}>Turn on</button>
            <button onClick={() => turnOffLed(ledIdx)}>Turn off</button>
            {props.isOrderer && (
                <span>
                    <br />
                    <br />
                    <button onClick={() => prevLed()}>Prev LED</button>
                    <button onClick={() => nextLed()}>Next LED</button>
                    <br />
                    <br />
                    Current list: {currentList.map((i) => i).join(",")}
                    <br />
                    <button onClick={() => pushLed()}>Push to list</button>
                </span>
            )}
        </div>
    );
}
