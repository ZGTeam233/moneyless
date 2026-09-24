# 人情记账本 (moneyless)

纯本地运行的请客/人情记账 PWA，数据保存在浏览器 localStorage，支持导出 JSON 备份、离线使用。

## 开发

```bash
npm install
npm run dev      # http://localhost:5173/moneyless/
npm run build    # 产物在 dist/，同时生成离线 Service Worker
npm run preview  # 本地预览构建产物
npm run deploy   # 构建并发布到 GitHub Pages
```

## 说明

- 入口：`index.html` → `src/main.js`（样式在 `src/style.css`）
- 存储：localStorage（key: `favor_records_v1`），与历史版本数据兼容
- PWA：`public/manifest.json` + 构建时由 `vite-plugin-pwa` 生成的 `sw.js`（缓存清单随内容哈希自动更新）
