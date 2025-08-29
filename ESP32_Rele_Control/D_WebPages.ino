// D_WebPages.ino - HTML page generation
// Simplified version - for full version with all features, restore from backup

String getHomeHTML() {
  return R"rawliteral(<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>lux2g0</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
:root{--bg-primary:#0f0f14;--bg-secondary:#1a1a24;--bg-tertiary:#252533;--text-primary:#ffffff;--text-secondary:#b0b0b0;--accent:#B84444;--success:#4CAF50;--warning:#ffeb3b;--danger:#B84444;--info:#03a9f4;--purple:#9c27b0;--border:#2a2a3e;--header-bg:#B84444}
body.light{--bg-primary:#f5f5f5;--bg-secondary:#fff;--bg-tertiary:#e0e0e0;--text-primary:#333;--text-secondary:#666;--accent:#2196F3;--warning:#FFC107;--danger:#f44336;--info:#03a9f4;--purple:#9c27b0;--border:#ddd;--header-bg:#2196F3}
body{font-family:Arial,sans-serif;background:var(--bg-primary);color:var(--text-primary);margin:0;padding-top:60px;visibility:hidden}
.header{position:fixed;top:0;left:0;right:0;height:60px;background:var(--header-bg);display:flex;align-items:center;justify-content:space-between;padding:0 20px;box-shadow:0 2px 10px rgba(0,0,0,0.2);z-index:100}
.header h1{color:#fff;font-size:24px;font-weight:600}
.header-buttons{display:flex;gap:10px;align-items:center}
.icon-button{background:rgba(255,255,255,0.2);border:none;width:40px;height:40px;border-radius:50%;cursor:pointer;display:flex;align-items:center;justify-content:center;transition:all 0.3s ease}
.icon-button:hover{background:rgba(255,255,255,0.3);transform:scale(1.05)}
.icon-button svg{width:24px;height:24px;fill:white}
.container{padding:20px}
.section{background:var(--bg-secondary);border-radius:8px;padding:20px;margin-bottom:20px}
.section h2{color:var(--accent);margin-bottom:15px}
.status-grid{display:grid;grid-template-columns:1fr;gap:10px;margin-bottom:15px}
.status-item{display:flex;justify-content:space-between;align-items:center;padding:12px 15px;background:var(--bg-tertiary);border-radius:5px}
.status-label{color:var(--text-primary)}
.status-value{color:var(--text-primary);font-weight:600}
.status-value.online{color:var(--success)}
.status-value.offline{color:var(--danger)}
.relay-control{display:flex;justify-content:space-between;align-items:center;padding:15px;background:var(--bg-tertiary);border-radius:5px;margin-bottom:10px}
.status{font-size:14px;color:var(--text-primary)}
.status.on{color:#4CAF50;font-weight:bold}
.status.off{color:var(--danger);font-weight:bold}
.switch{position:relative;display:inline-block;width:60px;height:34px}
.switch input{opacity:0;width:0;height:0}
.switch input:disabled+.slider{opacity:0.5;cursor:not-allowed}
.slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background:var(--text-secondary);border-radius:34px}
.slider:before{position:absolute;content:"";height:26px;width:26px;left:4px;bottom:4px;background:white;transition:.4s;border-radius:50%}
input:checked+.slider{background:var(--accent)}
input:checked+.slider:before{transform:translateX(26px)}
.mode-buttons{display:flex;gap:10px;margin-top:15px}
.mode-button{flex:1;padding:10px;background:var(--bg-tertiary);border:2px solid var(--border);border-radius:5px;color:var(--text-primary);cursor:pointer;transition:all 0.3s}
.mode-button.active{border-color:var(--accent);background:var(--accent);color:white}
.mode-button:hover{transform:translateY(-2px)}
a{color:var(--info);text-decoration:none}
.input-field{padding:5px;background:var(--bg-tertiary);border:1px solid var(--border);border-radius:5px;color:var(--text-primary)}
.button{min-width:120px;height:48px;padding:12px 40px;background:var(--accent);color:white;border:none;border-radius:5px;cursor:pointer;margin:5px;font-size:16px;font-weight:500;transition:all 0.3s ease;display:inline-flex;align-items:center;justify-content:center}
.button:hover{opacity:0.9;transform:translateY(-1px)}
.button:disabled{opacity:0.6;cursor:not-allowed;transform:none}
</style>
<script>
(function(){
  if(localStorage.getItem('theme')==='light'){
    document.documentElement.classList.add('light-theme');
  }
})();
</script>
</head>
<body>
<script>
if(localStorage.getItem('theme')==='light'){document.body.classList.add('light');}
document.body.style.visibility='visible';
// Polyfill for deprecated setCapture
if(!Element.prototype.setPointerCapture && Element.prototype.setCapture){
  Element.prototype.setPointerCapture = Element.prototype.setCapture;
}
</script>
<div class="header">
<h1>lux2g0</h1>
<div class="header-buttons">
<button class="icon-button" onclick="location.href='/'">
<svg viewBox="0 0 24 24"><path d="M10 20v-6h4v6h5v-8h3L12 3 2 12h3v8z"/></svg>
</button>
<button class="icon-button" onclick="location.href='/settings'">
<svg viewBox="0 0 24 24"><path d="M12 15.5A3.5 3.5 0 0 1 8.5 12A3.5 3.5 0 0 1 12 8.5a3.5 3.5 0 0 1 3.5 3.5a3.5 3.5 0 0 1-3.5 3.5m7.43-2.53c.04-.32.07-.64.07-.97c0-.33-.03-.65-.07-.97l2.11-1.63c.19-.15.24-.42.12-.64l-2-3.46c-.12-.22-.39-.3-.61-.22l-2.49 1c-.52-.39-1.06-.73-1.69-.98l-.37-2.65A.506.506 0 0 0 14 2h-4c-.25 0-.46.18-.5.42l-.37 2.65c-.63.25-1.17.59-1.69.98l-2.49-1c-.22-.08-.49 0-.61.22l-2 3.46c-.13.22-.07.49.12.64L4.57 11c-.04.32-.07.64-.07.97c0 .33.03.65.07.97l-2.11 1.63c-.19.15-.24.42-.12.64l2 3.46c.12.22.39.3.61.22l2.49-1c.52.39 1.06.73 1.69.98l.37 2.65c.04.24.25.42.5.42h4c.25 0 .46-.18.5-.42l.37-2.65c.63-.25 1.17-.59 1.69-.98l2.49 1c.22.08.49 0 .61-.22l2-3.46c.13-.22.07-.49-.12-.64l-2.11-1.63Z"/></svg>
</button>
<button class="icon-button" onclick="location.href='/about'">
<svg viewBox="0 0 24 24"><path d="M13,9H11V7H13M13,17H11V11H13M12,2A10,10 0 0,0 2,12A10,10 0 0,0 12,22A10,10 0 0,0 22,12A10,10 0 0,0 12,2Z"/></svg>
</button>
</div>
</div>
<div class="container">
<div class="section">
<h2>Status</h2>
<div class="status-grid">
<div class="status-item">
<span class="status-label">Connection</span>
<span id="connection" class="status-value online">Connected</span>
</div>
<div class="status-item">
<span class="status-label">Network</span>
<span id="network" class="status-value offline">Offline</span>
</div>
<div class="status-item">
<span class="status-label">IP Address</span>
<span id="ip-address" class="status-value">192.168.4.1</span>
</div>
<div class="status-item">
<span class="status-label">Mode</span>
<span id="mode-status" class="status-value">Manual</span>
</div>
</div>
</div>
<div class="section">
<h2>Operation Mode</h2>
<div class="mode-buttons">
<button class="mode-button" id="mode-0" onclick="setMode(0)">Manual</button>
<button class="mode-button" id="mode-1" onclick="setMode(1)">LDR</button>
<button class="mode-button" id="mode-2" onclick="setMode(2)">Scheduled</button>
</div>
<div id="mode-options" style="margin-top:15px">
<div id="ldr-options" style="display:none">
<div class="status-item" style="margin-top:10px">
<span class="status-label">LDR Value</span>
<span id="ldr-value" class="status-value">0</span>
</div>
<div class="status-item">
<span class="status-label">Threshold</span>
<input type="number" id="ldr-threshold" min="0" max="4095" value="500" style="width:80px;text-align:right" onchange="setThreshold(this.value)" class="input-field">
</div>
</div>
<div id="schedule-options" style="display:none">
<div class="status-item" style="margin-top:10px">
<span class="status-label">NTP Time</span>
<span id="ntp-time" style="font-size:14px;color:var(--text-primary);font-weight:normal">--:--:--</span>
</div>
<div class="status-grid" style="margin-top:10px">
<div class="status-item">
<span class="status-label">Turn ON at</span>
<input type="time" id="on-time" value="08:00:00" step="1" class="input-field" style="width:auto;text-align:right" onchange="updateSchedule()">
</div>
<div class="status-item">
<span class="status-label">Turn OFF at</span>
<input type="time" id="off-time" value="20:00:00" step="1" class="input-field" style="width:auto;text-align:right" onchange="updateSchedule()">
</div>
</div>
</div>
</div>
</div>
<div class="section">
<h2>Relay Control</h2>
<div class="relay-control">
<div>
<h3>Relay</h3>
<span id="status1" class="status">OFF</span>
</div>
<label class="switch">
<input type="checkbox" id="relay1" onchange="toggleRelay(1,this.checked)">
<span class="slider"></span>
</label>
</div>
</div>
</div>
<script>
async function toggleRelay(n,s){
  // Prevent toggle if disabled
  if(document.getElementById('relay'+n).disabled){
    return;
  }
  const r=await fetch('/api/control',{
    method:'POST',
    headers:{'Content-Type':'application/json'},
    body:JSON.stringify({rele:n,state:s})
  });
  if(r.ok){
    const d=await r.json();
    updateStatus(d);
  }else if(r.status===403){
    // If forbidden (not in manual mode), refresh status
    getStatus();
  }
}
async function setMode(m){
  console.log('Setting mode to:', m);
  try{
    const r=await fetch('/api/auto',{
      method:'POST',
      headers:{'Content-Type':'application/json'},
      body:JSON.stringify({mode:m})
    });
    if(r.ok){
      const d=await r.json();
      console.log('Mode response:', d);
      updateStatus(d);
      
      // Force update of schedule options visibility
      if(m === 2) {
        const schedOpts = document.getElementById('schedule-options');
        if(schedOpts) {
          schedOpts.style.display = 'block';
          console.log('Scheduled options shown');
        }
      }
    } else {
      console.error('Mode change failed:', r.status);
    }
  }catch(e){
    console.error('Mode change error:', e);
  }
}
function updateStatus(d){
  if(d.rele1!==undefined){
    document.getElementById('relay1').checked=d.rele1;
    document.getElementById('status1').textContent=d.rele1?'ON':'OFF';
    document.getElementById('status1').className=d.rele1?'status on':'status off';
  }
  if(d.rele2!==undefined){
    const relay2 = document.getElementById('relay2');
    const status2 = document.getElementById('status2');
    if(relay2 && status2) {
      relay2.checked=d.rele2;
      status2.textContent=d.rele2?'ON':'OFF';
      status2.className=d.rele2?'status on':'status off';
    }
  }
  if(d.mode!==undefined){
    // Update mode buttons
    document.querySelectorAll('.mode-button').forEach(b=>b.classList.remove('active'));
    const modeBtn = document.getElementById('mode-'+d.mode);
    if(modeBtn) modeBtn.classList.add('active');
    
    const modeNames=['Manual','LDR','Scheduled'];
    const modeStatus = document.getElementById('mode-status');
    if(modeStatus) {
      modeStatus.textContent=modeNames[d.mode];
      modeStatus.className='status-value';
    }
    
    // Disable relay buttons if not in manual mode
    const isManual = d.mode === 0;
    const relay1 = document.getElementById('relay1');
    const relay2 = document.getElementById('relay2');
    if(relay1) relay1.disabled = !isManual;
    if(relay2) relay2.disabled = !isManual;
    
    // Show/hide mode options
    const ldrOpts = document.getElementById('ldr-options');
    const schedOpts = document.getElementById('schedule-options');
    if(ldrOpts) ldrOpts.style.display = d.mode === 1 ? 'block' : 'none';
    if(schedOpts) schedOpts.style.display = d.mode === 2 ? 'block' : 'none';
    
    console.log('Mode changed to:', d.mode, modeNames[d.mode]);
  }
  if(d.ldrValue!==undefined){
    document.getElementById('ldr-value').textContent = d.ldrValue + ' (' + d.ldrPercent + '%)';
  }
  if(d.ldrThreshold!==undefined){
    document.getElementById('ldr-threshold').value = d.ldrThreshold;
  }
  if(d.schedule){
    document.getElementById('on-time').value = 
      String(d.schedule.onHour).padStart(2,'0') + ':' + 
      String(d.schedule.onMinute).padStart(2,'0');
    document.getElementById('off-time').value = 
      String(d.schedule.offHour).padStart(2,'0') + ':' + 
      String(d.schedule.offMinute).padStart(2,'0');
  }
  if(d.currentTime){
    document.getElementById('ntp-time').textContent = d.currentTime;
  }
  if(d.wifiConnected!==undefined){
    // Connection shows Connected (green) when in AP mode or connected to WiFi
    document.getElementById('connection').textContent='Connected';
    document.getElementById('connection').className='status-value online';
    // Network shows Online/Offline based on WiFi connection
    document.getElementById('network').textContent=d.wifiConnected?'Online':'Offline';
    document.getElementById('network').className=d.wifiConnected?'status-value online':'status-value offline';
  }
  if(d.localIP||d.apIP){
    document.getElementById('ip-address').textContent=d.wifiConnected?d.localIP:d.apIP;
  }
  if(d.schedule){
    // Update schedule time inputs with seconds
    const onTime = `${String(d.schedule.onHour).padStart(2,'0')}:${String(d.schedule.onMinute).padStart(2,'0')}:${String(d.schedule.onSecond || 0).padStart(2,'0')}`;
    const offTime = `${String(d.schedule.offHour).padStart(2,'0')}:${String(d.schedule.offMinute).padStart(2,'0')}:${String(d.schedule.offSecond || 0).padStart(2,'0')}`;
    const onTimeInput = document.getElementById('on-time');
    const offTimeInput = document.getElementById('off-time');
    if(onTimeInput) onTimeInput.value = onTime;
    if(offTimeInput) offTimeInput.value = offTime;
  }
}
async function getStatus(){
  try{
    const r=await fetch('/api/status');
    if(r.ok){
      const d=await r.json();
      updateStatus(d);
    }
  }catch(e){}
}
async function setThreshold(value){
  // Validate value
  const val = parseInt(value);
  if(val < 0 || val > 4095) return;
  
  const r = await fetch('/api/threshold',{
    method:'POST',
    headers:{'Content-Type':'application/json'},
    body:JSON.stringify({threshold:val})
  });
  if(r.ok){
    const d = await r.json();
    updateStatus(d);
  }
}
async function updateSchedule(){
  const onTime = document.getElementById('on-time').value.split(':');
  const offTime = document.getElementById('off-time').value.split(':');
  const r = await fetch('/api/schedule',{
    method:'POST',
    headers:{'Content-Type':'application/json'},
    body:JSON.stringify({
      onHour:parseInt(onTime[0]),
      onMinute:parseInt(onTime[1]),
      onSecond:parseInt(onTime[2] || 0),
      offHour:parseInt(offTime[0]),
      offMinute:parseInt(offTime[1]),
      offSecond:parseInt(offTime[2] || 0)
    })
  });
  if(r.ok){
    const d = await r.json();
    updateStatus(d);
  }
}
setInterval(getStatus,1000); // Faster updates
getStatus();
</script>
</body>
</html>)rawliteral";
}

String getSettingsHTML() {
  return R"rawliteral(<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>lux2g0</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
:root{--bg-primary:#0f0f14;--bg-secondary:#1a1a24;--bg-tertiary:#252533;--text-primary:#ffffff;--text-secondary:#b0b0b0;--accent:#B84444;--success:#4CAF50;--danger:#B84444;--border:#2a2a3e;--header-bg:#B84444}
body.light{--bg-primary:#f5f5f5;--bg-secondary:#fff;--bg-tertiary:#e0e0e0;--text-primary:#333;--text-secondary:#666;--accent:#2196F3;--danger:#f44336;--border:#ddd;--header-bg:#2196F3}
body{font-family:Arial,sans-serif;background:var(--bg-primary);color:var(--text-primary);margin:0;padding-top:60px;visibility:hidden}
.header{position:fixed;top:0;left:0;right:0;height:60px;background:var(--header-bg);display:flex;align-items:center;justify-content:space-between;padding:0 20px;box-shadow:0 2px 10px rgba(0,0,0,0.2);z-index:100}
.header h1{color:#fff;font-size:24px;font-weight:600}
.header-buttons{display:flex;gap:10px;align-items:center}
.icon-button{background:rgba(255,255,255,0.2);border:none;width:40px;height:40px;border-radius:50%;cursor:pointer;display:flex;align-items:center;justify-content:center;transition:all 0.3s ease}
.icon-button:hover{background:rgba(255,255,255,0.3);transform:scale(1.05)}
.icon-button svg{width:24px;height:24px;fill:white}
.container{padding:20px}
.section{background:var(--bg-secondary);border-radius:8px;padding:20px;margin-bottom:20px}
.section h2{color:var(--accent);margin-bottom:15px}
.input-field{width:100%;padding:10px;margin:5px 0;background:var(--bg-tertiary);border:1px solid var(--border);border-radius:5px;color:var(--text-primary)}
.button{min-width:120px;height:48px;padding:12px 30px;background:var(--accent);color:white;border:none;border-radius:5px;cursor:pointer;margin:5px;font-size:16px;font-weight:500;transition:all 0.3s ease;display:inline-flex;align-items:center;justify-content:center}
.button:hover{opacity:0.9;transform:translateY(-1px)}
.button:disabled{opacity:0.6;cursor:not-allowed;transform:none}
.network-item{padding:10px;background:var(--bg-tertiary);border-radius:5px;margin:5px 0;display:flex;justify-content:space-between;align-items:center}
.theme-toggle{display:flex;align-items:center;justify-content:space-between;padding:10px;background:var(--bg-tertiary);border-radius:5px;width:100%}
.switch{position:relative;display:inline-block;width:60px;height:34px}
.switch input{opacity:0;width:0;height:0}
.switch input:disabled+.slider{opacity:0.5;cursor:not-allowed}
.slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background:var(--text-secondary);border-radius:34px}
.slider:before{position:absolute;content:"";height:26px;width:26px;left:4px;bottom:4px;background:white;transition:.4s;border-radius:50%}
input:checked+.slider{background:var(--accent)}
input:checked+.slider:before{transform:translateX(26px)}
</style>
</head>
<body>
<script>
if(localStorage.getItem('theme')==='light'){document.body.classList.add('light');}
document.body.style.visibility='visible';
// Polyfill for deprecated setCapture
if(!Element.prototype.setPointerCapture && Element.prototype.setCapture){
  Element.prototype.setPointerCapture = Element.prototype.setCapture;
}
</script>
<div class="header">
<h1>lux2g0</h1>
<div class="header-buttons">
<button class="icon-button" onclick="location.href='/'">
<svg viewBox="0 0 24 24"><path d="M10 20v-6h4v6h5v-8h3L12 3 2 12h3v8z"/></svg>
</button>
<button class="icon-button" onclick="location.href='/settings'">
<svg viewBox="0 0 24 24"><path d="M12 15.5A3.5 3.5 0 0 1 8.5 12A3.5 3.5 0 0 1 12 8.5a3.5 3.5 0 0 1 3.5 3.5a3.5 3.5 0 0 1-3.5 3.5m7.43-2.53c.04-.32.07-.64.07-.97c0-.33-.03-.65-.07-.97l2.11-1.63c.19-.15.24-.42.12-.64l-2-3.46c-.12-.22-.39-.3-.61-.22l-2.49 1c-.52-.39-1.06-.73-1.69-.98l-.37-2.65A.506.506 0 0 0 14 2h-4c-.25 0-.46.18-.5.42l-.37 2.65c-.63.25-1.17.59-1.69.98l-2.49-1c-.22-.08-.49 0-.61.22l-2 3.46c-.13.22-.07.49.12.64L4.57 11c-.04.32-.07.64-.07.97c0 .33.03.65.07.97l-2.11 1.63c-.19.15-.24.42-.12.64l2 3.46c.12.22.39.3.61.22l2.49-1c.52.39 1.06.73 1.69.98l.37 2.65c.04.24.25.42.5.42h4c.25 0 .46-.18.5-.42l.37-2.65c.63-.25 1.17-.59 1.69-.98l2.49 1c.22.08.49 0 .61-.22l2-3.46c.13-.22.07-.49-.12-.64l-2.11-1.63Z"/></svg>
</button>
<button class="icon-button" onclick="location.href='/about'">
<svg viewBox="0 0 24 24"><path d="M13,9H11V7H13M13,17H11V11H13M12,2A10,10 0 0,0 2,12A10,10 0 0,0 12,22A10,10 0 0,0 22,12A10,10 0 0,0 12,2Z"/></svg>
</button>
</div>
</div>
<div class="container">
<div class="section">
<h2>Saved Networks</h2>
<div id="saved-networks" style="margin-bottom:15px">
<div style="text-align:center;color:var(--text-secondary);padding:20px">Loading...</div>
</div>
</div>
<div class="section">
<h2 onclick="toggleWifiConfig()" style="cursor:pointer;display:flex;align-items:center;justify-content:space-between">
WiFi Configuration 
<span id="wifi-arrow" style="transition:transform 0.3s ease">▼</span>
</h2>
<form id="wifi-form">
<div id="wifi-config-content" style="display:none">
<div style="position:relative">
<input type="text" id="ssid" placeholder="Network name (SSID)" class="input-field" style="width:100%;margin-bottom:10px">
<div id="networks-dropdown" style="position:absolute;top:100%;left:0;right:0;background:var(--bg-tertiary);border:1px solid var(--border);border-radius:5px;max-height:400px;overflow-y:auto;display:none;z-index:100"></div>
</div>
<input type="password" id="password" placeholder="Password" class="input-field" style="width:100%;margin-bottom:10px">
<div class="theme-toggle" style="margin-bottom:10px">
<span style="flex:1;color:var(--text-primary)">Use Static IP</span>
<label class="switch">
<input type="checkbox" id="use-static" onchange="toggleStatic()">
<span class="slider"></span>
</label>
</div>
<div id="static-config" style="display:none;margin-top:10px">
<input type="text" id="static-ip" placeholder="IP Address" class="input-field" style="width:100%;margin-bottom:10px">
<input type="text" id="gateway" placeholder="Gateway" class="input-field" style="width:100%;margin-bottom:10px">
<input type="text" id="subnet" placeholder="Subnet" class="input-field" style="width:100%;margin-bottom:10px">
</div>
<div style="display:flex;gap:15px;justify-content:center;align-items:center;margin-top:20px;flex-wrap:wrap">
<button type="button" class="button" onclick="scanNetworks()" style="background:#B84444">Scan</button>
<button type="submit" class="button" style="background:#4CAF50">Save</button>
</div>
</div>
</form>
</div>
<div class="section">
<h2>Firmware Update (OTA)</h2>
<p style="color:var(--text-secondary);margin-bottom:15px">Upload a new firmware (.bin) file to update the device wirelessly.</p>
<div style="margin-bottom:15px">
<input type="file" id="firmware-file" accept=".bin" style="display:none" onchange="updateFileName()">
<div style="padding:12px;background:var(--bg-tertiary);border:1px solid var(--border);border-radius:5px;margin-bottom:20px;cursor:pointer;display:flex;align-items:center;justify-content:space-between" onclick="document.getElementById('firmware-file').click()">
<span id="file-name" style="color:var(--text-secondary)">No file selected</span>
<span style="padding:8px 16px;background:#B84444;color:white;border-radius:4px;font-size:14px;font-weight:500">Browse</span>
</div>
<div id="upload-progress" style="display:none;margin:15px 0">
<div style="position:relative;background:var(--bg-tertiary);border:2px solid var(--border);border-radius:8px;height:40px;overflow:hidden">
<div id="progress-bar" style="height:100%;background:#B84444;width:0%;transition:width 0.5s ease"></div>
<div id="progress-text" style="position:absolute;top:50%;left:50%;transform:translate(-50%,-50%);font-size:16px;font-weight:600;color:white;text-shadow:0 1px 2px rgba(0,0,0,0.5);z-index:10">0%</div>
</div>
<div id="countdown-text" style="text-align:center;margin-top:10px;font-size:14px;color:var(--text-secondary);display:none"></div>
</div>
<div style="display:flex;gap:15px;justify-content:center;flex-wrap:wrap">
<button type="button" class="button" onclick="uploadFirmware()" style="background:#4CAF50;min-width:180px;height:36px;padding:8px;font-size:14px">
<span style="display:flex;align-items:center;gap:8px">
<svg width="16" height="16" viewBox="0 0 24 24" fill="white">
<path d="M9 16.17L4.83 12l-1.42 1.41L9 19 21 7l-1.41-1.41L9 16.17z"/>
</svg>
Upload Firmware
</span>
</button>
</div>
<div style="display:flex;align-items:center;gap:10px;padding:12px;background:rgba(255,152,0,0.1);border:1px solid #FF9800;border-radius:5px;margin-top:25px">
<svg width="24" height="24" viewBox="0 0 24 24" fill="#FF9800">
<path d="M1 21L12 2l11 19H1zm11-3h2v-2h-2v2zm0-4h2v-4h-2v4z"/>
</svg>
<p style="color:#FF9800;margin:0;font-size:14px;font-weight:500">Warning: Only upload compiled .bin firmware files for ESP32. Uploading incorrect files may brick your device!</p>
</div>
</div>
</div>
<div class="section">
<h2>Interface Color</h2>
<div class="theme-toggle">
<span style="flex:1;color:var(--text-primary)">Dark Mode</span>
<label class="switch">
<input type="checkbox" id="theme-switch" onchange="toggleTheme()">
<span class="slider"></span>
</label>
</div>
</div>
</div>
<script>
// Initialize theme switch state
document.addEventListener('DOMContentLoaded',function(){
  const isDark=!document.body.classList.contains('light');
  document.getElementById('theme-switch').checked=isDark;
  loadSavedNetworks(); // Load saved networks on page load
});

// Load saved networks
async function loadSavedNetworks(){
  try{
    const r=await fetch('/api/wifi/saved');
    const d=await r.json();
    const container=document.getElementById('saved-networks');
    
    if(d.networks && d.networks.length>0){
      let html='';
      d.networks.forEach(n=>{
        html+=`<div class="network-item">
          <div style="flex:1">
            <div style="font-weight:500">${n.ssid}</div>
            <div style="font-size:12px;color:var(--text-secondary)">
              ${n.hasPassword?'Protected':'Open'}
              ${n.useStaticIP?' • Static IP':''}
            </div>
          </div>
          <div style="display:flex;gap:10px">
            <button class="button" onclick="useNetworkAutoConnect('${n.ssid}')" 
                    style="background:#4CAF50;height:36px;padding:8px 16px;font-size:14px;min-width:70px">Use</button>
            <button class="button" onclick="deleteNetwork('${n.ssid}')" 
                    style="background:#B84444;height:36px;padding:8px 16px;font-size:14px;min-width:70px">Del</button>
          </div>
        </div>`;
      });
      container.innerHTML=html;
    }else{
      container.innerHTML='<div style="text-align:center;color:var(--text-secondary);padding:20px">No saved networks</div>';
    }
  }catch(e){
    console.error('Error:',e);
  }
}

// Use saved network and automatically connect
async function useNetworkAutoConnect(ssid){
  try{
    // Show loading
    const btn = event.target;
    const originalText = btn.textContent;
    btn.textContent = '⏳';
    btn.disabled = true;
    
    // Load and connect directly - server handles everything
    const loadResp = await fetch('/api/wifi/load',{
      method:'POST',
      headers:{'Content-Type':'application/json'},
      body:JSON.stringify({ssid:ssid})
    });
    
    if(!loadResp.ok){
      throw new Error('Failed to load network');
    }
    
    const config = await loadResp.json();
    
    // Successfully loaded and connecting
    btn.textContent = '✓';
    
    // Show status and wait for connection
    alert(`Connecting to ${ssid}...\nPlease wait 10 seconds.`);
    
    // Wait longer before reload to allow connection
    setTimeout(()=>{
      location.reload();
    }, 10000);
    
  }catch(e){
    console.error('Error:',e);
    const btn = event.target;
    btn.textContent = 'Use';
    btn.disabled = false;
    alert('Error: ' + e.message);
  }
}

// Original useNetwork for manual load
async function useNetwork(ssid){
  try{
    const r=await fetch('/api/wifi/load',{
      method:'POST',
      headers:{'Content-Type':'application/json'},
      body:JSON.stringify({ssid:ssid})
    });
    if(r.ok){
      const d=await r.json();
      document.getElementById('ssid').value=d.ssid;
      document.getElementById('use-static').checked=d.useStaticIP;
      if(d.useStaticIP){
        document.getElementById('static-ip').value=d.ip||'';
        document.getElementById('gateway').value=d.gateway||'';
        document.getElementById('subnet').value=d.subnet||'';
        toggleStatic();
      }
      alert('Network loaded! Click Save to connect.');
    }
  }catch(e){
    alert('Error: '+e.message);
  }
}

// Delete network
async function deleteNetwork(ssid){
  if(!confirm('Delete '+ssid+'?')) return;
  try{
    const r=await fetch('/api/wifi/delete',{
      method:'POST',
      headers:{'Content-Type':'application/json'},
      body:JSON.stringify({ssid:ssid})
    });
    if(r.ok){
      loadSavedNetworks();
    }
  }catch(e){
    alert('Error: '+e.message);
  }
}
function toggleTheme(){
  document.body.classList.toggle('light');
  localStorage.setItem('theme',document.body.classList.contains('light')?'light':'dark');
}
function toggleStatic(){
  document.getElementById('static-config').style.display=
    document.getElementById('use-static').checked?'block':'none';
}
async function scanNetworks(){
  const btn = event.target;
  const originalText = btn.textContent;
  const ssidField = document.getElementById('ssid');
  const originalSSID = ssidField.value;
  
  // Change to shorter text
  btn.textContent = 'Wait...';
  btn.style.cursor = 'wait';
  btn.disabled = true;
  ssidField.value = 'Searching for networks...';
  ssidField.disabled = true;
  
  const dropdown = document.getElementById('networks-dropdown');
  dropdown.style.display = 'none';
  
  try{
    const r=await fetch('/api/scan');
    if(r.ok){
      const d=await r.json();
      if(d.networks && d.networks.length > 0){
        let html='';
        d.networks.forEach(n=>{
          // Check if it might be an iPhone (common patterns)
          const isIPhone = n.ssid && (n.ssid.includes('iPhone') || n.ssid.includes('iPad') || 
                                      n.ssid.includes("'s ") || n.ssid.includes("'s "));
          const icon = isIPhone ? '📱 ' : '';
          
          html+=`<div style="padding:10px;cursor:pointer;border-bottom:1px solid var(--border)" 
                 onmouseover="this.style.background='var(--bg-secondary)'" 
                 onmouseout="this.style.background='transparent'"
                 onclick="selectNetwork('${n.ssid}')">
            ${icon}${n.ssid} 
            <span style="color:var(--text-secondary);font-size:12px">
              (Ch:${n.channel}, ${n.rssi}dBm${n.secured?' 🔒':''})
            </span>
          </div>`;
        });
        dropdown.innerHTML=html;
        dropdown.style.display = 'block';
        ssidField.value = originalSSID;
      } else {
        ssidField.value = 'No networks found';
        setTimeout(()=>{ssidField.value = originalSSID;}, 2000);
      }
    }
  }catch(e){
    ssidField.value = 'Error scanning networks';
    setTimeout(()=>{ssidField.value = originalSSID;}, 2000);
  }finally{
    // Restore button state
    btn.textContent = originalText;
    btn.style.cursor = 'pointer';
    btn.disabled = false;
    ssidField.disabled = false;
  }
}
function selectNetwork(ssid){
  document.getElementById('ssid').value=ssid;
  document.getElementById('networks-dropdown').style.display='none';
  document.getElementById('password').focus();
}
// Close dropdown when clicking outside
document.addEventListener('click', function(e){
  if(!e.target.closest('#ssid') && !e.target.closest('#networks-dropdown') && !e.target.closest('button[onclick*="scanNetworks"]')){
    document.getElementById('networks-dropdown').style.display='none';
  }
});
document.getElementById('wifi-form').onsubmit=async(e)=>{
  e.preventDefault();
  const submitBtn = e.target.querySelector('button[type="submit"]');
  const originalText = submitBtn.textContent;
  submitBtn.textContent = 'Wait...';
  submitBtn.disabled = true;
  
  const config={
    ssid:document.getElementById('ssid').value,
    password:document.getElementById('password').value,
    staticIP:document.getElementById('use-static').checked
  };
  if(config.staticIP){
    config.ip=document.getElementById('static-ip').value;
    config.gateway=document.getElementById('gateway').value;
    config.subnet=document.getElementById('subnet').value;
  }
  
  try{
    const r=await fetch('/api/wifi/config',{
      method:'POST',
      headers:{'Content-Type':'application/json'},
      body:JSON.stringify(config)
    });
    if(r.ok){
      // Show success message
      const msgDiv = document.createElement('div');
      msgDiv.style.cssText = 'position:fixed;top:80px;left:50%;transform:translateX(-50%);background:var(--success);color:white;padding:15px 30px;border-radius:8px;z-index:2000;box-shadow:0 4px 12px rgba(0,0,0,0.3)';
      msgDiv.textContent = 'Configuration saved! Redirecting to home...';
      document.body.appendChild(msgDiv);
      
      // Redirect to home quickly
      setTimeout(()=>{
        window.location.href = '/';
      }, 500);
    } else {
      alert('Error saving configuration');
      submitBtn.textContent = originalText;
      submitBtn.disabled = false;
    }
  }catch(e){
    alert('Error: ' + e.message);
    submitBtn.textContent = originalText;
    submitBtn.disabled = false;
  }
};

// Update file name display
function updateFileName() {
  const fileInput = document.getElementById('firmware-file');
  const fileNameSpan = document.getElementById('file-name');
  
  if (fileInput.files && fileInput.files[0]) {
    const fileName = fileInput.files[0].name;
    fileNameSpan.textContent = fileName;
    fileNameSpan.style.color = 'var(--text-primary)';
  } else {
    fileNameSpan.textContent = 'No file selected';
    fileNameSpan.style.color = 'var(--text-secondary)';
  }
}

// Toggle WiFi Configuration visibility
function toggleWifiConfig() {
  const content = document.getElementById('wifi-config-content');
  const arrow = document.getElementById('wifi-arrow');
  
  if (content.style.display === 'none') {
    content.style.display = 'block';
    arrow.style.transform = 'rotate(180deg)';
  } else {
    content.style.display = 'none';
    arrow.style.transform = 'rotate(0deg)';
  }
}

// OTA Firmware Upload Function
async function uploadFirmware() {
  const fileInput = document.getElementById('firmware-file');
  const progressDiv = document.getElementById('upload-progress');
  const progressBar = document.getElementById('progress-bar');
  const progressText = document.getElementById('progress-text');
  const countdownText = document.getElementById('countdown-text');
  
  if (!fileInput.files[0]) {
    alert('Please select a firmware file first');
    return;
  }
  
  const file = fileInput.files[0];
  if (!file.name.endsWith('.bin')) {
    alert('Please select a valid .bin firmware file');
    return;
  }
  
  if (!confirm('Are you sure you want to upload this firmware? The device will restart after upload.')) {
    return;
  }
  
  const formData = new FormData();
  formData.append('firmware', file);
  
  progressDiv.style.display = 'block';
  progressBar.style.width = '0%';
  progressText.textContent = '0%';
  countdownText.style.display = 'none';
  
  // Simulate progress (since actual upload progress isn't available)
  let progress = 0;
  const progressInterval = setInterval(() => {
    if (progress < 90) {
      progress += Math.random() * 15;
      if (progress > 90) progress = 90;
      progressBar.style.width = progress + '%';
      progressText.textContent = Math.floor(progress) + '%';
    }
  }, 200);
  
  try {
    const response = await fetch('/api/ota/upload', {
      method: 'POST',
      body: formData
    });
    
    clearInterval(progressInterval);
    
    if (response.ok) {
      progressBar.style.width = '100%';
      progressText.textContent = '100%';
      
      // Start countdown
      countdownText.style.display = 'block';
      let countdown = 10;
      
      const countdownInterval = setInterval(() => {
        countdownText.textContent = `Device restarting... Page will refresh in ${countdown} seconds`;
        countdown--;
        
        if (countdown < 0) {
          clearInterval(countdownInterval);
          location.reload();
        }
      }, 1000);
      
      countdownText.textContent = `Device restarting... Page will refresh in ${countdown} seconds`;
      
    } else {
      throw new Error('Upload failed');
    }
  } catch (error) {
    clearInterval(progressInterval);
    progressText.textContent = 'FAILED';
    progressBar.style.background = '#B84444';
    countdownText.style.display = 'block';
    countdownText.style.color = '#B84444';
    countdownText.textContent = 'Upload failed: ' + error.message;
    
    setTimeout(() => {
      progressDiv.style.display = 'none';
      progressBar.style.background = '#B84444';
      countdownText.style.display = 'none';
      countdownText.style.color = 'var(--text-secondary)';
    }, 5000);
  }
}

if(localStorage.getItem('theme')==='light'){
  document.body.classList.add('light');
}
</script>
</body>
</html>)rawliteral";
}

String getAboutHTML() {
  return R"rawliteral(<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>lux2g0</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
:root{--bg-primary:#0f0f14;--bg-secondary:#1a1a24;--bg-tertiary:#252533;--text-primary:#ffffff;--text-secondary:#b0b0b0;--accent:#B84444;--info:#03a9f4;--border:#2a2a3e;--header-bg:#B84444}
body.light{--bg-primary:#f5f5f5;--bg-secondary:#fff;--bg-tertiary:#e0e0e0;--text-primary:#333;--text-secondary:#666;--accent:#2196F3;--info:#03a9f4;--border:#ddd;--header-bg:#2196F3}
body{font-family:Arial,sans-serif;background:var(--bg-primary);color:var(--text-primary);margin:0;padding-top:60px;visibility:hidden}
.header{position:fixed;top:0;left:0;right:0;height:60px;background:var(--header-bg);display:flex;align-items:center;justify-content:space-between;padding:0 20px;box-shadow:0 2px 10px rgba(0,0,0,0.2);z-index:100}
.header h1{color:#fff;font-size:24px;font-weight:600}
.header-buttons{display:flex;gap:10px;align-items:center}
.icon-button{background:rgba(255,255,255,0.2);border:none;width:40px;height:40px;border-radius:50%;cursor:pointer;display:flex;align-items:center;justify-content:center;transition:all 0.3s ease}
.icon-button:hover{background:rgba(255,255,255,0.3);transform:scale(1.05)}
.icon-button svg{width:24px;height:24px;fill:white}
.container{padding:20px}
.section{background:var(--bg-secondary);border-radius:8px;padding:20px;margin-bottom:20px}
.section h2{color:var(--accent);margin-bottom:15px}
.info-row{display:flex;justify-content:space-between;padding:10px 0;border-bottom:1px solid var(--border)}
.info-row:last-child{border-bottom:none}
a{color:var(--info);text-decoration:none}
</style>
</head>
<body>
<script>
if(localStorage.getItem('theme')==='light'){document.body.classList.add('light');}
document.body.style.visibility='visible';
// Polyfill for deprecated setCapture
if(!Element.prototype.setPointerCapture && Element.prototype.setCapture){
  Element.prototype.setPointerCapture = Element.prototype.setCapture;
}
</script>
<div class="header">
<h1>lux2g0</h1>
<div class="header-buttons">
<button class="icon-button" onclick="location.href='/'">
<svg viewBox="0 0 24 24"><path d="M10 20v-6h4v6h5v-8h3L12 3 2 12h3v8z"/></svg>
</button>
<button class="icon-button" onclick="location.href='/settings'">
<svg viewBox="0 0 24 24"><path d="M12 15.5A3.5 3.5 0 0 1 8.5 12A3.5 3.5 0 0 1 12 8.5a3.5 3.5 0 0 1 3.5 3.5a3.5 3.5 0 0 1-3.5 3.5m7.43-2.53c.04-.32.07-.64.07-.97c0-.33-.03-.65-.07-.97l2.11-1.63c.19-.15.24-.42.12-.64l-2-3.46c-.12-.22-.39-.3-.61-.22l-2.49 1c-.52-.39-1.06-.73-1.69-.98l-.37-2.65A.506.506 0 0 0 14 2h-4c-.25 0-.46.18-.5.42l-.37 2.65c-.63.25-1.17.59-1.69.98l-2.49-1c-.22-.08-.49 0-.61.22l-2 3.46c-.13.22-.07.49.12.64L4.57 11c-.04.32-.07.64-.07.97c0 .33.03.65.07.97l-2.11 1.63c-.19.15-.24.42-.12.64l2 3.46c.12.22.39.3.61.22l2.49-1c.52.39 1.06.73 1.69.98l.37 2.65c.04.24.25.42.5.42h4c.25 0 .46-.18.5-.42l.37-2.65c.63-.25 1.17-.59 1.69-.98l2.49 1c.22.08.49 0 .61-.22l2-3.46c.13-.22.07-.49-.12-.64l-2.11-1.63Z"/></svg>
</button>
<button class="icon-button" onclick="location.href='/about'">
<svg viewBox="0 0 24 24"><path d="M13,9H11V7H13M13,17H11V11H13M12,2A10,10 0 0,0 2,12A10,10 0 0,0 12,22A10,10 0 0,0 22,12A10,10 0 0,0 12,2Z"/></svg>
</button>
</div>
</div>
<div class="container">
<div class="section">
<h2>About</h2>
<div class="info-row">
<span style="color:var(--text-primary)">Version</span>
<span>1.2.0</span>
</div>
<div class="info-row">
<span style="color:var(--text-primary)">Author</span>
<span><a href="mailto:marcuzz0@example.com">marcuzz0</a></span>
</div>
<div class="info-row">
<span style="color:var(--text-primary)">GitHub</span>
<span><a href="https://github.com/marcuzz0/lux2g0" target="_blank">lux2g0</a></span>
</div>
<div class="info-row">
<span style="color:var(--text-primary)">License</span>
<span><a href="https://www.gnu.org/licenses/gpl-3.0.html" target="_blank">GNU GPL v3.0</a></span>
</div>
</div>
<div class="section">
<h2>Features</h2>
<ul style="margin:0;padding-left:0;list-style:none">
<li style="padding:5px 0;color:var(--text-primary)">• Single relay control optimized</li>
<li style="padding:5px 0;color:var(--text-primary)">• OTA firmware update via web interface</li>
<li style="padding:5px 0;color:var(--text-primary)">• WiFi dual mode (AP + STA simultaneously)</li>
<li style="padding:5px 0;color:var(--text-primary)">• Light sensor automation (LDR)</li>
<li style="padding:5px 0;color:var(--text-primary)">• Time-based scheduling (NTP sync)</li>
<li style="padding:5px 0;color:var(--text-primary)">• OLED display 1.3" with animations</li>
<li style="padding:5px 0;color:var(--text-primary)">• Dark/Light theme with persistence</li>
<li style="padding:5px 0;color:var(--text-primary)">• Collapsible sections interface</li>
<li style="padding:5px 0;color:var(--text-primary)">• RESTful API endpoints</li>
<li style="padding:5px 0;color:var(--text-primary)">• mDNS support (lux2g0.local)</li>
</ul>
</div>
</div>
<script>
if(localStorage.getItem('theme')==='light'){
  document.body.classList.add('light');
}
</script>
</body>
</html>)rawliteral";
}