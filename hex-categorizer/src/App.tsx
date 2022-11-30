import React from "react";
import "./App.css";
import { HexCreator, HexType } from "./components/HexCreator";
import { DEFAULT_HEXES } from "./config";

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
                }}
            >
                <textarea rows={20} cols={50} readOnly value={hexJSON} />
            </div>
        </div>
    );
}

export default App;
