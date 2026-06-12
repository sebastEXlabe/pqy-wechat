import { buildApp } from "./app.js";
import { config } from "./config/index.js";

async function main() {
  const app = await buildApp();

  try {
    await app.listen({
      host: config.server.host,
      port: config.server.port,
    });

    console.log(`
╔═══════════════════════════════════════════════════════════╗
║                                                           ║
║   PQY 微信自动化中间层 - API 网关                          ║
║                                                           ║
║   Server: http://${config.server.host}:${config.server.port}                  ║
║   MCP:    stdio mode (for Claude Desktop)                 ║
║                                                           ║
╚═══════════════════════════════════════════════════════════╝
    `);
  } catch (err) {
    app.log.error(err);
    process.exit(1);
  }
}

main();
