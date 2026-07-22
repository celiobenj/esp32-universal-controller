#pragma once
#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP32 Universal Controller</title>
<style>
:root {
  --ash-grey: #b4b8abff;
  --deep-space-blue: #153243ff;
  --yale-blue: #284b63ff;
  --ivory: #f4f9e9ff;
  --soft-linen: #eef0ebff;

  --bg-body: var(--soft-linen);
  --bg-card: var(--ivory);
  --bg-hover: #e0e8f0;
  --accent: var(--yale-blue);
  --text-main: var(--deep-space-blue);
  --text-sec: #666666;
  --success: #28a745;
  --warning: #f39c12;
  --danger: #dc3545;
  --border: var(--ash-grey);
  --radius: 4px;
}
[data-theme="dark"] {
  --bg-body: var(--deep-space-blue);
  --bg-card: var(--yale-blue);
  --bg-hover: #1f3f54;
  --accent: var(--ash-grey);
  --text-main: var(--ivory);
  --text-sec: var(--ash-grey);
  --border: var(--ash-grey);
}
* { box-sizing: border-box; margin: 0; padding: 0; }
body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; background: var(--bg-body); color: var(--text-main); font-size: 14px; }
header { background: var(--bg-card); padding: 15px 20px; display: flex; justify-content: space-between; align-items: center; border-bottom: 1px solid var(--border); }
.status-dot { width: 12px; height: 12px; border-radius: 50%; background: var(--danger); display: inline-block; }
.status-dot.connected { background: var(--success); }
.tabs { display: flex; background: var(--bg-card); border-bottom: 1px solid var(--border); }
.tab { flex: 1; padding: 12px; text-align: center; cursor: pointer; color: var(--text-sec); display: flex; align-items: center; justify-content: center; gap: 8px; border-bottom: 2px solid transparent; }
.tab:hover { background: var(--bg-hover); }
.tab.active { color: var(--accent); border-bottom-color: var(--accent); }
.tab svg { width: 16px; height: 16px; fill: currentColor; }
.content { padding: 20px; max-width: 800px; margin: 0 auto; }
.panel { display: none; }
.panel.active { display: block; }
.card { background: var(--bg-card); border: 1px solid var(--border); border-radius: var(--radius); padding: 15px; margin-bottom: 15px; }
h2 { font-size: 16px; margin-bottom: 15px; color: var(--text-main); border-bottom: 1px solid var(--border); padding-bottom: 8px; }
.form-group { margin-bottom: 15px; }
.form-group label { display: block; margin-bottom: 5px; color: var(--text-sec); }
select, input[type="text"], input[type="password"], input[type="number"] { width: 100%; padding: 8px; background: var(--bg-body); border: 1px solid var(--border); color: var(--text-main); border-radius: var(--radius); }
select:focus, input:focus { outline: none; border-color: var(--accent); }
.btn { background: var(--bg-hover); color: var(--text-main); border: 1px solid var(--border); padding: 8px 15px; border-radius: var(--radius); cursor: pointer; display: inline-flex; align-items: center; gap: 8px; font-family: inherit; font-size: 14px;}
.btn:hover { background: var(--accent); color: #fff; border-color: var(--accent); }
.btn-primary { background: var(--accent); color: #fff; border: none; }
.btn-primary:hover { opacity: 0.9; }
.btn-danger { background: var(--danger); color: #fff; border: none; }
.btn-success { background: var(--success); color: #fff; border: none; }
.grid-2 { display: grid; grid-template-columns: 1fr 1fr; gap: 15px; }
.grid-3 { display: grid; grid-template-columns: 1fr 1fr 1fr; gap: 15px; }
.grid-4 { display: grid; grid-template-columns: repeat(4, 1fr); gap: 10px; }
#plotCanvas { width: 100%; height: 350px; background: #101c24; border: 1px solid var(--border); border-radius: var(--radius); margin-bottom: 10px; display: block; }
.legend { display: flex; gap: 15px; justify-content: center; margin-bottom: 15px; }
.legend-item { display: flex; align-items: center; gap: 5px; cursor: pointer; color: var(--text-sec); }
.legend-color { width: 12px; height: 12px; border-radius: 2px; }
.legend-item.disabled { opacity: 0.5; text-decoration: line-through; }
.indicators { display: grid; grid-template-columns: repeat(4, 1fr); gap: 10px; margin-bottom: 15px; }
.ind-box { background: var(--bg-card); border: 1px solid var(--border); padding: 10px; text-align: center; border-radius: var(--radius); }
.ind-label { font-size: 11px; color: var(--text-sec); text-transform: uppercase; }
.ind-val { font-size: 18px; font-weight: bold; margin-top: 5px; }
.switch { position: relative; display: inline-block; width: 40px; height: 20px; }
.switch input { opacity: 0; width: 0; height: 0; }
.slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: var(--bg-body); border: 1px solid var(--border); transition: .2s; border-radius: var(--radius); }
.slider:before { position: absolute; content: ""; height: 14px; width: 14px; left: 2px; bottom: 2px; background-color: var(--text-sec); transition: .2s; border-radius: 2px; }
input:checked + .slider { background-color: var(--accent); border-color: var(--accent); }
input:checked + .slider:before { transform: translateX(20px); background-color: #fff; }
input[type=range] { -webkit-appearance: none; width: 100%; background: transparent; }
input[type=range]::-webkit-slider-thumb { -webkit-appearance: none; height: 16px; width: 16px; border-radius: 50%; background: var(--accent); cursor: pointer; margin-top: -6px; }
input[type=range]::-webkit-slider-runnable-track { width: 100%; height: 4px; cursor: pointer; background: var(--bg-body); border: 1px solid var(--border); border-radius: 2px; }
.radio-group { display: flex; flex-direction: column; gap: 8px; }
.radio-label { display: flex; align-items: center; gap: 8px; cursor: pointer; color: var(--text-main); }
.sys-status { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin-bottom: 15px; }
.sys-box { background: var(--bg-body); padding: 10px; border: 1px solid var(--border); border-radius: var(--radius); display: flex; justify-content: space-between; }
.sys-box span:last-child { color: var(--accent); font-weight: bold; }
.hidden { display: none !important; }
.msg { padding: 10px; margin-bottom: 15px; border-radius: var(--radius); display: none; }
.msg.success { background: var(--success); color: #fff; display: block; }
.msg.error { background: var(--danger); color: #fff; display: block; }
.d-flex { display: flex; gap: 10px; align-items: center; }
</style>
<script>
  const savedTheme = localStorage.getItem('theme') || 'dark';
  document.documentElement.setAttribute('data-theme', savedTheme);
</script>
</head>
<body>

<header>
  <div style="font-size:16px; font-weight:bold;">ESP32 Universal Controller</div>
  <div id="save-status-text" style="font-size:12px; color:var(--text-sec); flex:1; text-align:center;"></div>
  <div class="d-flex">
    <button class="btn" onclick="toggleTheme()" style="padding: 4px 8px;" title="Alternar Tema">
      <svg viewBox="0 0 24 24" width="16" height="16"><path fill="currentColor" d="M12 21c-4.97 0-9-4.03-9-9s4.03-9 9-9 9 4.03 9 9-4.03 9-9 9zm0-16v14c3.87 0 7-3.13 7-7s-3.13-7-7-7z"/></svg>
    </button>
    <span id="global-status" style="padding: 4px 8px; border-radius: 4px; background: var(--border); font-size: 11px; font-weight: bold; margin-right: 15px; color: var(--text-sec);">[○ PARADO]</span>
    <div class="status-dot" id="ws-status" title="Disconnected"></div>
  </div>
</header>

<div class="tabs">
  <div class="tab active" onclick="showTab('tab-io', this)"><svg viewBox="0 0 24 24"><path d="M7 2v11h3v9l7-12h-4l4-8z"/></svg>I/O Config</div>
  <div class="tab" onclick="showTab('tab-ctrl', this)"><svg viewBox="0 0 24 24"><path d="M3 17v2h6v-2H3zM3 5v2h10V5H3zm10 16v-2h8v-2h-8v-2h-2v6h2zM7 9v2H3v2h4v2h2V9H7zm14 4v-2H11v2h10zm-6-4h2V7h4V5h-4V3h-2v6z"/></svg>Controle</div>
  <div class="tab" onclick="showTab('tab-plot', this)"><svg viewBox="0 0 24 24"><path d="M3.5 18.49l6-6.01 4 4L22 6.92l-1.41-1.41-7.09 7.97-4-4L2 16.99z"/></svg>Plotter</div>
  <div class="tab" onclick="showTab('tab-sys', this)"><svg viewBox="0 0 24 24"><path d="M19.14,12.94c0.04-0.3,0.06-0.61,0.06-0.94c0-0.32-0.02-0.64-0.06-0.94l2.03-1.58c0.18-0.14,0.23-0.41,0.12-0.61 l-1.92-3.32c-0.12-0.22-0.37-0.29-0.59-0.22l-2.39,0.96c-0.5-0.38-1.03-0.7-1.62-0.94L14.4,2.81c-0.04-0.24-0.24-0.41-0.48-0.41 h-3.84c-0.24,0-0.43,0.17-0.47,0.41L9.25,5.35C8.66,5.59,8.12,5.92,7.63,6.29L5.24,5.33c-0.22-0.08-0.47,0-0.59,0.22L2.73,8.87 C2.62,9.08,2.66,9.34,2.86,9.48l2.03,1.58C4.84,11.36,4.8,11.69,4.8,12s0.02,0.64,0.06,0.94l-2.03,1.58 c-0.18,0.14-0.23,0.41-0.12,0.61l1.92,3.32c0.12,0.22,0.37,0.29,0.59,0.22l2.39-0.96c0.5,0.38,1.03,0.7,1.62,0.94l0.36,2.54 c0.05,0.24,0.24,0.41,0.48,0.41h3.84c0.24,0,0.43-0.17,0.47-0.41l0.36-2.54c0.59-0.24,1.13-0.56,1.62-0.94l2.39,0.96 c0.22,0.08,0.47,0,0.59-0.22l1.92-3.32c0.12-0.22,0.07-0.49-0.12-0.61L19.14,12.94z M12,15.6c-1.98,0-3.6-1.62-3.6-3.6 s1.62-3.6,3.6-3.6s3.6,1.62,3.6,3.6S13.98,15.6,12,15.6z"/></svg>Sistema</div>
</div>

<div class="content">
  <div id="msg-box" class="msg"></div>

  <!-- TAB 1: I/O CONFIG -->
  <div id="tab-io" class="panel active">
    <div class="card">
      <h2>Canal de Entrada</h2>
      <div class="grid-2">
        <div class="form-group">
          <label>GPIO</label>
          <select id="io-in-pin" onchange="validatePins()">
            <option value="-1">Nenhum</option>
            <option value="4">4</option><option value="5">5</option><option value="13">13</option><option value="14">14</option><option value="15">15</option><option value="16">16</option><option value="17">17</option><option value="18">18</option><option value="19">19</option><option value="21">21</option><option value="22">22</option><option value="23">23</option><option value="25">25</option><option value="26">26</option><option value="27">27</option><option value="32">32</option><option value="33">33</option><option value="34">34</option><option value="35">35</option><option value="36">36</option><option value="39">39</option>
          </select>
        </div>
        <div class="form-group">
          <label>Modo</label>
          <select id="io-in-mode" onchange="toggleADCParams(); validatePins();">
            <option value="1">Entrada Digital</option>
            <option value="3">Entrada Analógica (ADC)</option>
          </select>
          <div id="io-pin-error" style="color: var(--danger); font-size: 11px; margin-top: 5px; display: none;">Pino inválido para ADC. Use 32, 33, 34, 35, 36 ou 39.</div>
        </div>
      </div>
      <div id="adc-params" class="grid-3 hidden">
        <div class="form-group">
          <label>Ganho</label>
          <input type="number" id="adc-gain" step="0.001" value="1.0">
        </div>
        <div class="form-group">
          <label>Offset</label>
          <input type="number" id="adc-offset" step="0.001" value="0.0">
        </div>
        <div class="form-group">
          <label>Amostras Média Móvel</label>
          <input type="number" id="adc-avg" min="1" max="16" value="8">
        </div>
      </div>
    </div>

    <div class="card">
      <h2>Canal de Saída</h2>
      <div class="grid-2">
        <div class="form-group">
          <label>GPIO</label>
          <select id="io-out-pin">
            <option value="-1">Nenhum</option>
            <option value="2">2</option><option value="4">4</option><option value="5">5</option><option value="13">13</option><option value="14">14</option><option value="15">15</option><option value="16">16</option><option value="17">17</option><option value="18">18</option><option value="19">19</option><option value="21">21</option><option value="22">22</option><option value="23">23</option><option value="25">25</option><option value="26">26</option><option value="27">27</option><option value="32">32</option><option value="33">33</option>
          </select>
        </div>
        <div class="form-group">
          <label>Modo</label>
          <select id="io-out-mode" onchange="togglePWMParams()">
            <option value="2">Saída Digital</option>
            <option value="4">Saída PWM</option>
          </select>
        </div>
      </div>
      <div id="pwm-params" class="hidden form-group">
        <label>Frequência Hz</label>
        <input type="number" id="pwm-freq" value="5000">
      </div>
    </div>

    <div class="card">
      <h2>Setpoint Externo</h2>
      <div class="form-group d-flex">
        <label class="switch">
          <input type="checkbox" id="ext-sp-en" onchange="toggleExtSP()">
          <span class="slider"></span>
        </label>
        <span>Habilitar Setpoint Externo via GPIO</span>
      </div>
      <div id="ext-sp-params" class="hidden form-group">
        <label>GPIO</label>
        <select id="ext-sp-pin">
          <option value="32">32</option><option value="33">33</option><option value="34">34</option><option value="35">35</option><option value="36">36</option><option value="39">39</option>
        </select>
      </div>
    </div>
    
    <button id="btn-apply-io" class="btn btn-primary" onclick="applyIO()">Aplicar I/O</button>
  </div>

  <!-- TAB 2: CONTROLE -->
  <div id="tab-ctrl" class="panel">
    <div class="card">
      <h2>Estratégia de Controle</h2>
      <div class="radio-group" style="margin-bottom: 15px;">
        <label class="radio-label"><input type="radio" name="strategy" value="0" checked onchange="updateStrategyUI()"> Direto</label>
        <label class="radio-label"><input type="radio" name="strategy" value="1" onchange="updateStrategyUI()"> Bang-Bang</label>
        <label class="radio-label"><input type="radio" name="strategy" value="2" onchange="updateStrategyUI()"> PID</label>
        <label class="radio-label"><input type="radio" name="strategy" value="3" onchange="updateStrategyUI()"> Equação de Diferenças</label>
      </div>

      <div id="strat-0" class="strat-panel">
        <div style="background:var(--bg-body); padding:10px; border-radius:4px; margin-bottom:15px; border:1px solid var(--border); text-align:center;">
          <p style="color:var(--text-sec)">Estratégia direta: A saída seguirá a entrada e o setpoint não é utilizado.</p>
        </div>
      </div>
      <div id="strat-1" class="strat-panel hidden">
        <div class="grid-2">
          <div class="form-group">
            <label>Limite Superior</label>
            <input type="number" id="bang-high" step="0.1" value="3000">
          </div>
          <div class="form-group">
            <label>Limite Inferior</label>
            <input type="number" id="bang-low" step="0.1" value="1000">
          </div>
        </div>
      </div>
      <div id="strat-2" class="strat-panel hidden">
        <div class="grid-3">
          <div class="form-group"><label>Kp</label><input type="number" id="pid-kp" step="0.001" value="1.0"></div>
          <div class="form-group"><label>Ki</label><input type="number" id="pid-ki" step="0.001" value="0.0"></div>
          <div class="form-group"><label>Kd</label><input type="number" id="pid-kd" step="0.001" value="0.0"></div>
        </div>
      </div>
      <div id="strat-3" class="strat-panel hidden">
        <div style="background:var(--bg-body); padding:10px; border-radius:4px; margin-bottom:15px; border:1px solid var(--border); text-align:center; font-family:monospace;">
          u[k] = Σ bi·e[k-i] - Σ aj·u[k-j]
        </div>
        <div class="grid-4">
          <div class="form-group"><label>b0</label><input type="number" id="diff-b0" step="0.001" value="1.0"></div>
          <div class="form-group"><label>b1</label><input type="number" id="diff-b1" step="0.001" value="0.0"></div>
          <div class="form-group"><label>b2</label><input type="number" id="diff-b2" step="0.001" value="0.0"></div>
          <div class="form-group"><label>b3</label><input type="number" id="diff-b3" step="0.001" value="0.0"></div>
        </div>
        <div class="grid-4">
          <div></div> <!-- spacer -->
          <div class="form-group"><label>a1</label><input type="number" id="diff-a1" step="0.001" value="0.0"></div>
          <div class="form-group"><label>a2</label><input type="number" id="diff-a2" step="0.001" value="0.0"></div>
          <div class="form-group"><label>a3</label><input type="number" id="diff-a3" step="0.001" value="0.0"></div>
        </div>
      </div>
    </div>

    <div class="card">
      <h2>Limites de Saída <small style="color:var(--text-sec); font-size:11px; font-weight:normal; margin-left:10px;">(Define a faixa útil do atuador)</small></h2>
      <div class="grid-2">
        <div class="form-group"><label>Mínimo</label><input type="number" id="out-min" value="0" min="0" max="1023"></div>
        <div class="form-group"><label>Máximo</label><input type="number" id="out-max" value="1023" min="0" max="1023"></div>
      </div>
    </div>

    <div class="card" id="web-sp-panel">
      <h2>Setpoint</h2>
      <div class="d-flex">
        <input type="range" id="sp-slider" min="0" max="4095" step="1" value="0" style="flex:1" oninput="updateSpInput(this.value)" onchange="sendSp()">
        <input type="number" id="sp-input" min="0" max="4095" value="0" style="width:100px" oninput="updateSpSlider(this.value)" onchange="sendSp()">
      </div>
    </div>

    <div class="d-flex" style="gap:15px; margin-top:15px;">
      <button class="btn btn-success" onclick="sendCmd('start')"><svg viewBox="0 0 24 24"><path d="M8 5v14l11-7z"/></svg> Iniciar</button>
      <button class="btn btn-danger" onclick="sendCmd('stop')"><svg viewBox="0 0 24 24"><path d="M6 6h12v12H6z"/></svg> Parar</button>
      <button class="btn btn-primary" onclick="applyCtrl()" style="margin-left:auto;">Aplicar Parâmetros</button>
    </div>
  </div>

  <!-- TAB 3: PLOTTER -->
  <div id="tab-plot" class="panel">
    <div class="indicators">
      <div class="ind-box"><div class="ind-label">Input</div><div class="ind-val" id="ind-in">0.00</div></div>
      <div class="ind-box"><div class="ind-label">Output</div><div class="ind-val" id="ind-out">0.00</div></div>
      <div class="ind-box"><div class="ind-label">Setpoint</div><div class="ind-val" id="ind-sp">0.00</div></div>
      <div class="ind-box"><div class="ind-label">Erro</div><div class="ind-val" id="ind-err">0.00</div></div>
    </div>
    
    <canvas id="plotCanvas"></canvas>
    
    <div class="legend">
      <div class="legend-item" onclick="toggleTrace(0)" id="leg-0"><div class="legend-color" style="background:#f1c40f"></div>Input</div>
      <div class="legend-item" onclick="toggleTrace(1)" id="leg-1"><div class="legend-color" style="background:#00b4d8"></div>Output</div>
      <div class="legend-item" onclick="toggleTrace(2)" id="leg-2"><div class="legend-color" style="background:#2ecc71"></div>Setpoint</div>
      <div class="legend-item" onclick="toggleTrace(3)" id="leg-3"><div class="legend-color" style="background:#e74c3c"></div>Erro</div>
    </div>
    
    <div class="d-flex" style="justify-content:center; gap:15px;">
      <button class="btn" onclick="exportCSV()"><svg viewBox="0 0 24 24" width="16" height="16"><path fill="currentColor" d="M19 9h-4V3H9v6H5l7 7 7-7zM5 18v2h14v-2H5z"/></svg> Exportar CSV</button>
      <button class="btn" onclick="clearPlot()"><svg viewBox="0 0 24 24" width="16" height="16"><path fill="currentColor" d="M15 16h4v2h-4zm0-8h7v2h-7zm0 4h6v2h-6zM3 18c0 1.1.9 2 2 2h6c1.1 0 2-.9 2-2V8H3v10zM14 5h-3l-1-1H6L5 5H2v2h12z"/></svg> Limpar Gráfico</button>
    </div>
  </div>

  <!-- TAB 4: SISTEMA -->
  <div id="tab-sys" class="panel">
    <div class="sys-status">
      <div class="sys-box"><span>Hostname</span><span id="sys-host">--</span></div>
      <div class="sys-box"><span>IP Address</span><span id="sys-ip">--</span></div>
      <div class="sys-box"><span>Wi-Fi SSID</span><span id="sys-ssid">--</span></div>
      <div class="sys-box"><span>RSSI Wi-Fi</span><span id="sys-rssi">--</span></div>
      <div class="sys-box"><span>Uptime</span><span id="sys-up">--</span></div>
      <div class="sys-box"><span>Free Heap</span><span id="sys-heap">--</span></div>
    </div>

    <div class="card">
      <h2>Configuração Wi-Fi</h2>
      <div class="grid-2">
        <div class="form-group"><label>SSID</label><input type="text" id="wifi-ssid"></div>
        <div class="form-group"><label>Senha</label><input type="password" id="wifi-pass"></div>
      </div>
      <button class="btn btn-primary" onclick="applyWifi()">Conectar</button>
    </div>

    <div class="card">
      <div style="display:flex; justify-content:space-between; align-items:center; margin-bottom:15px; border-bottom:1px solid var(--border); padding-bottom:8px;">
        <h2 style="border:none; margin:0; padding:0;">Persistência</h2>
        <div class="d-flex" style="font-size:12px;">
           <label class="switch">
             <input type="checkbox" id="auto-save-en" onchange="toggleAutoSave()">
             <span class="slider"></span>
           </label>
           <span>Auto-Save (NVS)</span>
        </div>
      </div>
      <div id="auto-save-warn" class="hidden" style="color:var(--warning); font-size:11px; margin-bottom:15px; text-align:right;">
        Atenção: A memória Flash possui ciclo de escrita limitado. Use com moderação.
      </div>
      
      <div style="display:flex; gap:15px; flex-wrap:wrap;">
        <button class="btn btn-success" onclick="postApi('/api/save')" style="flex:1; min-width:180px; justify-content:center;"><svg viewBox="0 0 24 24" width="18" height="18"><path fill="currentColor" d="M17 3H5c-1.11 0-2 .9-2 2v14c0 1.1.89 2 2 2h14c1.1 0 2-.9 2-2V7l-4-4zm-5 16c-1.66 0-3-1.34-3-3s1.34-3 3-3 3 1.34 3 3-1.34 3-3 3zm3-10H5V5h10v4z"/></svg> Salvar na Flash</button>
        <button class="btn btn-primary" onclick="exportConfig()" style="flex:1; min-width:150px; justify-content:center;">📤 Exportar (.json)</button>
        <button class="btn btn-primary" onclick="document.getElementById('import-file').click()" style="flex:1; min-width:150px; justify-content:center;">📥 Importar (.json)</button>
        <input type="file" id="import-file" accept=".json" style="display:none;" onchange="importConfig(event)">
      </div>
      <div style="margin-top:15px; border-top:1px dashed var(--border); padding-top:15px;">
        <button class="btn btn-danger" onclick="if(confirm('Restaurar padrões de fábrica?')) postApi('/api/defaults')" style="width:100%; justify-content:center;"><svg viewBox="0 0 24 24" width="18" height="18"><path fill="currentColor" d="M17.65 6.35C16.2 4.9 14.21 4 12 4c-4.42 0-7.99 3.58-7.99 8s3.57 8 7.99 8c3.73 0 6.84-2.55 7.73-6h-2.08c-.82 2.33-3.04 4-5.65 4-3.31 0-6-2.69-6-6s2.69-6 6-6c1.66 0 3.14.69 4.22 1.78L13 11h7V4l-2.35 2.35z"/></svg> Restaurar Defaults</button>
      </div>
    </div>
  </div>
</div>

<script>
let ws = null;
const plotterBufSize = 500;
let csvHistory = [];
let t0 = 0;
let tData = { t:[], in:[], out:[], sp:[], err:[] };
let traces = [true, true, true, true]; // visibilities

let autoSaveTimer = null;
let autoSaveEnabled = false;
let latestConfigJSON = null;
let lastSaveTime = null;

setInterval(() => {
  if(!autoSaveEnabled || !lastSaveTime) return;
  const mins = Math.floor((Date.now() - lastSaveTime) / 60000);
  const st = document.getElementById('save-status-text');
  if(mins === 0) st.innerText = "Salvo agora";
  else st.innerText = `Salvo há ${mins} minuto${mins > 1 ? 's' : ''}`;
}, 10000);

function scheduleAutoSave() {
  if (!autoSaveEnabled) return;
  if (autoSaveTimer) clearTimeout(autoSaveTimer);
  document.getElementById('save-status-text').innerText = "Salvando...";
  autoSaveTimer = setTimeout(async () => {
    if (await postApi('/api/save')) {
      lastSaveTime = Date.now();
      showMsg("Auto-Save: Salvo na Flash", false);
      document.getElementById('save-status-text').innerText = "Salvo agora";
    } else {
      document.getElementById('save-status-text').innerText = "Erro ao salvar";
    }
  }, 60000);
}

function toggleAutoSave() {
  autoSaveEnabled = document.getElementById('auto-save-en').checked;
  const warn = document.getElementById('auto-save-warn');
  const st = document.getElementById('save-status-text');
  if (autoSaveEnabled) {
    warn.classList.remove('hidden');
    if(!lastSaveTime) st.innerText = "Auto-save ativado (Aguardando)";
    else {
      const mins = Math.floor((Date.now() - lastSaveTime) / 60000);
      if(mins === 0) st.innerText = "Salvo agora";
      else st.innerText = `Salvo há ${mins} minuto${mins > 1 ? 's' : ''}`;
    }
  } else {
    warn.classList.add('hidden');
    st.innerText = "";
  }
}

function exportConfig() {
  if (!latestConfigJSON) { showMsg("Configuração ainda não carregada", true); return; }
  const blob = new Blob([JSON.stringify(latestConfigJSON, null, 2)], { type: 'application/json' });
  const url = URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.href = url;
  a.download = 'config_esp32.json';
  document.body.appendChild(a);
  a.click();
  document.body.removeChild(a);
}

function importConfig(e) {
  const file = e.target.files[0];
  if (!file) return;
  const reader = new FileReader();
  reader.onload = async (evt) => {
    try {
      const json = JSON.parse(evt.target.result);
      await postApi('/api/import-config', json);
      if(ws && ws.readyState===1) ws.send(JSON.stringify({cmd:"get_config"}));
    } catch(err) {
      showMsg("Erro ao ler JSON: " + err.message, true);
    }
  };
  reader.readAsText(file);
  e.target.value = ''; // Reset input
}

function clearPlot() {
  tData = { t:[], in:[], out:[], sp:[], err:[] };
  csvHistory = [];
  t0 = 0;
  drawPlot();
}

function toggleTheme() {
  const t = document.documentElement.getAttribute('data-theme') === 'dark' ? 'light' : 'dark';
  document.documentElement.setAttribute('data-theme', t);
  localStorage.setItem('theme', t);
  drawPlot(); // Redraw with new colors
}

function showTab(id, el) {
  document.querySelectorAll('.panel').forEach(p => p.classList.remove('active'));
  document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
  document.getElementById(id).classList.add('active');
  if(el) el.classList.add('active');
}

function updateGlobalStatus(running) {
  const badge = document.getElementById('global-status');
  if(running) {
    badge.innerText = '[● RODANDO]';
    badge.style.background = 'var(--success)';
    badge.style.color = '#fff';
  } else {
    badge.innerText = '[○ PARADO]';
    badge.style.background = 'var(--border)';
    badge.style.color = 'var(--text-sec)';
  }
}

function validatePins() {
  const mode = document.getElementById('io-in-mode').value;
  const pin = parseInt(document.getElementById('io-in-pin').value);
  const errDiv = document.getElementById('io-pin-error');
  const btn = document.getElementById('btn-apply-io');
  
  if (mode === '3') { // Analog Input
    const validADC1 = [32, 33, 34, 35, 36, 39];
    if (pin !== -1 && !validADC1.includes(pin)) {
      errDiv.style.display = 'block';
      btn.disabled = true;
      btn.style.opacity = '0.5';
      return;
    }
  }
  errDiv.style.display = 'none';
  btn.disabled = false;
  btn.style.opacity = '1';
}

function toggleADCParams() {
  const mode = document.getElementById('io-in-mode').value;
  const p = document.getElementById('adc-params');
  if(mode == '3') p.classList.remove('hidden'); else p.classList.add('hidden');
}

function togglePWMParams() {
  const mode = document.getElementById('io-out-mode').value;
  const p = document.getElementById('pwm-params');
  if(mode == '4') p.classList.remove('hidden'); else p.classList.add('hidden');
}

function toggleExtSP() {
  const en = document.getElementById('ext-sp-en').checked;
  const p = document.getElementById('ext-sp-params');
  const w = document.getElementById('web-sp-panel');
  if(en) { p.classList.remove('hidden'); w.classList.add('hidden'); }
  else { p.classList.add('hidden'); w.classList.remove('hidden'); }
}

function updateStrategyUI() {
  const val = document.querySelector('input[name="strategy"]:checked').value;
  document.querySelectorAll('.strat-panel').forEach(p => p.classList.add('hidden'));
  document.getElementById('strat-'+val).classList.remove('hidden');
  
  const w = document.getElementById('web-sp-panel');
  if (val === '0') {
    w.classList.add('hidden'); // Hide setpoint for direct mode
  } else if (!document.getElementById('ext-sp-en').checked) {
    w.classList.remove('hidden');
  }
}

function showMsg(msg, isErr=false) {
  const b = document.getElementById('msg-box');
  b.textContent = msg;
  b.className = 'msg ' + (isErr ? 'error' : 'success');
  setTimeout(() => b.className = 'msg', 3000);
}

async function postApi(url, data=null) {
  try {
    const opts = { method: 'POST' };
    if (data) {
      opts.headers = {'Content-Type': 'application/json'};
      opts.body = JSON.stringify(data);
    }
    const res = await fetch(url, opts);
    if (!res.ok) throw new Error("Status " + res.status);
    showMsg("Sucesso!", false);
    return true;
  } catch(err) {
    showMsg("Erro: " + err.message, true);
    return false;
  }
}

function applyIO() {
  const data = {
    inputPin: parseInt(document.getElementById('io-in-pin').value),
    inputMode: parseInt(document.getElementById('io-in-mode').value),
    outputPin: parseInt(document.getElementById('io-out-pin').value),
    outputMode: parseInt(document.getElementById('io-out-mode').value),
    inputGain: parseFloat(document.getElementById('adc-gain').value),
    inputOffset: parseFloat(document.getElementById('adc-offset').value),
    movingAvgSamples: parseInt(document.getElementById('adc-avg').value),
    pwmFrequency: parseInt(document.getElementById('pwm-freq').value),
    setpointSource: document.getElementById('ext-sp-en').checked ? 1 : 0,
    setpointPin: parseInt(document.getElementById('ext-sp-pin').value)
  };
  postApi('/api/io-config', data).then(() => scheduleAutoSave());
}

function applyCtrl() {
  const strat = parseInt(document.querySelector('input[name="strategy"]:checked').value);
  const data = {
    strategy: strat,
    bangHigh: parseFloat(document.getElementById('bang-high').value),
    bangLow: parseFloat(document.getElementById('bang-low').value),
    kp: parseFloat(document.getElementById('pid-kp').value),
    ki: parseFloat(document.getElementById('pid-ki').value),
    kd: parseFloat(document.getElementById('pid-kd').value),
    b: [
      parseFloat(document.getElementById('diff-b0').value),
      parseFloat(document.getElementById('diff-b1').value),
      parseFloat(document.getElementById('diff-b2').value),
      parseFloat(document.getElementById('diff-b3').value)
    ],
    a: [
      parseFloat(document.getElementById('diff-a1').value),
      parseFloat(document.getElementById('diff-a2').value),
      parseFloat(document.getElementById('diff-a3').value)
    ],
    outputMin: parseFloat(document.getElementById('out-min').value),
    outputMax: parseFloat(document.getElementById('out-max').value),
    setpoint: parseFloat(document.getElementById('sp-input').value)
  };
  postApi('/api/control-config', data).then(() => scheduleAutoSave());
}

function applyWifi() {
  const data = {
    ssid: document.getElementById('wifi-ssid').value,
    password: document.getElementById('wifi-pass').value
  };
  postApi('/api/wifi', data);
}

function updateSpSlider(val) { document.getElementById('sp-slider').value = val; }
function updateSpInput(val) { document.getElementById('sp-input').value = val; }
function sendSp() {
  const val = parseFloat(document.getElementById('sp-input').value);
  if(ws && ws.readyState===1) ws.send(JSON.stringify({cmd:"setpoint", value:val}));
}
function sendCmd(cmd) {
  if(ws && ws.readyState===1) ws.send(JSON.stringify({cmd:cmd}));
}

function connectWS() {
  const proto = location.protocol === 'https:' ? 'wss' : 'ws';
  ws = new WebSocket(`${proto}://${location.host}/ws`);
  ws.onopen = () => {
    document.getElementById('ws-status').className = 'status-dot connected';
    ws.send(JSON.stringify({cmd:"get_config"}));
  };
  ws.onclose = () => {
    document.getElementById('ws-status').className = 'status-dot';
    setTimeout(connectWS, 3000);
  };
  ws.onmessage = (e) => {
    try {
      const msg = JSON.parse(e.data);
      if(msg.type === "tel") handleTel(msg);
      else if(msg.type === "config") handleConfig(msg);
      else if(msg.type === "status") handleStatus(msg);
    } catch(err){}
  };
}

function handleConfig(cfg) {
  latestConfigJSON = cfg;
  if(cfg.io) {
    document.getElementById('io-in-pin').value = cfg.io.inputPin;
    document.getElementById('io-in-mode').value = cfg.io.inputMode;
    document.getElementById('io-out-pin').value = cfg.io.outputPin;
    document.getElementById('io-out-mode').value = cfg.io.outputMode;
    document.getElementById('adc-gain').value = cfg.io.inputGain;
    document.getElementById('adc-offset').value = cfg.io.inputOffset;
    document.getElementById('adc-avg').value = cfg.io.movingAvgSamples;
    document.getElementById('pwm-freq').value = cfg.io.pwmFrequency;
    const isExt = (cfg.io.setpointSource === 1);
    document.getElementById('ext-sp-en').checked = isExt;
    document.getElementById('ext-sp-pin').value = cfg.io.setpointPin;
    toggleADCParams();
    togglePWMParams();
    toggleExtSP();
    validatePins();
  }
  if(cfg.ctrl) {
    document.querySelector(`input[name="strategy"][value="${cfg.ctrl.strategy}"]`).checked = true;
    updateStrategyUI();
    document.getElementById('bang-high').value = cfg.ctrl.bangHigh;
    document.getElementById('bang-low').value = cfg.ctrl.bangLow;
    document.getElementById('pid-kp').value = cfg.ctrl.kp;
    document.getElementById('pid-ki').value = cfg.ctrl.ki;
    document.getElementById('pid-kd').value = cfg.ctrl.kd;
    if(cfg.ctrl.b && cfg.ctrl.b.length>=4) {
      document.getElementById('diff-b0').value = cfg.ctrl.b[0];
      document.getElementById('diff-b1').value = cfg.ctrl.b[1];
      document.getElementById('diff-b2').value = cfg.ctrl.b[2];
      document.getElementById('diff-b3').value = cfg.ctrl.b[3];
    }
    if(cfg.ctrl.a && cfg.ctrl.a.length>=3) {
      document.getElementById('diff-a1').value = cfg.ctrl.a[0];
      document.getElementById('diff-a2').value = cfg.ctrl.a[1];
      document.getElementById('diff-a3').value = cfg.ctrl.a[2];
    }
    document.getElementById('out-min').value = cfg.ctrl.outputMin;
    document.getElementById('out-max').value = cfg.ctrl.outputMax;
    document.getElementById('sp-input').value = cfg.ctrl.setpoint;
    document.getElementById('sp-slider').value = cfg.ctrl.setpoint;
    if (cfg.ctrl.running !== undefined) updateGlobalStatus(cfg.ctrl.running);
  }
}

function handleStatus(st) {
  if(st.ip) document.getElementById('sys-ip').innerText = st.ip;
  if(st.ssid) {
    document.getElementById('sys-ssid').innerText = st.ssid;
    // Auto-fill Wi-Fi config input if empty
    const ssidInput = document.getElementById('wifi-ssid');
    if (!ssidInput.value && st.ssid !== "ESP32-Controller") {
      ssidInput.value = st.ssid;
      ssidInput.placeholder = `Atualmente conectado a: ${st.ssid}`;
    }
  }
  if(st.hostname) document.getElementById('sys-host').innerText = st.hostname;
  if(st.rssi !== undefined) document.getElementById('sys-rssi').innerText = st.rssi + " dBm";
  if(st.uptime) document.getElementById('sys-up').innerText = st.uptime + " s";
  if(st.heap) document.getElementById('sys-heap').innerText = st.heap + " B";
  if(st.running !== undefined) updateGlobalStatus(st.running);
}

function handleTel(t) {
  if(t0 === 0) t0 = t.t;
  const rel = (t.t - t0)/1000.0;
  
  tData.t.push(rel);
  tData.in.push(t.in);
  tData.out.push(t.out);
  tData.sp.push(t.sp);
  tData.err.push(t.err);
  
  csvHistory.push(`${t.t},${t.in},${t.out},${t.sp},${t.err}`);
  
  if(tData.t.length > plotterBufSize) {
    tData.t.shift();
    tData.in.shift();
    tData.out.shift();
    tData.sp.shift();
    tData.err.shift();
  }
  
  document.getElementById('ind-in').innerText = t.in.toFixed(2);
  document.getElementById('ind-out').innerText = t.out.toFixed(2);
  document.getElementById('ind-sp').innerText = t.sp.toFixed(2);
  document.getElementById('ind-err').innerText = t.err.toFixed(2);
  
  requestAnimationFrame(drawPlot);
}

function toggleTrace(idx) {
  traces[idx] = !traces[idx];
  document.getElementById('leg-'+idx).classList.toggle('disabled', !traces[idx]);
  requestAnimationFrame(drawPlot);
}

function drawPlot() {
  const canvas = document.getElementById('plotCanvas');
  const ctx = canvas.getContext('2d');
  const w = canvas.width = canvas.clientWidth;
  const h = canvas.height = canvas.clientHeight;
  
  ctx.clearRect(0, 0, w, h);
  if(tData.t.length < 2) return;
  
  let min = Infinity, max = -Infinity;
  const arrs = [tData.in, tData.out, tData.sp, tData.err];
  for(let i=0; i<4; i++) {
    if(!traces[i]) continue;
    for(let j=0; j<arrs[i].length; j++) {
      if(arrs[i][j] < min) min = arrs[i][j];
      if(arrs[i][j] > max) max = arrs[i][j];
    }
  }
  
  if(min === Infinity) { min = 0; max = 100; }
  else if(min === max) { min -= 10; max += 10; }
  else {
    const pad = (max - min)*0.1;
    min -= pad; max += pad;
  }
  
  const padX = 40, padY = 20, plotW = w - padX - 10, plotH = h - padY - 10;
  
  ctx.strokeStyle = '#2a3a4a';
  ctx.lineWidth = 1;
  ctx.fillStyle = '#8899aa';
  ctx.font = '10px sans-serif';
  ctx.textAlign = 'right';
  ctx.textBaseline = 'middle';
  
  for(let i=0; i<=5; i++) {
    const y = 10 + plotH - (i/5)*plotH;
    ctx.beginPath(); ctx.moveTo(padX, y); ctx.lineTo(w-10, y); ctx.stroke();
    ctx.fillText((min + (i/5)*(max-min)).toFixed(1), padX-5, y);
  }
  
  const tMin = tData.t[0], tMax = tData.t[tData.t.length-1];
  const tSpan = Math.max(tMax - tMin, 0.1);
  
  const colors = ['#f1c40f', '#00b4d8', '#2ecc71', '#e74c3c'];
  ctx.lineWidth = 2;
  
  for(let i=0; i<4; i++) {
    if(!traces[i]) continue;
    ctx.strokeStyle = colors[i];
    ctx.beginPath();
    for(let j=0; j<tData.t.length; j++) {
      const px = padX + ((tData.t[j] - tMin)/tSpan)*plotW;
      const py = 10 + plotH - ((arrs[i][j] - min)/(max - min))*plotH;
      if(j===0) ctx.moveTo(px, py); else ctx.lineTo(px, py);
    }
    ctx.stroke();
  }
  
  ctx.textAlign = 'left';
  ctx.textBaseline = 'top';
  ctx.fillText(tMin.toFixed(1)+"s", padX, h-15);
  ctx.textAlign = 'right';
  ctx.fillText(tMax.toFixed(1)+"s", w-10, h-15);
}

function exportCSV() {
  const header = "Timestamp(ms),Input,Output,Setpoint,Error\n";
  const blob = new Blob([header + csvHistory.join('\n')], {type: 'text/csv'});
  const url = URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.href = url;
  a.download = 'telemetry.csv';
  a.click();
  URL.revokeObjectURL(url);
}

window.onload = () => {
  toggleADCParams();
  togglePWMParams();
  toggleExtSP();
  updateStrategyUI();
  connectWS();
  drawPlot(); // initial draw
};
</script>
</body>
</html>
)rawliteral";
