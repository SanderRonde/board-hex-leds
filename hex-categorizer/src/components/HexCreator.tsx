import React from "react";
import { Hex } from "./Hex";
import { LedCategorizer } from "./LedCategorizer";

export interface HexType {
    id: number;
    borders: [number, number, number, number, number, number];
    leds: number[];
}

const DEFAULT_HEXES: HexType[] = [
    {
        id: 1,
        borders: [-1, 3, 4, 2, -1, -1],
        leds: [],
    },
    {
        id: 2,
        borders: [1, 4, -1, -1, -1, -1],
        leds: [],
    },
    {
        id: 3,
        borders: [-1, -1, 5, 4, 2, -1],
        leds: [],
    },
    {
        id: 4,
        borders: [3, 5, 6, -1, 2, 1],
        leds: [],
    },
    {
        id: 5,
        borders: [-1, 7, 8, 6, 4, 3],
        leds: [],
    },
    {
        id: 6,
        borders: [5, 8, -1, -1, -1, 4],
        leds: [],
    },
    {
        id: 7,
        borders: [-1, 9, 10, 8, 5, -1],
        leds: [],
    },
    {
        id: 8,
        borders: [7, 10, -1, -1, 6, 5],
        leds: [],
    },
    {
        id: 9,
        borders: [-1, -1, -1, 10, 7, -1],
        leds: [],
    },
    {
        id: 10,
        borders: [9, -1, -1, -1, 8, 7],
        leds: [],
    },
];

export class HexCls {
    static classesByIds: Map<number, HexCls> = new Map();

    static store() {
        const hexes = Array.from(this.classesByIds.values()).map((h) =>
            h.toJSON()
        );
        localStorage.setItem("hexes", JSON.stringify(hexes));
    }

    constructor(private _hexType: HexType) {
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

	save() {
		HexCls.store();
	}
}

export function HexCreator() {
    const [hexes, _] = React.useState<HexCls[]>(
        (localStorage.getItem("hexes")
            ? (JSON.parse(localStorage.getItem("hexes")!) as HexType[])
            : DEFAULT_HEXES
        ).map((h) => new HexCls(h))
    );

    const rows: HexCls[][] = [[]];
    for (const hex of hexes) {
        if (hex.top === null) {
            rows[0].push(hex);
        }
    }

    let lastIndex: number = 0;
    do {
        rows.push([]);
        for (const lastRowItem of rows[lastIndex]) {
            if (lastRowItem.bot) {
                rows[lastIndex + 1].push(lastRowItem.bot);
            }
        }
        lastIndex++;
    } while (rows[lastIndex].length);

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
                                    <Hex hex={item} key={item.id} />
                                ))}
                            </div>
                        );
                    })}
                </div>
            </div>

            <LedCategorizer hexes={hexes} />
        </span>
    );
}
