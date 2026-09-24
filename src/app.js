// 初始化默认时间为当前时间
document.getElementById('when').value = new Date().toISOString().slice(0, 16);

const STORAGE_KEY = 'favor_records_v1';
let records = JSON.parse(localStorage.getItem(STORAGE_KEY)) || [];

// 保存数据到 localStorage
function saveRecords() {
    localStorage.setItem(STORAGE_KEY, JSON.stringify(records));
    render();
}

// 添加记录
document.getElementById('record-form').addEventListener('submit', (e) => {
    e.preventDefault();

    const type = document.querySelector('input[name="type"]:checked').value;
    const who = document.getElementById('who').value.trim();
    const when = document.getElementById('when').value;
    const why = document.getElementById('why').value.trim();
    const amount = parseFloat(document.getElementById('amount').value);

    if (!who || !when || !why || isNaN(amount)) return;

    const newRecord = {
        id: Date.now().toString(),
        type,
        who,
        when,
        why,
        amount
    };

    records.unshift(newRecord); // 添加到开头
    saveRecords();

    // 重置表单但保留时间
    e.target.reset();
    document.getElementById('when').value = new Date().toISOString().slice(0, 16);
    document.querySelector('input[name="type"][value="in"]').checked = true;
});

// 删除记录
function deleteRecord(id) {
    if (confirm('确定要删除这条记录吗？')) {
        records = records.filter(r => r.id !== id);
        saveRecords();
    }
}

// 渲染界面
function render() {
    const listEl = document.getElementById('record-list');
    const emptyTip = document.getElementById('empty-tip');

    let totalIn = 0;
    let totalOut = 0;

    listEl.innerHTML = '';

    if (records.length === 0) {
        emptyTip.style.display = 'block';
    } else {
        emptyTip.style.display = 'none';
        records.forEach(record => {
            if (record.type === 'in') totalIn += record.amount;
            else totalOut += record.amount;

            const dateStr = new Date(record.when).toLocaleString('zh-CN', {
                month: 'short', day: 'numeric', hour: '2-digit', minute: '2-digit'
            });

            const li = document.createElement('li');
            li.className = 'record-item';
            li.innerHTML = `
                <div class="record-info">
                    <div class="record-who">${record.who}</div>
                    <div class="record-why">${record.why}</div>
                    <div class="record-time">${dateStr}</div>
                </div>
                <div class="record-amount ${record.type === 'in' ? 'amount-in' : 'amount-out'}">
                    ${record.type === 'in' ? '+' : '-'}¥${record.amount.toFixed(2)}
                </div>
                <button class="btn-delete" onclick="deleteRecord('${record.id}')">🗑️</button>
            `;
            listEl.appendChild(li);
        });
    }

    // 更新统计
    document.getElementById('stat-in').textContent = `¥${totalIn.toFixed(2)}`;
    document.getElementById('stat-out').textContent = `¥${totalOut.toFixed(2)}`;
    const net = totalIn - totalOut;
    const netEl = document.getElementById('stat-net');
    netEl.textContent = `${net >= 0 ? '+' : ''}¥${net.toFixed(2)}`;
    netEl.style.color = net >= 0 ? 'var(--success)' : 'var(--danger)';
}

// 导出数据功能 (防止清理缓存丢失数据)
document.getElementById('export-btn').addEventListener('click', () => {
    if (records.length === 0) {
        alert('暂无数据可导出');
        return;
    }
    const dataStr = "data:text/json;charset=utf-8," + encodeURIComponent(JSON.stringify(records, null, 2));
    const downloadAnchorNode = document.createElement('a');
    downloadAnchorNode.setAttribute("href", dataStr);
    downloadAnchorNode.setAttribute("download", "人情记账备份_" + new Date().toISOString().slice(0,10) + ".json");
    document.body.appendChild(downloadAnchorNode);
    downloadAnchorNode.click();
    downloadAnchorNode.remove();
});

// 初始渲染
render();