/*
 * Wortuhr Home Assistant Card
 * - Only needs `device:` (MQTT prefix/device id) in the Lovelace editor
 * - Auto-wires all discovered Wortuhr entities via known suffixes
 * - Optional overrides via `entities:{ ... }`
 */

const LitElementBase = window.LitElement || Object.getPrototypeOf(customElements.get('ha-panel-lovelace'));
const html = (LitElementBase && LitElementBase.prototype.html) || window.html;
const css = (LitElementBase && LitElementBase.prototype.css) || window.css;

if (!LitElementBase || !html || !css) {
  throw new Error('LitElement oder lit-html nicht gefunden. Bitte Home Assistant neu laden.');
}

class WortuhrCard extends LitElementBase {
  static get properties() {
    return {
      hass: {},
      config: {},
    };
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
      .card {
        padding: 16px;
        background: linear-gradient(145deg, #0f172a, #111827);
        color: #e5e7eb;
        border-radius: 16px;
        box-shadow: 0 12px 30px rgba(0, 0, 0, 0.35);
      }
      .header {
        display: flex;
        align-items: center;
        justify-content: space-between;
        margin-bottom: 12px;
      }
      .title {
        font-size: 1.2rem;
        font-weight: 700;
        letter-spacing: 0.02em;
      }
      .subtitle {
        font-size: 0.85rem;
        color: #9ca3af;
      }
      .grid {
        display: grid;
        grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
        gap: 12px;
      }
      .block {
        background: rgba(255, 255, 255, 0.04);
        border: 1px solid rgba(255, 255, 255, 0.06);
        border-radius: 12px;
        padding: 12px;
      }
      .block h4 {
        margin: 0 0 8px 0;
        font-size: 0.95rem;
        font-weight: 600;
        color: #f3f4f6;
      }
      .row {
        display: grid;
        grid-template-columns: auto 1fr auto;
        align-items: center;
        gap: 8px;
        margin: 6px 0;
      }
      .label {
        font-size: 0.9rem;
        color: #d1d5db;
      }
      select, input[type="color"], input[type="range"] {
        width: 100%;
        background: rgba(30, 41, 59, 0.8);
        border: 1px solid rgba(255, 255, 255, 0.15);
        color: #e5e7eb;
        border-radius: 8px;
        padding: 8px;
        box-sizing: border-box;
        transition: all 0.2s ease;
      }
      select option {
        background: #1e293b;
        color: #e5e7eb;
      }
      select:hover, input[type="color"]:hover, input[type="range"]:hover {
        background: rgba(30, 41, 59, 0.95);
        border-color: rgba(255, 255, 255, 0.25);
      }
      select:focus, input[type="color"]:focus, input[type="range"]:focus {
        outline: none;
        background: rgba(255, 255, 255, 0.1);
        border-color: rgba(34, 211, 238, 0.5);
        box-shadow: 0 0 12px rgba(34, 211, 238, 0.2);
      }
      input[type="range"] {
        padding: 0;
        cursor: pointer;
      }
      input[type="color"] {
        cursor: pointer;
        height: 42px;
      }
      .color-display {
        display: inline-block;
        width: 32px;
        height: 32px;
        border-radius: 6px;
        border: 2px solid rgba(255, 255, 255, 0.2);
        box-shadow: 0 2px 8px rgba(0, 0, 0, 0.3);
      }
      button {
        background: linear-gradient(135deg, #22d3ee, #06b6d4);
        color: #0b1324;
        border: none;
        border-radius: 10px;
        padding: 10px 12px;
        font-weight: 700;
        cursor: pointer;
        box-shadow: 0 8px 18px rgba(6, 182, 212, 0.35);
      }
      button.secondary {
        background: rgba(255, 255, 255, 0.06);
        color: #e5e7eb;
        box-shadow: none;
      }
      .chip-row {
        display: flex;
        gap: 8px;
        flex-wrap: wrap;
        margin-top: 6px;
      }
      .chip {
        padding: 6px 10px;
        border-radius: 999px;
        background: rgba(255, 255, 255, 0.05);
        border: 1px solid rgba(255, 255, 255, 0.08);
        font-size: 0.85rem;
        cursor: pointer;
      }
      .chip.active {
        background: rgba(34, 211, 238, 0.18);
        border-color: rgba(34, 211, 238, 0.45);
        color: #67e8f9;
      }
      .missing {
        color: #fbbf24;
        font-size: 0.85rem;
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
    return 5;
  }

  render() {
    if (!this.hass || !this.config) return html``;
    const device = (this.config.device || '').trim();
    const entities = this._entityMap(device);
    const powerState = this._state(entities.power);
    
    // Get brightness from any color entity (all should have same value)
    const v1State = this._state(entities.v1);
    const brightness = v1State?.attributes?.brightness ?? 255;

    return html`
      <ha-card class="card">
        <div class="header">
          <div>
            <div class="title">${this.config.title || 'Wortuhr'}</div>
            <div class="subtitle">${device ? `Gerät: ${device}` : 'Entity-Overrides aktiv'}</div>
          </div>
          ${powerState
            ? html`<button @click=${() => this._togglePower(entities.power)}>
                ${powerState.state === 'on' ? 'Ausschalten' : 'Einschalten'}
              </button>`
            : html`<span class="missing">Schalter nicht gefunden</span>`}
        </div>

        <!-- Master Brightness Control -->
        <div style="background: rgba(255, 255, 255, 0.04); border: 1px solid rgba(255, 255, 255, 0.06); border-radius: 12px; padding: 12px; margin-bottom: 12px;">
          <div class="row">
            <span class="label" style="font-weight: 600;">🔆 Helligkeit</span>
            <input
              type="range"
              min="1"
              max="255"
              .value=${brightness}
              @change=${(ev) => this._setBrightnessAll(entities, Number(ev.target.value))}
              style="cursor: pointer;"
            />
            <span class="label" style="font-size: 0.8rem; color: #9ca3af; min-width: 45px; text-align: right;">${Math.round((brightness / 255) * 100)}%</span>
          </div>
        </div>

        <div class="grid">
          <div class="block">
            <h4>Effekte</h4>
            ${this._selectRow('Übergang', entities.effect, this._effectOptions())}
            ${this._selectRow('Tempo', entities.effectTime, this._speedOptions())}
          </div>

          <div class="block">
            <h4>Animation</h4>
            ${this._selectRow('Animation', entities.animation, this._animationOptions())}
            ${this._selectRow('Tempo', entities.animationTime, this._speedOptions())}
            ${this._selectRow('Intensität', entities.animationDepth, this._depthOptions())}
          </div>

          <div class="block">
            <h4>Vordergrund</h4>
            ${this._colorRow('Farbe 1', entities.v1)}
            ${this._colorRow('Farbe 2', entities.v2)}
            ${this._selectRow('Schema', entities.vs, this._schemaOptions())}
          </div>

          <div class="block">
            <h4>Hintergrund</h4>
            ${this._colorRow('Farbe 1', entities.h1)}
            ${this._colorRow('Farbe 2', entities.h2)}
            ${this._selectRow('Schema', entities.hs, this._schemaOptions())}
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
      
      // Try exact match: base_suffix
      if (normUid === `${normBase}_${normSuffix}`) {
        return eid;
      }
      
      // Try with extra "wortuhr" in the middle (bug workaround): base_wortuhr_suffix
      if (normUid === `${normBase}_wortuhr_${normSuffix}`) {
        return eid;
      }
      
      // Try ending check: if uid ends with the suffix and contains the base
      if (normUid.endsWith(`_${normSuffix}`) && normUid.includes(normBase)) {
        return eid;
      }
    }
    return undefined;
  }

  _findEntityBySuffix(base, suffix) {
    if (!this.hass || !this.hass.states) return undefined;
    const normBase = this._norm(base);
    const normSuffix = this._norm(suffix);
    
    // Map short suffixes to German entity names - indexed by SHORT suffix!
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
    };
    
    const possibleSuffixes = suffixMaps[normSuffix] || [suffix];
    
    for (const [eid, st] of Object.entries(this.hass.states)) {
      const normEid = this._norm(eid);
      const uid = st?.attributes?.unique_id;
      const normUid = uid ? this._norm(uid) : '';
      
      // Try each possible suffix variant
      for (const suff of possibleSuffixes) {
        const normSuff = this._norm(suff);
        
        // Check entity_id: should be like "switch.base_suffix" or "light.base_suffix"
        if (normBase && normEid === `switch_${normBase}_${normSuff}`) return eid;
        if (normBase && normEid === `light_${normBase}_${normSuff}`) return eid;
        if (normBase && normEid === `select_${normBase}_${normSuff}`) return eid;
        
        // Remove domain prefix and check
        const eidWithoutDomain = normEid.split('_').slice(1).join('_');
        if (normBase && eidWithoutDomain === `${normBase}_${normSuff}`) return eid;
        if (normBase && eidWithoutDomain === `${normBase}_wortuhr_${normSuff}`) return eid;
        
        // Check unique_id as fallback
        if (normBase && normUid === `${normBase}_${normSuff}`) return eid;
        if (normBase && normUid === `${normBase}_wortuhr_${normSuff}`) return eid;
        
        // Check if normalized unique_id matches base_suffix pattern
        if (normBase && normUid.length > normBase.length) {
          const uIdAfterBase = normUid.substring(normBase.length);
          if (uIdAfterBase === `_${normSuff}` || uIdAfterBase === `_wortuhr_${normSuff}`) {
            return eid;
          }
        }
        
        // Last resort: if uid ends with the suffix
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

  _selectRow(label, entityId, fallbackOptions) {
    const stateObj = this._state(entityId);
    const options = stateObj?.attributes?.options || fallbackOptions;
    const currentValue = stateObj?.state || '';
    
    if (!stateObj || !options) {
      return html`<div class="row"><span class="missing">${label}: Entity fehlt</span></div>`;
    }
    
    return html`
      <div class="row">
        <span class="label">${label}</span>
        <select
          .value=${currentValue}
          @change=${(ev) => this._select(entityId, ev.target.value)}
          style="appearance: none; background-image: url('data:image/svg+xml;utf8,<svg xmlns=%22http://www.w3.org/2000/svg%22 width=%2212%22 height=%228%22 viewBox=%220 0 12 8%22><path fill=%22%23e5e7eb%22 d=%22M0 0l6 8 6-8z%22/></svg>'); background-repeat: no-repeat; background-position: right 8px center; padding-right: 28px;"
        >
          ${options.map((opt) => html`<option value=${opt} ?selected=${opt === currentValue}>${opt}</option>`) }
        </select>
      </div>
    `;
  }

  _colorRow(label, entityId) {
    const stateObj = this._state(entityId);
    if (!stateObj) return html`<div class="row"><span class="missing">${label}: Entity fehlt</span></div>`;
    const rgb = stateObj.attributes?.rgb_color ?? [0, 0, 0];
    const hex = this._rgbToHex(rgb);
    const displayColor = `rgb(${rgb[0]}, ${rgb[1]}, ${rgb[2]})`;
    const brightness = stateObj.attributes?.brightness ?? 255;
    
    return html`
      <div class="row">
        <span class="label">${label}</span>
        <div style="display: flex; gap: 8px; align-items: center; flex: 1;">
          <input
            type="color"
            .value=${hex}
            @change=${(ev) => this._setColor(entityId, ev.target.value, brightness)}
          />
          <div class="color-display" style="background: ${displayColor};"></div>
        </div>
      </div>
    `;
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

  _setBrightness(entityId, brightness, rgb) {
    if (!entityId) return;
    this.hass.callService('light', 'turn_on', {
      entity_id: entityId,
      brightness,
      rgb_color: rgb,
    });
  }

  _setBrightnessAll(entities, brightness) {
    // Set brightness for all color entities
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
    return [
      'kein Effekt',
      'zufällig',
      'Fade',
      'Running',
      'Schlange',
      'Zeilen',
      'Scrollen',
      'Slide in',
      'Diagonal',
      'Rain',
      'Spirale',
      'Schlangenfresser',
      'Raute',
      'Feuerwerk',
    ];
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

customElements.define('wortuhr-card', WortuhrCard);
window.customCards = window.customCards || [];
window.customCards.push({
  type: 'wortuhr-card',
  name: 'Wortuhr Card',
  description: 'Steuerung für die Wortuhr (MQTT Discovery)',
});

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
      'anidepth', 'animationsstarke', 'animationsintentat'
    ];
    
    // Track found entities per device to filter out non-Wortuhr devices
    const deviceEntityCounts = {};
    
    Object.entries(this.hass.states).forEach(([eid, st]) => {
      // Try unique_id first
      const uid = st?.attributes?.unique_id;
      if (uid && uid.includes('_')) {
        const normUid = this._norm(uid);
        // Try to find which suffix this uid ends with
        for (const suffix of suffixes) {
          const normSuffix = this._norm(suffix);
          const suffixPattern = '_' + normSuffix;
          if (normUid.endsWith(suffixPattern)) {
            const base = normUid.substring(0, normUid.length - suffixPattern.length);
            if (base) {
              // Filter: Only accept devices that start with "wortuhr"
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
      
      // Fallback: try entity_id pattern - remove domain prefix first
      const eidParts = eid.split('.');
      const eidName = eidParts[eidParts.length - 1]; // e.g., "wortuhr_testuhr11_vordergrundfarbe_1"
      
      for (const suffix of suffixes) {
        const normSuffix = this._norm(suffix);
        const suffixPattern = '_' + normSuffix;
        const normEidName = this._norm(eidName);
        
        if (normEidName.endsWith(suffixPattern)) {
          const base = normEidName.substring(0, normEidName.length - suffixPattern.length);
          if (base) {
            // Filter: Only accept devices that start with "wortuhr"
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
    
    // Only return devices that have at least 3 Wortuhr entities (to avoid random single entities)
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
