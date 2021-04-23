import React from "react";
import { HexGetter } from "./HexCreator";

export function Hex(props: { hexGetter: HexGetter; isOrderer: boolean }) {
    const [hovered, setHover] = React.useState<boolean>(false);

    return (
        <div
            onMouseEnter={() => !props.isOrderer && setHover(true)}
            onMouseLeave={() => !props.isOrderer && setHover(false)}
            onClick={() => {
                if (props.isOrderer) {
                    if (hovered) {
                        setHover(false);
                    } else {
                        setHover(true);
                        props.hexGetter.hex.onOrdering();
                    }
                } else {
                    props.hexGetter.hex.onClicked();
                }
            }}
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
            {props.hexGetter.id}
        </div>
    );
}
