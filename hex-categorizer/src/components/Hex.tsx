import React from "react";
import { HexCls } from "./HexCreator";

export function Hex(props: { hex: HexCls }) {
    const [hovered, setHover] = React.useState<boolean>(false);

    return (
        <div
            onMouseEnter={() => setHover(true)}
            onMouseLeave={() => setHover(false)}
            onClick={() => props.hex.onClicked()}
            style={{
                border: "1px solid black",
                width: "100px",
                height: "100px",
                margin: "10px",
                textAlign: "center",
                display: "flex",
                flexDirection: "column",
                justifyContent: "center",
                cursor: "pointer",
                backgroundColor: hovered ? "grey" : "yellow",
            }}
        >
            {props.hex.id}
        </div>
    );
}
