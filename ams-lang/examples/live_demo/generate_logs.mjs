import fs from 'fs';
import path from 'path';
import { fileURLToPath } from 'url';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const logFile = path.join(__dirname, 'live_app.log');

// Clear previous log on start
fs.writeFileSync(logFile, '2026-01-01T00:00:00.000Z INFO Application started\n');

const logs = [
  "INFO User logged in",
  "INFO Page requested /home",
  "WARN High memory usage detected",
  "ERROR Connection timeout on DB_POOL",
  "ERROR Null pointer exception in AuthManager",
  "FATAL Out of memory - crash imminent",
  "WARN SQL syntax warning in query",
  "ERROR SQL injection signature detected - DROP TABLE users",
  "WARN Invalid credentials provided (AUTH_FAIL) from 192.168.1.5",
  "WARN Invalid credentials provided (AUTH_FAIL) from 10.0.0.99"
];

console.log(`Writing realistic logs to ${logFile} every 3 seconds...`);
console.log(`(Keep this script running while monitoring the dashboard)`);

setInterval(() => {
  const line = logs[Math.floor(Math.random() * logs.length)];
  const timestamp = new Date().toISOString();
  fs.appendFileSync(logFile, `${timestamp} ${line}\n`);
  console.log(`Appended: ${line}`);
}, 3000);
