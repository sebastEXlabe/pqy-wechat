#!/bin/bash

echo "========================================"
echo "PQY 微信自动化中间层 - 环境安装"
echo "========================================"
echo ""

echo "[1/4] 安装根目录依赖..."
npm install
echo ""

echo "[2/4] 安装前端依赖..."
cd frontend && npm install && cd ..
echo ""

echo "[3/4] 安装后端依赖..."
cd server && npm install && cd ..
echo ""

echo "[4/4] 安装引擎依赖..."
cd engine && pip install -r requirements.txt && cd ..
echo ""

echo "========================================"
echo "安装完成！"
echo "========================================"
echo ""
echo "启动命令："
echo "  npm run dev          # 启动全部服务"
echo "  npm run dev:frontend # 仅启动前端"
echo "  npm run dev:server   # 仅启动后端"
echo "  npm run dev:engine   # 仅启动引擎"
echo ""
echo "MCP Server (for Claude Desktop):"
echo "  npm run start:mcp"
echo ""
