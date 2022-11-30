import React from "react";
import { DEFAULT_HEXES } from "../config";
import { Hex } from "./Hex";
import { LedCategorizer } from "./LedCategorizer";

export interface HexType {
    id: number;
    borders: [
        top: number,
        topRight: number,
        botRight: number,
        bot: number,
        botLeft: number,
        topLeft: number
    ];
    leds: number[];
}

export class HexGetter {
    constructor(public id: number) {}

    get hex(): HexCls {
        return HexCls.classesByIds.get(this.id)!;
    }
}

export class HexCls {
    static classesByIds: Map<number, HexCls> = new Map();

    static store(updateStoredData: (data: string) => void) {
        const hexes = Array.from(this.classesByIds.values()).map((h) =>
            h.toJSON()
        );
        updateStoredData(JSON.stringify(hexes));
        localStorage.setItem("hexes", JSON.stringify(hexes));
    }

    constructor(
        private _hexType: HexType,
        private _updateStoredData: (data: string) => void
    ) {
        HexCls.classesByIds.set(_hexType.id, this);
    }

    private _getByIndex(index: number): HexCls | null {
        const id = this._hexType.borders[index];
        if (id !== -1) {
            return HexCls.classesByIds.get(id)!;
        }
        return null;
    }

    get leds() {
        return this._hexType.leds;
    }

    set leds(ledArr: number[]) {
        this._hexType.leds = ledArr;
    }

    get id() {
        return this._hexType.id;
    }

    get top() {
        return this._getByIndex(0);
    }

    get topRight() {
        return this._getByIndex(1);
    }

    get botRight() {
        return this._getByIndex(2);
    }

    get bot() {
        return this._getByIndex(3);
    }

    get botLeft() {
        return this._getByIndex(4);
    }

    get topLeft() {
        return this._getByIndex(5);
    }

    toJSON(): HexType {
        return this._hexType;
    }

    onClicked: () => void = () => {};

    onOrdering: () => void = () => {};

    save() {
        HexCls.classesByIds.set(this.id, this);
        HexCls.store(this._updateStoredData);
    }
}

export function HexCreator(props: {
    isOrderer: boolean;
    updateStoredData: (data: string) => void;
}) {
    const [hexes] = React.useState<HexCls[]>(
        (localStorage.getItem("hexes")
            ? (JSON.parse(localStorage.getItem("hexes")!) as HexType[])
            : DEFAULT_HEXES
        ).map((h) => new HexCls(h, props.updateStoredData))
    );

    const rows: HexCls[][] = [];
    for (let i = 0; i < hexes.length; i++) {
        rows[Math.floor(i / 6)] ??= [];
        rows[Math.floor(i / 6)].push(hexes[i]);
    }

    return (
        <span>
            <div
                style={{
                    display: "flex",
                    flexDirection: "row",
                    justifyContent: "center",
                }}
            >
                <div>
                    {rows.map((row, i) => {
                        return (
                            <div
                                key={i}
                                style={{
                                    borderBottom: "1px solid yellow",
                                    display: "flex",
                                    flexDirection: "row",
                                }}
                            >
                                {row.map((item) => (
                                    <Hex
                                        isOrderer={props.isOrderer}
                                        hexGetter={new HexGetter(item.id)}
                                        key={item.id}
                                    />
                                ))}
                            </div>
                        );
                    })}
                </div>
            </div>

            <LedCategorizer
                isOrderer={props.isOrderer}
                hexGetters={hexes.map((hex) => new HexGetter(hex.id))}
            />
        </span>
    );
}
