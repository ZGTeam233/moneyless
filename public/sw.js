const CACHE_NAME = 'moneyless-v1';
const ASSETS_TO_CACHE = [
    '/',
    '/index.html',
    '/src/style.css',
    '/src/app.js',
    '/manifest.json'
];

// 安装阶段：缓存核心文件
self.addEventListener('install', (event) => {
    event.waitUntil(
        caches.open(CACHE_NAME).then((cache) => {
            return cache.addAll(ASSETS_TO_CACHE);
        })
    );
});

// 拦截请求：优先使用缓存，没有再联网 (纯本地方案下，基本全走缓存)
self.addEventListener('fetch', (event) => {
    event.respondWith(
        caches.match(event.request).then((response) => {
            return response || fetch(event.request);
        })
    );
});