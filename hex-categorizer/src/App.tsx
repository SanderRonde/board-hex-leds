import React from "react";
import "./App.css";
import { HexCreator, HexType } from "./components/HexCreator";
import { DEFAULT_HEXES } from "./config";

const C_TEMPLATE = (arrayContent: string[]) => {
    return `
const hex_describer_t described_hexes_NAME[] = {
${arrayContent.map((c) => `\t${c}`).join(",\n")}  
};

size_t num_hexes_NAME = sizeof(described_hexes_NAME) / sizeof(hex_describer_t);
`;
};

const C_LINE_TEMPLATE = (hexType: HexType) => {
    return `{.id = ${
        hexType.id
    }, .offset_percentage = 0, .invert_order = true, .borders = {${hexType.borders.join(
        ", "
    )}}, .leds = {${[...hexType.leds, -1].join(", ")}}}`;
};

function App() {
    const prettyify = (data: string) => {
        const parsed = JSON.parse(data) as HexType[];
        return `[\n${parsed
            .map((hexType) => JSON.stringify(hexType))
            .join(",\n")}\n]`;
    };

    const [isOrderer, setIsOrderer] = React.useState<boolean>(false);
    const [hexJSON, setHexJSON] = React.useState<string>(
        prettyify(
            localStorage.getItem("hexes") || JSON.stringify(DEFAULT_HEXES)
        )
    );

    const updateHexesStoredData = (data: string) => {
        setHexJSON(prettyify(data));
        localStorage.setItem("hexes", data);
    };

    const cFormat = React.useMemo(() => {
        const parsedHexes = JSON.parse(hexJSON) as HexType[];
        return C_TEMPLATE(parsedHexes.map((line) => C_LINE_TEMPLATE(line)));
    }, [hexJSON]);

    return (
        <div className="App">
            <h1>Hex Categorizer</h1>
            <HexCreator
                updateStoredData={updateHexesStoredData}
                isOrderer={isOrderer}
            />

            <br />
            <label htmlFor="cb">Enable ordering mode</label>
            <input
                id="cb"
                type="checkbox"
                onChange={(e) => {
                    setIsOrderer(e.target.checked);
                }}
            />

            <br />
            <br />
            <br />
            <div
                style={{
                    border: "1px solid red",
                    height: "200px!important",
                    overflow: "hidden",
                    display: "flex",
                    flexDirection: "row",
                    padding: 20,
                }}
            >
                <div style={{ flexGrow: 1 }}>
                    <p>JSON</p>
                    <textarea
                        style={{ width: "100%" }}
                        rows={30}
                        cols={100}
                        readOnly
                        value={hexJSON}
                    />
                </div>
                <div style={{ flexGrow: 1 }}>
                    <p>C format</p>
                    <textarea
                        style={{ width: "100%" }}
                        rows={30}
                        cols={100}
                        readOnly
                        value={cFormat}
                    />
                </div>
            </div>
        </div>
    );
}

export default App;
