const net = require("net");
const http = require("http");

const NEURACACHE_HOST = "127.0.0.1";
const NEURACACHE_PORT = 6379;
const API_PORT = 3001;


function encodeCommand(command, args = []) {
    const parts = [command, ...args];

    let result = `*${parts.length}\r\n`;

    for (const part of parts) {
        const value = String(part);

        result += `$${Buffer.byteLength(value)}\r\n`;
        result += `${value}\r\n`;
    }

    return result;
}


function sendCommand(command, args = []) {
    return new Promise((resolve, reject) => {
        const socket = net.createConnection({
            host: NEURACACHE_HOST,
            port: NEURACACHE_PORT
        });

        let response = "";

        socket.on("connect", () => {
            socket.write(
                encodeCommand(command, args)
            );
        });

        socket.on("data", (chunk) => {
            response += chunk.toString();

            // Simple string response such as +PONG
            if (response.startsWith("+")) {
                if (response.includes("\r\n")) {
                    socket.end();
                    resolve(response);
                }
                return;
            }

            // Bulk string response: $<length>\r\n<body>\r\n
            if (response.startsWith("$")) {
                const headerEnd = response.indexOf("\r\n");

                if (headerEnd === -1) {
                    return;
                }

                const length = Number(
                    response.substring(1, headerEnd)
                );

                if (length < 0) {
                    socket.end();
                    resolve("");
                    return;
                }

                const bodyStart = headerEnd + 2;
                const requiredLength =
                    bodyStart + length + 2;

                if (response.length >= requiredLength) {
                    const body = response.substring(
                        bodyStart,
                        bodyStart + length
                    );

                    socket.end();
                    resolve(body);
                }
            }
        });

        socket.on("error", (error) => {
            reject(error);
        });

        socket.setTimeout(3000, () => {
            socket.destroy();
            reject(
                new Error(
                    "NeuraCache request timed out"
                )
            );
        });
    });
}

function parseInfo(response) {

    const info = {};

    const lines = response.split("\r\n");

    for (const line of lines) {

        const separator = line.indexOf(":");

        if (separator === -1) {
            continue;
        }

        const key =
            line.substring(
                0,
                separator
            );

        const value =
            line.substring(
                separator + 1
            );

        info[key] = value;
    }

    return info;
}


function parseAnalyze(response) {
    const result = {
        totalAccesses: 0,
        trackedKeys: 0,
        predictions: []
    };

    const lines = response.split(/\r?\n/);

    let readingPredictions = false;

    for (const line of lines) {

        const trimmed = line.trim();

        if (trimmed.startsWith("total_accesses:")) {

            result.totalAccesses =
                Number(
                    trimmed
                        .split(":")[1]
                        .trim()
                );
        }

        else if (trimmed.startsWith("tracked_keys:")) {

            result.trackedKeys =
                Number(
                    trimmed
                        .split(":")[1]
                        .trim()
                );
        }

        else if (trimmed === "top_keys:") {

            readingPredictions = true;
        }

        else if (
            readingPredictions &&
            trimmed
        ) {

            const match =
                trimmed.match(
                    /^(.+?)\s+score=([0-9.]+)\s+accesses=(\d+)$/
                );

            if (match) {

                result.predictions.push({
                    key: match[1].trim(),
                    score: Number(match[2]),
                    accesses: Number(match[3])
                });
            }
        }
    }

    return result;
}


const server = http.createServer(
    async (request, response) => {

        response.setHeader(
            "Access-Control-Allow-Origin",
            "*"
        );

        response.setHeader(
            "Content-Type",
            "application/json"
        );


        if (request.url === "/api/health") {

            try {

                await sendCommand("PING");

                response.end(
                    JSON.stringify({
                        online: true
                    })
                );

            } catch (error) {

                response.statusCode = 503;

                response.end(
                    JSON.stringify({
                        online: false
                    })
                );
            }

            return;
        }


        if (request.url === "/api/dashboard") {

            try {

                const infoResponse =
                    await sendCommand("INFO");

                const analyzeResponse =
                    await sendCommand("ANALYZE");
                

                const info =
                    parseInfo(infoResponse);

                const analyze =
                    parseAnalyze(analyzeResponse);


                response.end(
                    JSON.stringify({
                        info,
                        analyze
                    })
                );

            } catch (error) {

                response.statusCode = 503;

                response.end(
                    JSON.stringify({
                        error:
                            "Unable to connect to NeuraCache"
                    })
                );
            }

            return;
        }


        response.statusCode = 404;

        response.end(
            JSON.stringify({
                error: "Not found"
            })
        );
    }
);


server.listen(
    API_PORT,
    () => {

        console.log(
            `NeuraCache API bridge running on http://localhost:${API_PORT}`
        );

    }
);