/*
 * Wortuhr Compact Card
 * - Compact version of Wortuhr control card
 * - Uses same entity mapping as full version
 * - Tab-based layout for space efficiency
 * - Full functionality in minimal space
 */

const LitElementBase = window.LitElement || Object.getPrototypeOf(customElements.get('ha-panel-lovelace'));
const html = (LitElementBase && LitElementBase.prototype.html) || window.html;
const css = (LitElementBase && LitElementBase.prototype.css) || window.css;

if (!LitElementBase || !html || !css) {
  throw new Error('LitElement oder lit-html nicht gefunden. Bitte Home Assistant neu laden.');
}

class WortuhrCardCompact extends LitElementBase {
  static get properties() {
    return {
      hass: {},
      config: {},
      _activeTab: { state: true },
    };
  }

  constructor() {
    super();
    this._activeTab = 'effects';
  }

  static getStubConfig() {
    return { device: 'wortuhr', title: 'Wortuhr' };
  }

  static getConfigElement() {
    return document.createElement('wortuhr-card-editor');
  }

  static get styles() {
    return css`
      :host {
        display: block;
      }
      ha-card {
        box-shadow: none;
        background: none;
        padding: 0;
        border: none;
      }
      .card {
        width: 100%;
        max-width: 100%;
        padding: 5px;
        border: 2px solid var(--mycardbackgroundcolor-1);
        border-radius: var(--Borderradius);
        background-image: linear-gradient(43deg, var(--mycardbackgroundcolor-1) 0%, var(--mycardbackgroundcolor-2, var(--Kartenhintergrund)) 31%, var(--mycardbackgroundcolor-3));
        margin: auto;
        display: flex;
        flex-direction: column;
        box-sizing: border-box;
      }
      .header {
        display: flex;
        align-items: center;
        justify-content: space-between;
        gap: 8px;
        margin-bottom: 10px;
        flex-wrap: wrap;
      }
      .title {
        font-size: clamp(13px, 2.5vw, 17px);
        font-weight: bold;
        color: var(--Textfarbe, black);
      }
      .controls-row {
        display: flex;
        gap: 6px;
        align-items: center;
        flex: 1;
        min-width: 200px;
      }
      .brightness-control {
        display: flex;
        align-items: center;
        gap: 4px;
        flex: 1;
        min-width: 120px;
      }
      .brightness-control input {
        flex: 1;
        min-width: 60px;
      }
      .brightness-label {
        font-size: clamp(10px, 1.5vw, 12px);
        color: var(--secondary-text-color, black);
        min-width: 30px;
        text-align: right;
      }
      button {
        background-color: var(--primary-color, white);
        color: var(--Textfarbe, black);
        border: none;
        border-radius: 15px;
        padding: 6px 12px;
        font-weight: 600;
        font-size: clamp(10px, 1.5vw, 12px);
        cursor: pointer;
        transition: transform 0.2s ease;
        white-space: nowrap;
      }
      button:hover {
        transform: scale(1.1);
      }
      .tabs {
        display: flex;
        gap: 4px;
        margin-bottom: 10px;
        border-bottom: 1px solid var(--divider-color, #e0e0e0);
      }
      .tab {
        padding: 6px 10px;
        font-size: clamp(9px, 1.3vw, 11px);
        cursor: pointer;
        background: transparent;
        border: none;
        color: var(--secondary-text-color, black);
        border-bottom: 2px solid transparent;
        transition: all 0.2s;
        box-shadow: none;
      }
      .tab:hover {
        color: var(--Textfarbe, black);
      }
      .tab.active {
        color: var(--accent-color);
        border-bottom-color: var(--accent-color);
      }
      .tab-content {
        display: none;
      }
      .tab-content.active {
        display: block;
      }
      .row {
        display: flex;
        align-items: center;
        gap: 6px;
        margin: 6px 0;
        font-size: clamp(10px, 1.5vw, 12px);
      }
      .label {
        font-size: clamp(9px, 1.3vw, 11px);
        color: var(--Textfarbe, black);
        min-width: 70px;
        flex-shrink: 0;
      }
      select, input[type="color"], input[type="range"] {
        flex: 1;
        background: rgba(255, 255, 255, 0.1);
        border: 1px solid var(--divider-color, #e0e0e0);
        color: var(--Textfarbe, black);
        border-radius: 6px;
        padding: 5px 6px;
        box-sizing: border-box;
        font-size: clamp(9px, 1.3vw, 11px);
        transition: all 0.2s ease;
      }
      select option {
        background: var(--primary-color, white);
        color: var(--Textfarbe, black);
      }
      select:hover, input[type="color"]:hover, input[type="range"]:hover {
        background: rgba(255, 255, 255, 0.15);
        border-color: var(--accent-color);
      }
      select:focus, input[type="color"]:focus, input[type="range"]:focus {
        outline: none;
        border-color: var(--accent-color);
        box-shadow: 0 0 8px rgba(var(--accent-color-rgb, 52, 211, 153), 0.15);
      }
      input[type="color"] {
        width: 40px;
        height: 32px;
        cursor: pointer;
        padding: 3px;
      }
      input[type="range"] {
        padding: 0;
        cursor: pointer;
      }
      .color-row {
        display: flex;
        align-items: center;
        gap: 6px;
        margin: 6px 0;
      }
      .color-display {
        width: 24px;
        height: 24px;
        border-radius: 4px;
        border: 1px solid var(--divider-color, #e0e0e0);
        flex-shrink: 0;
      }
      .missing {
        color: var(--warning-color, #fbbf24);
        font-size: clamp(9px, 1.3vw, 11px);
      }
    `;
  }

  setConfig(config) {
    if (!config || (!config.device && !config.entities)) {
      throw new Error('Bitte device angeben (MQTT-Prefix/Device-ID).');
    }
    this.config = config;
  }

  getCardSize() {
    return 3;
  }

  render() {
    if (!this.hass || !this.config) return html``;
    const device = (this.config.device || '').trim();
    const entities = this._entityMap(device);
    const powerState = this._state(entities.power);
    
    const v1State = this._state(entities.v1);
    const brightness = v1State?.attributes?.brightness ?? 255;

    return html`
      <ha-card>
        <div class="card">
          <div class="header">
            <div class="title">${this.config.title || 'Wortuhr'}</div>
            <div class="controls-row">
              <div class="brightness-control">
                <input
                  type="range"
                  min="1"
                  max="255"
                  .value=${brightness}
                  @change=${(ev) => this._setBrightnessAll(entities, Number(ev.target.value))}
                />
                <span class="brightness-label">${Math.round((brightness / 255) * 100)}%</span>
              </div>
              ${powerState
                ? html`<button @click=${() => this._togglePower(entities.power)}>
                    ${powerState.state === 'on' ? 'Aus' : 'An'}
                  </button>`
                : html`<span class="missing">Fehler</span>`}
            </div>
          </div>

          <div class="tabs">
            <button class="tab ${this._activeTab === 'effects' ? 'active' : ''}" @click=${() => this._activeTab = 'effects'}>
              Effekte
            </button>
            <button class="tab ${this._activeTab === 'animation' ? 'active' : ''}" @click=${() => this._activeTab = 'animation'}>
              Animation
            </button>
            <button class="tab ${this._activeTab === 'vordergrund' ? 'active' : ''}" @click=${() => this._activeTab = 'vordergrund'}>
              Vordergrund
            </button>
            <button class="tab ${this._activeTab === 'hintergrund' ? 'active' : ''}" @click=${() => this._activeTab = 'hintergrund'}>
              Hintergrund
            </button>
            <button class="tab ${this._activeTab === 'status' ? 'active' : ''}" @click=${() => this._activeTab = 'status'}>
              Status
            </button>
          </div>

          <div class="tab-content ${this._activeTab === 'effects' ? 'active' : ''}">
            ${this._selectRowCompact('Übergang', entities.effect, this._effectOptions())}
            ${this._selectRowCompact('Tempo', entities.effectTime, this._speedOptions())}
          </div>

          <div class="tab-content ${this._activeTab === 'animation' ? 'active' : ''}">
            ${this._selectRowCompact('Animation', entities.animation, this._animationOptions())}
            ${this._selectRowCompact('Tempo', entities.animationTime, this._speedOptions())}
            ${this._selectRowCompact('Stärke', entities.animationDepth, this._depthOptions())}
          </div>

          <div class="tab-content ${this._activeTab === 'vordergrund' ? 'active' : ''}">
            <div style="font-size: clamp(9px, 1.3vw, 11px); color: var(--secondary-text-color, black); margin-bottom: 6px;">Farben:</div>
            ${this._colorRowCompact('Farbe 1', entities.v1)}
            ${this._colorRowCompact('Farbe 2', entities.v2)}
            ${this._selectRowCompact('Schema', entities.vs, this._schemaOptions())}
          </div>

          <div class="tab-content ${this._activeTab === 'hintergrund' ? 'active' : ''}">
            <div style="font-size: clamp(9px, 1.3vw, 11px); color: var(--secondary-text-color, black); margin-bottom: 6px;">Farben:</div>
            ${this._colorRowCompact('Farbe 1', entities.h1)}
            ${this._colorRowCompact('Farbe 2', entities.h2)}
            ${this._selectRowCompact('Schema', entities.hs, this._schemaOptions())}
          </div>

          <div class="tab-content ${this._activeTab === 'status' ? 'active' : ''}">
            <div style="font-size: clamp(9px, 1.3vw, 11px); color: var(--secondary-text-color, black); margin-bottom: 6px;">Diagnose-Informationen:</div>
            ${this._statusRow('IP-Adresse', entities.ipAddress)}
            ${this._statusRow('WiFi-Signalstärke', entities.rssi, 'dBm')}
            ${this._statusRow('System Auslastung', entities.systemLoad, '%')}
            ${this._formatUptimeRow('Uptime', entities.uptime)}
            ${this._statusRow('Letzter NTP Sync', entities.lastNtpSync)}
          </div>
        </div>
      </ha-card>
    `;
  }

  _entityMap(device) {
    const base = this._norm(device || '');
    const overrides = this.config.entities || {};
    const suffixes = {
      power: 'power',
      effect: 'efx',
      effectTime: 'efxtime',
      animation: 'ani',
      animationTime: 'anitime',
      animationDepth: 'anidepth',
      v1: 'v1',
      v2: 'v2',
      h1: 'h1',
      h2: 'h2',
      vs: 'vs',
      hs: 'hs',
      ipAddress: 'ip_address',
      uptime: 'uptime',
      rssi: 'rssi',
      systemLoad: 'system_load',
      lastNtpSync: 'last_ntp_sync',
    };

    const resolved = {};
    Object.keys(suffixes).forEach((key) => {
      if (overrides[key]) {
        resolved[key] = overrides[key];
        return;
      }
      const suffix = suffixes[key];
      resolved[key] =
        this._findEntityByUnique(base, suffix) ||
        this._findEntityBySuffix(base, suffix);
    });
    return resolved;
  }

  _findEntityByUnique(base, suffix) {
    if (!this.hass || !this.hass.states || !base) return undefined;
    const normBase = this._norm(base);
    
    for (const [eid, st] of Object.entries(this.hass.states)) {
      const uid = st?.attributes?.unique_id;
      if (!uid) continue;
      
      const normUid = this._norm(uid);
      const normSuffix = this._norm(suffix);
      
      if (normUid === `${normBase}_${normSuffix}`) return eid;
      if (normUid === `${normBase}_wortuhr_${normSuffix}`) return eid;
      if (normUid.endsWith(`_${normSuffix}`) && normUid.includes(normBase)) return eid;
    }
    return undefined;
  }

  _findEntityBySuffix(base, suffix) {
    if (!this.hass || !this.hass.states) return undefined;
    const normBase = this._norm(base);
    const normSuffix = this._norm(suffix);
    
    const suffixMaps = {
      power: ['power', 'wortuhr_power'],
      efx: ['efx', 'ubergangseffekt', 'uebergangseffekt'],
      efxtime: ['efxtime', 'ubergangsgeschwindigkeit', 'uebergangsgeschwindigkeit', 'uebergansgeschwindigkeit'],
      ani: ['ani', 'animationseffekt', 'animationen'],
      anitime: ['anitime', 'animationsgeschwindigkeit', 'animations_geschwindigkeit', 'animationsgeschwindikkeit'],
      anidepth: ['anidepth', 'animationsstarke', 'animationstarke', 'animationsintenstat', 'animationsintensitat'],
      v1: ['v1', 'vordergrundfarbe_1', 'vordergrund_farbe_1'],
      v2: ['v2', 'vordergrundfarbe_2', 'vordergrund_farbe_2'],
      h1: ['h1', 'hintergrundfarbe_1', 'hintergrund_farbe_1'],
      h2: ['h2', 'hintergrundfarbe_2', 'hintergrund_farbe_2'],
      vs: ['vs', 'vordergrundfarbschema', 'vordergrund_farbschema'],
      hs: ['hs', 'hintergrundfarbschema', 'hintergrund_farbschema'],
      ip_address: ['ip_address', 'ip_adresse'],
      uptime: ['uptime'],
      rssi: ['rssi', 'wifi_signalstarke', 'wifi_signalstärke'],
      system_load: ['system_load', 'system_auslastung'],
      last_ntp_sync: ['last_ntp_sync', 'letzter_ntp_sync'],
    };
    
    const possibleSuffixes = suffixMaps[normSuffix] || [suffix];
    
    for (const [eid, st] of Object.entries(this.hass.states)) {
      const normEid = this._norm(eid);
      const uid = st?.attributes?.unique_id;
      const normUid = uid ? this._norm(uid) : '';
      
      for (const suff of possibleSuffixes) {
        const normSuff = this._norm(suff);
        
        if (normBase && normEid === `switch_${normBase}_${normSuff}`) return eid;
        if (normBase && normEid === `light_${normBase}_${normSuff}`) return eid;
        if (normBase && normEid === `select_${normBase}_${normSuff}`) return eid;
        if (normBase && normEid === `sensor_${normBase}_${normSuff}`) return eid;
        
        const eidWithoutDomain = normEid.split('_').slice(1).join('_');
        if (normBase && eidWithoutDomain === `${normBase}_${normSuff}`) return eid;
        if (normBase && eidWithoutDomain === `${normBase}_wortuhr_${normSuff}`) return eid;
        
        if (normBase && normUid === `${normBase}_${normSuff}`) return eid;
        if (normBase && normUid === `${normBase}_wortuhr_${normSuff}`) return eid;
        
        if (normBase && normUid.length > normBase.length) {
          const uIdAfterBase = normUid.substring(normBase.length);
          if (uIdAfterBase === `_${normSuff}` || uIdAfterBase === `_wortuhr_${normSuff}`) return eid;
        }
        
        if (normBase && normUid.endsWith(`_${normSuff}`) && normUid.includes(normBase)) return eid;
      }
    }
    return undefined;
  }

  _norm(str) {
    return (str || '')
      .toLowerCase()
      .replace(/[^a-z0-9]+/g, '_')
      .replace(/^_+|_+$/g, '');
  }

  _state(entityId) {
    return entityId ? this.hass.states[entityId] : undefined;
  }

  _selectRowCompact(label, entityId, fallbackOptions) {
    const stateObj = this._state(entityId);
    const options = stateObj?.attributes?.options || fallbackOptions;
    const currentValue = stateObj?.state || '';
    
    if (!stateObj || !options) {
      return html`<div class="row"><span class="missing">${label}: Entity fehlt</span></div>`;
    }
    
    return html`
      <div class="row">
        <span class="label">${label}</span>
        <select .value=${currentValue} @change=${(ev) => this._select(entityId, ev.target.value)}>
          ${options.map((opt) => html`<option value=${opt} ?selected=${opt === currentValue}>${opt}</option>`)}
        </select>
      </div>
    `;
  }

  _colorRowCompact(label, entityId) {
    const stateObj = this._state(entityId);
    if (!stateObj) return html`<div class="row"><span class="missing">${label}: fehlt</span></div>`;
    
    const rgb = stateObj.attributes?.rgb_color ?? [0, 0, 0];
    const hex = this._rgbToHex(rgb);
    const displayColor = `rgb(${rgb[0]}, ${rgb[1]}, ${rgb[2]})`;
    const brightness = stateObj.attributes?.brightness ?? 255;
    
    return html`
      <div class="color-row">
        <span class="label">${label}</span>
        <input
          type="color"
          .value=${hex}
          @change=${(ev) => this._setColor(entityId, ev.target.value, brightness)}
        />
        <div class="color-display" style="background: ${displayColor};"></div>
      </div>
    `;
  }

  _statusRow(label, entityId, unit = '') {
    const stateObj = this._state(entityId);
    if (!stateObj) return html`<div class="row"><span class="missing">${label}: fehlt</span></div>`;
    
    const value = stateObj.state;
    const displayValue = unit ? `${value} ${unit}` : value;
    
    return html`
      <div class="row">
        <span class="label">${label}</span>
        <span style="flex: 1; color: var(--secondary-text-color, black);">${displayValue}</span>
      </div>
    `;
  }

  _formatUptimeRow(label, entityId) {
    const stateObj = this._state(entityId);
    if (!stateObj) return html`<div class="row"><span class="missing">${label}: fehlt</span></div>`;
    
    const seconds = parseInt(stateObj.state, 10);
    if (isNaN(seconds)) {
      return html`
        <div class="row">
          <span class="label">${label}</span>
          <span style="flex: 1; color: var(--secondary-text-color, black);">${stateObj.state}</span>
        </div>
      `;
    }
    
    const days = Math.floor(seconds / 86400);
    const hours = Math.floor((seconds % 86400) / 3600);
    const minutes = Math.floor((seconds % 3600) / 60);
    
    let displayValue = '';
    if (days > 0) {
      displayValue = `${days}d ${hours}h ${minutes}m`;
    } else if (hours > 0) {
      displayValue = `${hours}h ${minutes}m`;
    } else {
      displayValue = `${minutes}m`;
    }
    
    return html`
      <div class="row">
        <span class="label">${label}</span>
        <span style="flex: 1; color: var(--secondary-text-color, black);">${displayValue}</span>
      </div>
    `;
  }

  _formatUptime(entityId) {
    const stateObj = this._state(entityId);
    if (!stateObj) return '';
    
    const seconds = parseInt(stateObj.state, 10);
    if (isNaN(seconds)) return stateObj.state;
    
    const days = Math.floor(seconds / 86400);
    const hours = Math.floor((seconds % 86400) / 3600);
    const minutes = Math.floor((seconds % 3600) / 60);
    
    if (days > 0) {
      return `${days}d ${hours}h ${minutes}m`;
    } else if (hours > 0) {
      return `${hours}h ${minutes}m`;
    } else {
      return `${minutes}m`;
    }
  }

  _togglePower(entityId) {
    if (!entityId) return;
    const stateObj = this._state(entityId);
    const turnOn = stateObj?.state !== 'on';
    this.hass.callService('switch', turnOn ? 'turn_on' : 'turn_off', { entity_id: entityId });
  }

  _select(entityId, option) {
    if (!entityId) return;
    this.hass.callService('select', 'select_option', { entity_id: entityId, option });
  }

  _setColor(entityId, hex, brightness) {
    if (!entityId) return;
    const rgb = this._hexToRgb(hex);
    this.hass.callService('light', 'turn_on', {
      entity_id: entityId,
      rgb_color: rgb,
      brightness,
    });
  }

  _setBrightnessAll(entities, brightness) {
    [entities.v1, entities.v2, entities.h1, entities.h2].forEach((entityId) => {
      if (!entityId) return;
      const stateObj = this._state(entityId);
      const rgb = stateObj?.attributes?.rgb_color ?? [0, 0, 0];
      this.hass.callService('light', 'turn_on', {
        entity_id: entityId,
        brightness,
        rgb_color: rgb,
      });
    });
  }

  _rgbToHex([r, g, b]) {
    return `#${[r, g, b]
      .map((v) => {
        const h = Number(v).toString(16).padStart(2, '0');
        return h.length > 2 ? 'ff' : h;
      })
      .join('')}`;
  }

  _hexToRgb(hex) {
    const val = hex.replace('#', '');
    return [
      parseInt(val.substring(0, 2), 16) || 0,
      parseInt(val.substring(2, 4), 16) || 0,
      parseInt(val.substring(4, 6), 16) || 0,
    ];
  }

  _effectOptions() {
    return ['kein Effekt', 'zufällig', 'Fade', 'Running', 'Schlange', 'Zeilen', 'Scrollen', 'Slide in', 'Diagonal', 'Rain', 'Spirale', 'Schlangenfresser', 'Raute', 'Feuerwerk'];
  }

  _animationOptions() {
    return ['keine Animation', 'Blinken', 'Vordergrundblinken', 'Pulsieren', 'Verlauf', 'Fliegen'];
  }

  _schemaOptions() {
    return ['einfarbig', 'Schachbrett', 'Spalten', 'Zeilen', 'Verlauf', 'Zufällig'];
  }

  _speedOptions() {
    return ['langsam', 'mittel', 'schnell'];
  }

  _depthOptions() {
    return ['schwach', 'mittel', 'stark'];
  }
}

customElements.define('wortuhr-card-compact', WortuhrCardCompact);
window.customCards = window.customCards || [];
window.customCards.push({
  type: 'wortuhr-card-compact',
  name: 'Wortuhr Compact',
  description: 'Kompakte Steuerung für die Wortuhr (MQTT Discovery)',
});

// Reuse the editor from wortuhr-card.js
class WortuhrCardEditor extends LitElementBase {
  static get properties() {
    return {
      hass: {},
      _config: {},
    };
  }

  setConfig(config) {
    this._config = { device: '', title: '', ...config };
  }

  render() {
    if (!this._config) return html``;
    const devices = this._deviceOptions();
    return html`
      <div class="card">
        <div class="row">
          <span class="label">Titel</span>
          <input
            type="text"
            .value=${this._config.title || ''}
            @input=${(ev) => this._onValue('title', ev)}
            placeholder="Wortuhr"
          />
        </div>
        <div class="row">
          <span class="label">Device (MQTT Prefix)</span>
          <select
            .value=${this._config.device || ''}
            @change=${(ev) => this._onSelectDevice(ev)}
          >
            <option value="">-- auswählen oder manuell --</option>
            ${devices.map((d) => html`<option value=${d}>${d}</option>`) }
          </select>
        </div>
        <div class="row">
          <span class="label">Manuell (falls nicht gefunden)</span>
          <input
            type="text"
            .value=${this._config.device || ''}
            @input=${(ev) => this._onValue('device', ev)}
            placeholder="wortuhr"
          />
        </div>
        <div class="row">
          <span class="label">Entity Overrides (optional)</span>
          <input
            type="text"
            .value=${this._config.entities ? JSON.stringify(this._config.entities) : ''}
            @input=${this._onEntities.bind(this)}
            placeholder='{"power":"switch.wortuhr"}'
          />
        </div>
      </div>
    `;
  }

  _onValue(key, ev) {
    const value = ev.target.value;
    const newConfig = { ...this._config, [key]: value };
    this._emitConfig(newConfig);
  }

  _onEntities(ev) {
    const value = ev.target.value;
    let parsed = undefined;
    try {
      parsed = value ? JSON.parse(value) : undefined;
    } catch (e) {
      parsed = undefined;
    }
    const newConfig = { ...this._config };
    if (parsed) {
      newConfig.entities = parsed;
    } else {
      delete newConfig.entities;
    }
    this._emitConfig(newConfig);
  }

  _onSelectDevice(ev) {
    const value = ev.target.value;
    this._emitConfig({ ...this._config, device: value });
  }

  _deviceOptions() {
    if (!this.hass || !this.hass.states) return [];
    const bases = new Set();
    const suffixes = [
      'power', 'wortuhr_power',
      'efx', 'ubergangseffekt', 'uebergangseffekt',
      'ani', 'animationseffekt',
      'v1', 'vordergrundfarbe_1', 'vordergrund_farbe_1',
      'v2', 'vordergrundfarbe_2', 'vordergrund_farbe_2',
      'h1', 'hintergrundfarbe_1', 'hintergrund_farbe_1',
      'h2', 'hintergrundfarbe_2', 'hintergrund_farbe_2',
      'vs', 'vordergrundfarbschema', 'vordergrund_farbschema',
      'hs', 'hintergrundfarbschema', 'hintergrund_farbschema',
      'efxtime', 'ubergangsgeschwindigkeit', 'uebergangsgeschwindigkeit',
      'anitime', 'animationsgeschwindigkeit',
      'anidepth', 'animationsstarke', 'animationsintentat',
      'ip_address', 'ip_adresse', 'ipaddress', 'ip',
      'uptime',
      'rssi', 'wifi_signalstarke', 'wifi_signalstärke', 'wifi_signal', 'wifisignal',
      'system_load', 'system_auslastung', 'systemload', 'speicherauslastung',
      'last_ntp_sync', 'letzter_ntp_sync', 'lastntpsync', 'ntpsync'
    ];
    
    const deviceEntityCounts = {};
    
    Object.entries(this.hass.states).forEach(([eid, st]) => {
      const uid = st?.attributes?.unique_id;
      if (uid && uid.includes('_')) {
        const normUid = this._norm(uid);
        for (const suffix of suffixes) {
          const normSuffix = this._norm(suffix);
          const suffixPattern = '_' + normSuffix;
          if (normUid.endsWith(suffixPattern)) {
            const base = normUid.substring(0, normUid.length - suffixPattern.length);
            if (base) {
              const cleanBase = base.endsWith('_wortuhr') ? base.substring(0, base.length - 8) : base;
              if (cleanBase.startsWith('wortuhr')) {
                bases.add(cleanBase);
                deviceEntityCounts[cleanBase] = (deviceEntityCounts[cleanBase] || 0) + 1;
              }
            }
            return;
          }
        }
      }
      
      const eidParts = eid.split('.');
      const eidName = eidParts[eidParts.length - 1];
      
      for (const suffix of suffixes) {
        const normSuffix = this._norm(suffix);
        const suffixPattern = '_' + normSuffix;
        const normEidName = this._norm(eidName);
        
        if (normEidName.endsWith(suffixPattern)) {
          const base = normEidName.substring(0, normEidName.length - suffixPattern.length);
          if (base) {
            const cleanBase = base.endsWith('_wortuhr') ? base.substring(0, base.length - 8) : base;
            if (cleanBase.startsWith('wortuhr')) {
              bases.add(cleanBase);
              deviceEntityCounts[cleanBase] = (deviceEntityCounts[cleanBase] || 0) + 1;
            }
          }
          return;
        }
      }
    });
    
    return Array.from(bases)
      .filter(base => deviceEntityCounts[base] >= 3)
      .sort();
  }

  _norm(str) {
    return (str || '')
      .toLowerCase()
      .replace(/[^a-z0-9]+/g, '_')
      .replace(/^_+|_+$/g, '');
  }

  _emitConfig(config) {
    this._config = config;
    this.dispatchEvent(new CustomEvent('config-changed', { detail: { config } }));
  }
}

customElements.define('wortuhr-card-editor', WortuhrCardEditor);
