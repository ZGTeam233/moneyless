本项目使用 AI 辅助开发
-

# 人情记账本 (moneyless) 🍲

一个**纯本地运行**的请客 / 人情记账 PWA（渐进式 Web 应用）。

别人请你吃饭、你回请朋友——记下来，谁欠谁多少，一目了然。

- **无需后端、无需账号**：数据全部保存在浏览器 `localStorage`，断网也能用
- **一键备份**：随时导出 JSON 数据文件
- **可安装到主屏幕**：iOS / Android 浏览器均可"添加到主屏幕"，全屏沉浸式体验
- **静态托管**：`npm run deploy` 一条命令发布到 GitHub Pages

## 功能

| 功能 | 说明 |
|---|---|
| 记一笔 | 方向（别人请我 / 我请别人）、对象、时间、原因、金额 |
| 实时统计 | 顶部汇总「别人请我 / 我请别人 / 净差额」，净差额正绿负红 |
| 历史记录 | 按时间倒序展示，支持逐条删除（带确认） |
| 导出备份 | 一键下载全部记录的 JSON 文件 |
| 离线缓存 | Service Worker 预缓存全部资源，二次打开秒加载 |

## 快速开始

### 直接使用（在线版）

访问部署好的 GitHub Pages 地址即可，无需安装任何东西。

### 本地开发

```bash
npm install        # 安装依赖（需要 Node.js 18+）
npm run dev        # 启动开发服务器 http://localhost:5173/moneyless/
```

### 构建与预览

```bash
npm run build      # 产物输出到 dist/，同时生成离线 Service Worker
npm run preview    # 本地预览构建产物
```

## 部署到 GitHub Pages

本项目已内置 `gh-pages` 部署脚本，三步搞定：

**1. 创建 GitHub 仓库并关联远程**

> 仓库名必须为 `moneyless`（或同步修改 `vite.config.js` 中的 `base`，见下文）

```bash
git remote add origin git@github.com:<你的用户名>/moneyless.git
git push -u origin main
```

**2. 一键构建并发布**

```bash
npm run deploy     # 等价于 npm run build && gh-pages -d dist
```

该命令会将 `dist/` 产物推送到仓库的 `gh-pages` 分支。

**3. 开启 GitHub Pages**

进入仓库 **Settings → Pages**，将 Source 选择为 `gh-pages` 分支（根目录），保存后访问：

```
https://<你的用户名>.github.io/moneyless/
```

### 部署注意事项

- **`base` 路径必须与仓库名一致**：`vite.config.js` 中配置了 `base: '/moneyless/'`。
  若改用了其他仓库名，需同步修改此项，否则页面会因资源 404 而白屏。
- **私有仓库**：GitHub Pages 免费版不支持私有仓库发布，请使用 Public 仓库。
- **SW 缓存更新**：站点上线后再次部署，Workbox 会自动接管旧 Service Worker 并更新缓存；
  若遇到页面未更新，可强制刷新（Ctrl/Cmd + Shift + R）一次。

## 项目结构

```
├── index.html          # 页面入口（统计区 + 记一笔表单 + 历史列表）
├── vite.config.js      # Vite 配置：base 路径、PWA 插件
├── src/
│   ├── main.js         # 全部业务逻辑（数据层 / 渲染 / 事件 / SW 注册）
│   └── style.css       # iOS 风格样式（CSS 变量主题色、safe-area 适配）
└── public/
    ├── manifest.json   # PWA 清单（名称、图标、display: standalone）
    └── sw.js           # 占位 SW，构建时被 Workbox 生成的真实 sw.js 覆盖
```

## 技术栈

- [Vite](https://vitejs.dev) — 开发与构建
- [vite-plugin-pwa](https://vite-pwa-org.netlify.app) — PWA / Workbox 自动生成
- 原生 JavaScript (ESM) + 原生 CSS，**无任何 UI 框架**
- [gh-pages](https://www.npmjs.com/package/gh-pages) — 发布到 GitHub Pages

## 数据存储说明

- 存储位置：浏览器 `localStorage`，key 为 `favor_records_v1`
- 数据格式：JSON 数组，每条记录包含 `id / type / who / when / why / amount`
- **注意**：清除浏览器数据会导致记录丢失，请定期使用「导出数据」备份
- 换设备 / 换浏览器时，导出的 JSON 目前仅作为备份存档（暂不支持一键导入恢复）

## License

[MIT](./LICENSE)
