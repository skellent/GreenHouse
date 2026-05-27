import { existsSync } from "node:fs";

const required = ["www/index.html", "www/styles.css", "www/app.js", "www/assets/icon.png", "www/assets/icon.svg"];
const missing = required.filter((path) => !existsSync(path));

if (missing.length) {
  console.error(`Missing static files:\n${missing.map((path) => `- ${path}`).join("\n")}`);
  process.exit(1);
}

console.log("Static web app ready in www/");
