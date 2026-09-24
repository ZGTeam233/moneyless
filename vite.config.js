import { defineConfig } from 'vite';
import { VitePWA } from 'vite-plugin-pwa';

export default defineConfig({
  base: '/moneyless/',
  plugins: [
    VitePWA({
      registerType: 'autoUpdate',
      includeAssets: ['manifest.json'],
      manifest: false, // 使用 public/manifest.json
      workbox: {
        globPatterns: ['**/*.{js,css,html,json}'],
      },
    }),
  ],
});
