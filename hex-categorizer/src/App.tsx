import React from "react";
import "./App.css";
import { DEFAULT_HEXES, HexCreator, HexType } from "./components/HexCreator";
import { UnControlled as CodeMirror } from "react-codemirror2";
import "codemirror/lib/codemirror.css";
import "codemirror/theme/material.css";
import "codemirror/mode/javascript/javascript.js";

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
                    height: '200px!important',
                    overflow: 'hidden'
                }}
            >
                <CodeMirror
                    options={{
                        readOnly: true,
                        mode: "javascript",
                        lineWrapping: true,
                    }}
                    value={hexJSON}
                />
            </div>
        </div>
    );
}

export default App;
