const express = require("express");
const bodyParser = require("body-parser");
const { exec } = require("child_process");
const fs = require("fs");
const path = require("path");

const app = express();
const port = 3000;

app.use(bodyParser.json());
app.use(express.static(path.join(__dirname, "public")));

app.post("/run", (req, res) => {
  const code = req.body.code;
  const tempFile = path.resolve(__dirname, "../temp.rox");

  fs.writeFile(tempFile, code, (err) => {
    if (err) {
      console.error(err);
      return res.status(500).json({ output: "Error writing temp file." });
    }

    // Run the compiler from the parent directory
    // Command: ../rox run ../temp.rox
    const roxPath = path.resolve(__dirname, "../rox");
    const cmd = `${roxPath} run ${tempFile}`;

    exec(
      cmd,
      { cwd: path.resolve(__dirname, "..") },
      (error, stdout, stderr) => {
        // Delete temp file
        fs.unlink(tempFile, () => {});

        // Separate build logs from program output
        const lines = stdout.split("\n");
        const logs = [];
        const outputLines = [];

        for (const line of lines) {
          if (line.startsWith("Generated ") || line.startsWith("Compiled ")) {
            logs.push(line);
          } else {
            outputLines.push(line);
          }
        }

        if (error) {
          // Return stderr (runtime error) + stdout (partial output)
          // If compilation failed, stdout has the logs (already extracted) and maybe partial output
          return res.json({
            output: stderr || outputLines.join("\n").trim() || error.message,
            logs: logs.join("\n"),
          });
        }

        res.json({
          output: outputLines.join("\n").trim(),
          logs: logs.join("\n"),
        });
      },
    );
  });
});

app.post("/format", (req, res) => {
  const code = req.body.code;
  const tempFile = path.resolve(__dirname, "../temp_format.rox");

  fs.writeFile(tempFile, code, (err) => {
    if (err) {
      console.error(err);
      return res.status(500).json({ error: "Error writing temp file." });
    }

    const roxPath = path.resolve(__dirname, "../rox");
    // Command: ../rox format ../temp_format.rox
    const cmd = `${roxPath} format ${tempFile}`;

    exec(
      cmd,
      { cwd: path.resolve(__dirname, "..") },
      (error, stdout, stderr) => {
        // Delete temp file
        fs.unlink(tempFile, () => {});

        if (error) {
          return res.status(500).json({ error: stderr || error.message });
        }

        res.json({ formatted: stdout });
      },
    );
  });
});

app.listen(port, () => {
  console.log(`ROX Playground running at http://localhost:${port}`);
});
