import './style.css';

const STORAGE_KEY = 'favor_records_v1';

/* ---------- 数据层（localStorage，纯本地存储） ---------- */

function loadRecords() {
  try {
    const raw = JSON.parse(localStorage.getItem(STORAGE_KEY));
    return Array.isArray(raw) ? raw : [];
  } catch {
    return [];
  }
}

let records = loadRecords();

function saveRecords() {
  localStorage.setItem(STORAGE_KEY, JSON.stringify(records));
}

/* ---------- DOM 引用 ---------- */

const form = document.getElementById('record-form');
const whoInput = document.getElementById('who');
const whenInput = document.getElementById('when');
const whyInput = document.getElementById('why');
const amountInput = document.getElementById('amount');
const listEl = document.getElementById('record-list');
const emptyTip = document.getElementById('empty-tip');
const statIn = document.getElementById('stat-in');
const statOut = document.getElementById('stat-out');
const statNet = document.getElementById('stat-net');
const exportBtn = document.getElementById('export-btn');

/* ---------- 工具函数 ---------- */

function nowLocal() {
  const d = new Date();
  const pad = (n) => String(n).padStart(2, '0');
  return `${d.getFullYear()}-${pad(d.getMonth() + 1)}-${pad(d.getDate())}T${pad(d.getHours())}:${pad(d.getMinutes())}`;
}

function fmtMoney(x) {
  return x.toFixed(2).replace(/\.00$/, '');
}

function formatDate(str) {
  const d = new Date(str);
  if (isNaN(d)) return str;
  return d.toLocaleString('zh-CN', {
    month: 'short', day: 'numeric', hour: '2-digit', minute: '2-digit',
  });
}

/* ---------- 渲染 ---------- */

function buildItem(record) {
  const li = document.createElement('li');
  li.className = 'record-item';

  const info = document.createElement('div');
  info.className = 'record-info';

  const who = document.createElement('div');
  who.className = 'record-who';
  who.textContent = record.who;

  const why = document.createElement('div');
  why.className = 'record-why';
  why.textContent = record.why;

  const time = document.createElement('div');
  time.className = 'record-time';
  time.textContent = formatDate(record.when);

  info.append(who, why, time);

  const amount = document.createElement('div');
  amount.className = `record-amount ${record.type === 'in' ? 'amount-in' : 'amount-out'}`;
  amount.textContent = `${record.type === 'in' ? '+' : '-'}¥${fmtMoney(record.amount)}`;

  const delBtn = document.createElement('button');
  delBtn.className = 'btn-delete';
  delBtn.type = 'button';
  delBtn.textContent = '🗑️';
  delBtn.setAttribute('aria-label', '删除这条记录');
  delBtn.dataset.id = record.id;

  li.append(info, amount, delBtn);
  return li;
}

function render() {
  let totalIn = 0;
  let totalOut = 0;

  listEl.innerHTML = '';

  if (records.length === 0) {
    emptyTip.hidden = false;
  } else {
    emptyTip.hidden = true;
    const frag = document.createDocumentFragment();
    for (const record of records) {
      if (record.type === 'in') totalIn += record.amount;
      else totalOut += record.amount;
      frag.appendChild(buildItem(record));
    }
    listEl.appendChild(frag);
  }

  statIn.textContent = `¥${fmtMoney(totalIn)}`;
  statOut.textContent = `¥${fmtMoney(totalOut)}`;
  const net = totalIn - totalOut;
  statNet.textContent = `${net >= 0 ? '+' : '-'}¥${fmtMoney(Math.abs(net))}`;
  statNet.style.color = net >= 0 ? 'var(--success)' : 'var(--danger)';
}

/* ---------- 事件 ---------- */

// 事件委托：避免每行绑定监听器，也兼容旧数据里含特殊字符的 id
listEl.addEventListener('click', (e) => {
  const btn = e.target.closest('.btn-delete');
  if (!btn) return;
  if (!confirm('确定要删除这条记录吗？')) return;
  records = records.filter((r) => r.id !== btn.dataset.id);
  saveRecords();
  render();
});

form.addEventListener('submit', (e) => {
  e.preventDefault();

  const type = form.elements['type'].value;
  const who = whoInput.value.trim();
  const when = whenInput.value;
  const why = whyInput.value.trim();
  const amount = parseFloat(amountInput.value);

  if (!who || !when || !why || isNaN(amount) || amount < 0) return;

  records.unshift({
    id: Date.now().toString(36) + Math.random().toString(36).slice(2, 8),
    type,
    who,
    when,
    why,
    amount,
  });
  saveRecords();
  render();

  // 重置表单但保留时间；先清空 required 再 reset，避免 Safari 弹出"请填充此字段"提示
  [whoInput, whyInput, amountInput].forEach((el) => (el.required = false));
  form.reset();
  [whoInput, whyInput, amountInput].forEach((el) => (el.required = true));
  whenInput.value = nowLocal();
});

// 防止清理缓存丢失数据：导出 / 导入 JSON 备份
exportBtn.addEventListener('click', () => {
  if (records.length === 0) {
    alert('暂无数据可导出');
    return;
  }
  const blob = new Blob([JSON.stringify(records, null, 2)], { type: 'application/json' });
  const url = URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.href = url;
  a.download = `人情记账备份_${new Date().toISOString().slice(0, 10)}.json`;
  document.body.appendChild(a);
  a.click();
  a.remove();
  URL.revokeObjectURL(url);
});

window.addEventListener('beforeinstallprompt', (e) => {
  e.preventDefault();
});

/* ---------- Service Worker（离线缓存，构建时由 vite-plugin-pwa 生成） ---------- */

if ('serviceWorker' in navigator && import.meta.env.PROD) {
  window.addEventListener('load', () => {
    navigator.serviceWorker
      .register(`${import.meta.env.BASE_URL}sw.js`)
      .catch((err) => console.error('Service Worker 注册失败:', err));
  });
}

/* ---------- 初始渲染 ---------- */

whenInput.value = nowLocal();
render();
