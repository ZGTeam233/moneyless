// 占位文件：构建时由 vite-plugin-pwa (Workbox) 自动生成真正的 sw.js 并覆盖此文件。
// 保留源文件仅为兼容已按旧地址注册过本 SW 的客户端，使其能被新版本接管。
self.addEventListener('install', () => self.skipWaiting());
self.addEventListener('activate', () => self.clients.claim());
