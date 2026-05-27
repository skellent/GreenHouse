(function () {
  const storageKeys = {
    apiBase: "greenhouse.apiBase",
    theme: "greenhouse.theme",
    language: "greenhouse.language",
    autoRefresh: "greenhouse.autoRefresh",
    refreshIntervalMs: "greenhouse.refreshIntervalMs",
    historyEnabled: "greenhouse.historyEnabled",
    history: "greenhouse.history",
  };

  const translations = {
    es: {
      appName: "GreenHouse",
      appSubtitle: "Invernadero autonomo con IA",
      dashboard: "Monitoreo",
      camera: "Camara",
      history: "Historial",
      settings: "Configurar",
      credits: "Creditos",
      apiAddress: "Direccion API",
      refresh: "Actualizar",
      save: "Guardar",
      autoRefresh: "Auto",
      refreshInterval: "Intervalo",
      every1s: "1 segundo",
      every2s: "2 segundos",
      recordHistory: "Registrar historial",
      saveReading: "Guardar lectura",
      historySaved: "Lectura guardada",
      historyEmpty: "Aun no hay lecturas guardadas",
      clearHistory: "Limpiar",
      exportHistory: "Exportar CSV",
      samples: "registros",
      latestReadings: "Lecturas recientes",
      historyOn: "Historial activo",
      historyOff: "Historial detenido",
      historyCleared: "Historial limpiado",
      connected: "Datos actualizados",
      loading: "Consultando invernadero...",
      offline: "Sin conexion con la API",
      ready: "Listo",
      lastUpdate: "Ultima lectura",
      never: "Sin lecturas",
      environment: "Ambiente",
      automation: "IA y actuadores",
      hardware: "ESP32",
      ambientTemperature: "Temperatura ambiente",
      waterTemperature: "Temperatura agua",
      ambientHumidity: "Humedad ambiente",
      soilHumidity: "Humedad suelo",
      lightIntensity: "Intensidad luz",
      height: "Altura",
      espTemperature: "Temperatura ESP32",
      camStatus: "Estado camara",
      uvIntensity: "Intensidad UV",
      irrigation: "Riego",
      ventilation: "Ventilacion",
      profile: "Perfil",
      active: "Activo",
      inactive: "Inactivo",
      unknown: "Desconocido",
      online: "En linea",
      offlineShort: "Fuera",
      photo: "Fotografia",
      capture: "Capturar",
      download: "Descargar",
      noPhoto: "Sin fotografia",
      photoSaved: "Foto descargada",
      photoError: "No se pudo obtener la foto",
      plantProfile: "Perfil de planta",
      profile1: "Desertico",
      profile2: "Tropical",
      profile3: "Alpino",
      profileSaved: "Perfil actualizado",
      wifi: "WiFi",
      ssid: "Red",
      password: "Contrasena",
      connectWifi: "Reconectar",
      wifiSent: "Orden enviada. Espera unos 15s.",
      theme: "Tema",
      light: "Claro",
      dark: "Oscuro",
      device: "Sistema",
      language: "Idioma",
      apiPlaceholder: "http://192.168.1.50",
      creditsTitle: "SKELL'S GREENHOUSE V3.0",
      developedBy: "Developed By:",
      inferenceBusy: "Inferencia en progreso",
      invalidApi: "Introduce una direccion valida",
      manualTitle: "Manual de usuario",
      projectTitle: "Sobre el proyecto",
      projectText: "GreenHouse es una aplicacion movil para monitorear y configurar un invernadero autonomo basado en ESP32-S3, sensores ambientales, camara e inferencia de inteligencia artificial. La app consulta la API local del dispositivo, muestra el estado del cultivo y permite guardar evidencia historica para revisar el comportamiento del sistema.",
    },
    en: {
      appName: "GreenHouse",
      appSubtitle: "Autonomous AI greenhouse",
      dashboard: "Monitor",
      camera: "Camera",
      history: "History",
      settings: "Settings",
      credits: "Credits",
      apiAddress: "API address",
      refresh: "Refresh",
      save: "Save",
      autoRefresh: "Auto",
      refreshInterval: "Interval",
      every1s: "1 second",
      every2s: "2 seconds",
      recordHistory: "Record history",
      saveReading: "Save reading",
      historySaved: "Reading saved",
      historyEmpty: "No saved readings yet",
      clearHistory: "Clear",
      exportHistory: "Export CSV",
      samples: "samples",
      latestReadings: "Recent readings",
      historyOn: "History active",
      historyOff: "History paused",
      historyCleared: "History cleared",
      connected: "Data updated",
      loading: "Contacting greenhouse...",
      offline: "API connection failed",
      ready: "Ready",
      lastUpdate: "Last reading",
      never: "No readings",
      environment: "Environment",
      automation: "AI and actuators",
      hardware: "ESP32",
      ambientTemperature: "Ambient temperature",
      waterTemperature: "Water temperature",
      ambientHumidity: "Ambient humidity",
      soilHumidity: "Soil humidity",
      lightIntensity: "Light intensity",
      height: "Height",
      espTemperature: "ESP32 temperature",
      camStatus: "Camera status",
      uvIntensity: "UV intensity",
      irrigation: "Irrigation",
      ventilation: "Ventilation",
      profile: "Profile",
      active: "Active",
      inactive: "Inactive",
      unknown: "Unknown",
      online: "Online",
      offlineShort: "Offline",
      photo: "Photo",
      capture: "Capture",
      download: "Download",
      noPhoto: "No photo",
      photoSaved: "Photo downloaded",
      photoError: "Could not fetch photo",
      plantProfile: "Plant profile",
      profile1: "Desert",
      profile2: "Tropical",
      profile3: "Alpine",
      profileSaved: "Profile updated",
      wifi: "WiFi",
      ssid: "Network",
      password: "Password",
      connectWifi: "Reconnect",
      wifiSent: "Request sent. Wait about 15s.",
      theme: "Theme",
      light: "Light",
      dark: "Dark",
      device: "System",
      language: "Language",
      apiPlaceholder: "http://192.168.1.50",
      creditsTitle: "SKELL'S GREENHOUSE V3.0",
      developedBy: "Developed By:",
      inferenceBusy: "Inference in progress",
      invalidApi: "Enter a valid address",
      manualTitle: "User manual",
      projectTitle: "About the project",
      projectText: "GreenHouse is a mobile app for monitoring and configuring an autonomous ESP32-S3 greenhouse with environmental sensors, camera and AI inference. The app talks to the local device API, shows crop status and stores historical evidence for later review.",
    },
  };

  const languageNames = { es: "Español", en: "English" };
  const tabs = [
    { id: "dashboard", icon: "activity" },
    { id: "camera", icon: "camera" },
    { id: "history", icon: "history" },
    { id: "settings", icon: "settings" },
    { id: "credits", icon: "spark" },
  ];

  const iconPaths = {
    activity: '<path d="M3 12h4l3-8 4 16 3-8h4"/>',
    camera: '<path d="M14 7h.01"/><path d="M5 7h2l1.5-2h7L17 7h2a2 2 0 0 1 2 2v8a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V9a2 2 0 0 1 2-2Z"/><circle cx="12" cy="13" r="3"/>',
    clock: '<circle cx="12" cy="12" r="9"/><path d="M12 7v5l3 2"/>',
    cloud: '<path d="M17.5 18H7a4 4 0 1 1 .8-7.9A5.5 5.5 0 0 1 18 12a3 3 0 0 1-.5 6Z"/>',
    database: '<ellipse cx="12" cy="5" rx="7" ry="3"/><path d="M5 5v6c0 1.7 3.1 3 7 3s7-1.3 7-3V5"/><path d="M5 11v6c0 1.7 3.1 3 7 3s7-1.3 7-3v-6"/>',
    download: '<path d="M12 3v12"/><path d="m7 10 5 5 5-5"/><path d="M5 21h14"/>',
    droplet: '<path d="M12 3s6 6.2 6 10a6 6 0 0 1-12 0c0-3.8 6-10 6-10Z"/>',
    export: '<path d="M12 3v12"/><path d="m7 8 5-5 5 5"/><path d="M5 13v6h14v-6"/>',
    globe: '<circle cx="12" cy="12" r="9"/><path d="M3 12h18"/><path d="M12 3c2.2 2.5 3.3 5.5 3.3 9S14.2 18.5 12 21c-2.2-2.5-3.3-5.5-3.3-9S9.8 5.5 12 3Z"/>',
    history: '<path d="M3 12a9 9 0 1 0 3-6.7"/><path d="M3 4v6h6"/><path d="M12 7v5l4 2"/>',
    image: '<rect x="3" y="5" width="18" height="14" rx="2"/><circle cx="8" cy="10" r="1.6"/><path d="m21 16-5-5L5 19"/>',
    leaf: '<path d="M5 21c7-1 13-7 14-16-9 1-15 7-16 14 4-2 8-5 11-9"/>',
    moon: '<path d="M20 14.5A8 8 0 0 1 9.5 4 8.5 8.5 0 1 0 20 14.5Z"/>',
    refresh: '<path d="M21 12a9 9 0 0 1-15.3 6.4"/><path d="M3 12A9 9 0 0 1 18.3 5.6"/><path d="M18 2v4h4"/><path d="M6 22v-4H2"/>',
    save: '<path d="M5 3h12l2 2v16H5Z"/><path d="M8 3v6h8V3"/><path d="M8 21v-7h8v7"/>',
    settings: '<path d="M12 15.5a3.5 3.5 0 1 0 0-7 3.5 3.5 0 0 0 0 7Z"/><path d="M19.4 15a1.8 1.8 0 0 0 .4 2l.1.1-2 3.4-.2-.1a1.8 1.8 0 0 0-2.1.3 1.8 1.8 0 0 0-.5 1.3h-4a1.8 1.8 0 0 0-.5-1.3 1.8 1.8 0 0 0-2.1-.3l-.2.1-2-3.4.1-.1a1.8 1.8 0 0 0 .4-2 1.8 1.8 0 0 0-1.6-1.1H5v-4h.2a1.8 1.8 0 0 0 1.6-1.1 1.8 1.8 0 0 0-.4-2l-.1-.1 2-3.4.2.1a1.8 1.8 0 0 0 2.1-.3A1.8 1.8 0 0 0 11.1 2h4a1.8 1.8 0 0 0 .5 1.3 1.8 1.8 0 0 0 2.1.3l.2-.1 2 3.4-.1.1a1.8 1.8 0 0 0-.4 2 1.8 1.8 0 0 0 1.6 1.1h.2v4H21a1.8 1.8 0 0 0-1.6 1.1Z"/>',
    spark: '<path d="m12 3 1.8 5.2L19 10l-5.2 1.8L12 17l-1.8-5.2L5 10l5.2-1.8Z"/><path d="m5 15 .8 2.2L8 18l-2.2.8L5 21l-.8-2.2L2 18l2.2-.8Z"/>',
    sun: '<circle cx="12" cy="12" r="4"/><path d="M12 2v2"/><path d="M12 20v2"/><path d="m4.9 4.9 1.4 1.4"/><path d="m17.7 17.7 1.4 1.4"/><path d="M2 12h2"/><path d="M20 12h2"/><path d="m4.9 19.1 1.4-1.4"/><path d="m17.7 6.3 1.4-1.4"/>',
    thermometer: '<path d="M14 14.8V5a2 2 0 0 0-4 0v9.8a4 4 0 1 0 4 0Z"/><path d="M12 9v6"/>',
    trash: '<path d="M4 7h16"/><path d="M10 11v6"/><path d="M14 11v6"/><path d="M6 7l1 14h10l1-14"/><path d="M9 7V4h6v3"/>',
    wifi: '<path d="M5 13a10 10 0 0 1 14 0"/><path d="M8.5 16.5a5 5 0 0 1 7 0"/><path d="M12 20h.01"/><path d="M2 9a15 15 0 0 1 20 0"/>',
    wind: '<path d="M3 8h12a3 3 0 1 0-3-3"/><path d="M3 14h16a3 3 0 1 1-3 3"/><path d="M3 20h8"/>',
  };

  const app = document.querySelector("#app");
  let refreshTimer;

  const state = {
    apiBase: localStorage.getItem(storageKeys.apiBase) || "http://192.168.1.50",
    activeTab: "dashboard",
    autoRefresh: localStorage.getItem(storageKeys.autoRefresh) !== "false",
    refreshIntervalMs: readRefreshInterval(),
    historyEnabled: localStorage.getItem(storageKeys.historyEnabled) === "true",
    history: readHistory(),
    data: null,
    lastUpdate: null,
    loadingData: false,
    loadingPhoto: false,
    photoUrl: null,
    photoBlob: null,
    status: translations.es.ready,
    statusTone: "idle",
    theme: readTheme(),
    language: readLanguage(),
    wifiRed: "",
    wifiPsw: "",
  };

  function readTheme() {
    const value = localStorage.getItem(storageKeys.theme);
    return value === "light" || value === "dark" || value === "device" ? value : "device";
  }

  function readLanguage() {
    const value = localStorage.getItem(storageKeys.language);
    return value === "es" || value === "en" ? value : "es";
  }

  function readRefreshInterval() {
    const value = Number(localStorage.getItem(storageKeys.refreshIntervalMs));
    return value === 1000 || value === 2000 ? value : 2000;
  }

  function readHistory() {
    try {
      const parsed = JSON.parse(localStorage.getItem(storageKeys.history) || "[]");
      return Array.isArray(parsed) ? parsed.slice(0, 500) : [];
    } catch {
      return [];
    }
  }

  function t(key) {
    return (translations[state.language] && translations[state.language][key]) || translations.es[key] || key;
  }

  function icon(name, className = "ui-icon") {
    return `<svg class="${className}" viewBox="0 0 24 24" aria-hidden="true">${iconPaths[name] || ""}</svg>`;
  }

  function isNativePlatform() {
    const cap = window.Capacitor;
    if (!cap) return false;
    if (typeof cap.isNativePlatform === "function") return cap.isNativePlatform();
    return typeof cap.getPlatform === "function" && cap.getPlatform() !== "web";
  }

  function filesystemPlugin() {
    return window.Capacitor && window.Capacitor.Plugins && window.Capacitor.Plugins.Filesystem;
  }

  function normalizeBaseUrl(value) {
    const trimmed = String(value || "").trim();
    if (!trimmed) return "";
    const withProtocol = /^https?:\/\//i.test(trimmed) ? trimmed : `http://${trimmed}`;
    return withProtocol.replace(/\/+$/, "");
  }

  function applyTheme() {
    const deviceDark = window.matchMedia("(prefers-color-scheme: dark)").matches;
    const resolved = state.theme === "device" ? (deviceDark ? "dark" : "light") : state.theme;
    document.documentElement.dataset.theme = resolved;
    document.documentElement.dataset.themeMode = state.theme;
    document.documentElement.lang = state.language;
  }

  function setStatus(message, tone = "idle") {
    state.status = message;
    state.statusTone = tone;
  }

  function valueText(value, unit = "", fractionDigits = 1) {
    if (typeof value !== "number" || Number.isNaN(value)) return "--";
    const formatted = Number.isInteger(value) ? String(value) : value.toFixed(fractionDigits);
    return `${formatted}${unit}`;
  }

  function booleanText(value) {
    if (value === true || value === 1 || value === "1" || value === "true" || value === "on") return t("active");
    if (value === false || value === 0 || value === "0" || value === "false" || value === "off") return t("inactive");
    return t("unknown");
  }

  function cameraStatusText(value) {
    if (value === true || value === 1 || value === "1" || value === "true" || value === "on") return t("online");
    if (value === false || value === 0 || value === "0" || value === "false" || value === "off") return t("offlineShort");
    if (value === undefined || value === null || value === "") return t("unknown");
    return String(value);
  }

  function profileLabel(profile) {
    const value = typeof profile === "number" ? profile : Number(profile);
    if (value === 1 || value === 2 || value === 3) return t(`profile${value}`);
    return "--";
  }

  function friendlyError(error) {
    const message = error instanceof Error ? error.message : String(error || "");
    const normalized = message.toLowerCase();
    if (normalized.includes("failed to fetch") || normalized.includes("load failed") || normalized.includes("networkerror") || normalized.includes("aborted")) {
      return t("offline");
    }
    if (message.includes("Inferencia")) return t("inferenceBusy");
    return message || t("offline");
  }

  function apiUrl(path) {
    return `${state.apiBase}${path}`;
  }

  async function readError(response) {
    try {
      const body = await response.json();
      return body.error || body.mensaje || response.statusText;
    } catch {
      return response.statusText;
    }
  }

  async function fetchWithTimeout(url, init = {}, timeoutMs = 8000) {
    const controller = new AbortController();
    const timeout = window.setTimeout(() => controller.abort(), timeoutMs);
    try {
      return await fetch(url, { ...init, signal: controller.signal });
    } finally {
      window.clearTimeout(timeout);
    }
  }

  async function loadData() {
    const normalized = normalizeBaseUrl(state.apiBase);
    if (!normalized) {
      setStatus(t("invalidApi"), "error");
      render();
      return;
    }

    state.apiBase = normalized;
    state.loadingData = true;
    setStatus(t("loading"), "idle");
    render();

    try {
      const response = await fetchWithTimeout(apiUrl("/api/datos"), { headers: { Accept: "application/json" } });
      if (!response.ok) throw new Error(await readError(response));
      const data = await response.json();
      if (data.error) throw new Error(data.error);

      state.data = data;
      state.lastUpdate = new Date();
      if (state.historyEnabled) addHistoryEntry(data, false);
      setStatus(t("connected"), "ok");
    } catch (error) {
      setStatus(friendlyError(error), "error");
    } finally {
      state.loadingData = false;
      render();
    }
  }

  async function refreshPhoto(downloadAfterFetch = false) {
    const normalized = normalizeBaseUrl(state.apiBase);
    if (!normalized) {
      setStatus(t("invalidApi"), "error");
      render();
      return;
    }

    state.apiBase = normalized;
    state.loadingPhoto = true;
    setStatus(t("loading"), "idle");
    render();

    try {
      const url = `${apiUrl("/api/foto")}?t=${Date.now()}`;
      try {
        const warmup = await fetchWithTimeout(url, { headers: { Accept: "image/jpeg" }, cache: "no-store" }, 12000);
        if (warmup.body && typeof warmup.body.cancel === "function") warmup.body.cancel();
      } catch {
        // The ESP32-S3 camera endpoint can return a stale cached frame first; the second request is authoritative.
      }

      const response = await fetchWithTimeout(`${apiUrl("/api/foto")}?t=${Date.now()}-live`, { headers: { Accept: "image/jpeg" }, cache: "no-store" }, 12000);
      if (!response.ok) throw new Error(await response.text());

      const blob = await response.blob();
      if (state.photoUrl) URL.revokeObjectURL(state.photoUrl);
      state.photoBlob = blob;
      state.photoUrl = URL.createObjectURL(blob);
      setStatus(t("connected"), "ok");

      if (downloadAfterFetch) await downloadPhoto();
    } catch {
      setStatus(t("photoError"), "error");
    } finally {
      state.loadingPhoto = false;
      render();
    }
  }

  async function downloadPhoto() {
    if (!state.photoBlob) {
      await refreshPhoto(true);
      return;
    }

    const fileName = `greenhouse-${new Date().toISOString().replace(/[:.]/g, "-")}.jpg`;

    if (isNativePlatform() && filesystemPlugin()) {
      await filesystemPlugin().writeFile({
        path: fileName,
        data: await blobToBase64(state.photoBlob),
        directory: "DOCUMENTS",
      });
    } else {
      downloadBlob(state.photoBlob, fileName);
    }

    setStatus(t("photoSaved"), "ok");
    render();
  }

  function blobToBase64(blob) {
    return new Promise((resolve, reject) => {
      const reader = new FileReader();
      reader.onload = () => {
        const result = String(reader.result);
        resolve(result.includes(",") ? result.split(",")[1] || result : result);
      };
      reader.onerror = () => reject(reader.error);
      reader.readAsDataURL(blob);
    });
  }

  async function postProfile(profile) {
    state.loadingData = true;
    setStatus(t("loading"), "idle");
    render();

    try {
      const response = await fetchWithTimeout(apiUrl("/api/perfil"), {
        method: "POST",
        headers: { "Content-Type": "text/plain" },
        body: String(profile),
      });
      if (!response.ok) throw new Error(await readError(response));
      const body = await response.json();
      state.data = { ...(state.data || {}), ia: { ...((state.data && state.data.ia) || {}), perfil: body.perfil || profile } };
      setStatus(t("profileSaved"), "ok");
    } catch (error) {
      setStatus(friendlyError(error), "error");
    } finally {
      state.loadingData = false;
      render();
    }
  }

  async function postWifi() {
    if (!state.wifiRed.trim() || !state.wifiPsw) {
      setStatus(t("invalidApi"), "warn");
      render();
      return;
    }

    state.loadingData = true;
    setStatus(t("loading"), "idle");
    render();

    try {
      const response = await fetchWithTimeout(apiUrl("/api/wifi"), {
        method: "POST",
        headers: { "Content-Type": "application/json", Accept: "application/json" },
        body: JSON.stringify({ red: state.wifiRed.trim(), psw: state.wifiPsw }),
      });
      if (!response.ok) throw new Error(await readError(response));
      setStatus(t("wifiSent"), "ok");
      state.wifiPsw = "";
    } catch (error) {
      setStatus(friendlyError(error), "error");
    } finally {
      state.loadingData = false;
      render();
    }
  }

  function saveApiBase() {
    const normalized = normalizeBaseUrl((document.querySelector("#apiBase") || {}).value || state.apiBase);
    if (!normalized) {
      setStatus(t("invalidApi"), "error");
      render();
      return;
    }
    state.apiBase = normalized;
    localStorage.setItem(storageKeys.apiBase, normalized);
    setStatus(t("ready"), "ok");
    render();
  }

  function setTheme(theme) {
    state.theme = theme;
    localStorage.setItem(storageKeys.theme, theme);
    applyTheme();
    render();
  }

  function setLanguage(language) {
    state.language = language;
    localStorage.setItem(storageKeys.language, language);
    applyTheme();
    setStatus(t("ready"), "idle");
    render();
  }

  function setAutoRefresh(enabled) {
    state.autoRefresh = enabled;
    localStorage.setItem(storageKeys.autoRefresh, String(enabled));
    startAutoRefresh();
    render();
  }

  function setRefreshInterval(value) {
    state.refreshIntervalMs = value === 1000 ? 1000 : 2000;
    localStorage.setItem(storageKeys.refreshIntervalMs, String(state.refreshIntervalMs));
    startAutoRefresh();
    render();
  }

  function setHistoryEnabled(enabled) {
    state.historyEnabled = enabled;
    localStorage.setItem(storageKeys.historyEnabled, String(enabled));
    setStatus(enabled ? t("historyOn") : t("historyOff"), enabled ? "ok" : "idle");
    render();
  }

  function persistHistory() {
    localStorage.setItem(storageKeys.history, JSON.stringify(state.history));
  }

  function addHistoryEntry(data, notify = true) {
    if (!data) {
      setStatus(t("never"), "warn");
      render();
      return;
    }

    state.history = [{ id: `${Date.now()}-${Math.random().toString(16).slice(2)}`, timestamp: new Date().toISOString(), data: structuredCloneSafe(data) }, ...state.history].slice(0, 500);
    persistHistory();

    if (notify) {
      setStatus(t("historySaved"), "ok");
      render();
    }
  }

  function structuredCloneSafe(value) {
    return JSON.parse(JSON.stringify(value));
  }

  function clearHistory() {
    state.history = [];
    persistHistory();
    setStatus(t("historyCleared"), "ok");
    render();
  }

  async function exportHistory() {
    const fileName = `greenhouse-history-${new Date().toISOString().replace(/[:.]/g, "-")}.csv`;
    const content = historyToCsv(state.history);

    if (isNativePlatform() && filesystemPlugin()) {
      await filesystemPlugin().writeFile({
        path: fileName,
        data: content,
        directory: "DOCUMENTS",
        encoding: "utf8",
      });
    } else {
      downloadBlob(new Blob([content], { type: "text/csv;charset=utf-8" }), fileName);
    }

    setStatus(t("exportHistory"), "ok");
    render();
  }

  function historyToCsv(entries) {
    const headers = [
      "timestamp",
      "sensores_temperatura_ambiente",
      "sensores_temperatura_agua",
      "sensores_humedad_ambiente",
      "sensores_humedad_suelo",
      "sensores_intensidad_luz",
      "sensores_altura",
      "esp32_temperatura",
      "esp32_camStatus",
      "ia_intensidad_uv",
      "ia_riego",
      "ia_ventilacion",
      "ia_perfil_numero",
      "ia_perfil_nombre",
      "raw_json",
    ];

    const rows = entries.map((entry) => {
      const sensores = (entry.data && entry.data.sensores) || {};
      const esp32 = (entry.data && entry.data.esp32) || {};
      const ia = (entry.data && entry.data.ia) || {};
      return [
        entry.timestamp,
        sensores.temperatura_ambiente,
        sensores.temperatura_agua,
        sensores.humedad_ambiente,
        sensores.humedad_suelo,
        sensores.intensidad_luz,
        sensores.ultrasonido,
        esp32.temperatura,
        esp32.camStatus,
        ia.intensidad_uv,
        ia.riego,
        ia.ventilacion,
        ia.perfil,
        profileLabel(ia.perfil),
        JSON.stringify(entry.data || {}),
      ];
    });

    return [headers, ...rows].map((row) => row.map(csvCell).join(",")).join("\n");
  }

  function csvCell(value) {
    if (value === undefined || value === null) return "";
    const text = String(value).replace(/"/g, '""');
    return /[",\n\r]/.test(text) ? `"${text}"` : text;
  }

  function downloadBlob(blob, fileName) {
    const link = document.createElement("a");
    link.href = URL.createObjectURL(blob);
    link.download = fileName;
    link.click();
    URL.revokeObjectURL(link.href);
  }

  function startAutoRefresh() {
    if (refreshTimer) window.clearInterval(refreshTimer);
    if (!state.autoRefresh) return;
    refreshTimer = window.setInterval(() => {
      if (!state.loadingData) loadData();
    }, state.refreshIntervalMs);
  }

  function metricCard(label, value, tone, iconName) {
    return `<article class="metric metric-${tone}"><span>${icon(iconName)}${label}</span><strong>${value}</strong></article>`;
  }

  function signalRow(label, value, iconName) {
    return `<div class="signal-row"><span>${icon(iconName)}${label}</span><strong>${value}</strong></div>`;
  }

  function renderDashboard() {
    const sensores = (state.data && state.data.sensores) || {};
    const esp32 = (state.data && state.data.esp32) || {};
    const ia = (state.data && state.data.ia) || {};
    const last = state.lastUpdate ? state.lastUpdate.toLocaleTimeString() : t("never");

    return `
      <section class="screen-grid">
        <div class="panel status-panel">
          <div><span class="eyebrow">${t("lastUpdate")}</span><strong>${last}</strong></div>
          <div class="button-row">
            <button class="soft-button ${state.historyEnabled ? "active" : ""}" data-action="historyToggle" aria-label="${t("recordHistory")}" title="${t("recordHistory")}">${icon("database")}${state.historyEnabled ? t("historyOn") : t("historyOff")}</button>
            <button class="icon-button" data-action="saveReading" aria-label="${t("saveReading")}" title="${t("saveReading")}">${icon("save")}</button>
          </div>
        </div>

        <section class="section-block">
          <div class="section-title">
            <h2>${icon("leaf")}${t("environment")}</h2>
            <button class="icon-button ${state.loadingData ? "is-spinning" : ""}" data-action="refresh" aria-label="${t("refresh")}" title="${t("refresh")}">${icon("refresh")}</button>
          </div>
          <div class="metric-grid">
            ${metricCard(t("ambientTemperature"), valueText(sensores.temperatura_ambiente, " °C"), "warm", "thermometer")}
            ${metricCard(t("waterTemperature"), valueText(sensores.temperatura_agua, " °C"), "water", "droplet")}
            ${metricCard(t("ambientHumidity"), valueText(sensores.humedad_ambiente, " %"), "cool", "cloud")}
            ${metricCard(t("soilHumidity"), valueText(sensores.humedad_suelo, " %"), "soil", "leaf")}
            ${metricCard(t("lightIntensity"), valueText(sensores.intensidad_luz, " lx", 0), "light", "sun")}
            ${metricCard(t("height"), valueText(sensores.ultrasonido, " cm"), "neutral", "activity")}
          </div>
        </section>

        <section class="section-block two-column">
          <div>
            <div class="section-title"><h2>${icon("spark")}${t("automation")}</h2></div>
            <div class="signal-list">
              ${signalRow(t("uvIntensity"), valueText(ia.intensidad_uv, "", 2), "sun")}
              ${signalRow(t("irrigation"), booleanText(ia.riego), "droplet")}
              ${signalRow(t("ventilation"), booleanText(ia.ventilacion), "wind")}
              ${signalRow(t("profile"), profileLabel(ia.perfil), "leaf")}
            </div>
          </div>
          <div>
            <div class="section-title"><h2>${icon("settings")}${t("hardware")}</h2></div>
            <div class="signal-list">
              ${signalRow(t("espTemperature"), valueText(esp32.temperatura, " °C"), "thermometer")}
              ${signalRow(t("camStatus"), cameraStatusText(esp32.camStatus), "camera")}
            </div>
          </div>
        </section>
      </section>
    `;
  }

  function renderCamera() {
    return `
      <section class="screen-grid">
        <section class="section-block">
          <div class="section-title">
            <h2>${icon("camera")}${t("photo")}</h2>
            <div class="button-row">
              <button class="icon-button ${state.loadingPhoto ? "is-spinning" : ""}" data-action="photo" aria-label="${t("capture")}" title="${t("capture")}" ${state.loadingPhoto ? "disabled" : ""}>${icon("camera")}</button>
              <button class="icon-button" data-action="download" aria-label="${t("download")}" title="${t("download")}" ${state.loadingPhoto ? "disabled" : ""}>${icon("download")}</button>
            </div>
          </div>
          <div class="photo-frame">
            ${state.photoUrl ? `<img src="${state.photoUrl}" alt="${t("photo")}" />` : `<div class="photo-empty">${icon("image", "empty-icon")}<strong>${t("noPhoto")}</strong></div>`}
          </div>
        </section>
      </section>
    `;
  }

  function renderSettings() {
    const currentProfile = (state.data && state.data.ia && state.data.ia.perfil) || 1;
    return `
      <section class="screen-grid">
        <section class="section-block">
          <div class="section-title"><h2>${icon("globe")}${t("apiAddress")}</h2></div>
          <div class="form-line">
            <input id="apiBase" value="${escapeHtml(state.apiBase)}" placeholder="${t("apiPlaceholder")}" inputmode="url" />
            <button class="primary-button" data-action="saveApi">${icon("save")}${t("save")}</button>
          </div>
          <label class="toggle-line"><span>${icon("refresh")}${t("autoRefresh")}</span><input type="checkbox" data-action="autoRefresh" ${state.autoRefresh ? "checked" : ""} /></label>
          <div class="setting-group">
            <span>${icon("clock")}${t("refreshInterval")}</span>
            <div class="segmented interval-segmented">
              ${[1000, 2000].map((interval) => `<button class="${state.refreshIntervalMs === interval ? "active" : ""}" data-interval="${interval}">${interval === 1000 ? t("every1s") : t("every2s")}</button>`).join("")}
            </div>
          </div>
        </section>

        <section class="section-block">
          <div class="section-title"><h2>${icon("leaf")}${t("plantProfile")}</h2></div>
          <div class="segmented profile-segmented">
            ${[1, 2, 3].map((profile) => `<button class="${currentProfile === profile ? "active" : ""}" data-profile="${profile}">${t(`profile${profile}`)}</button>`).join("")}
          </div>
        </section>

        <section class="section-block">
          <div class="section-title"><h2>${icon("wifi")}${t("wifi")}</h2></div>
          <div class="form-stack">
            <input id="wifiRed" value="${escapeHtml(state.wifiRed)}" placeholder="${t("ssid")}" autocomplete="off" />
            <input id="wifiPsw" value="${escapeHtml(state.wifiPsw)}" placeholder="${t("password")}" type="password" autocomplete="new-password" />
            <button class="primary-button full" data-action="wifi">${icon("wifi")}${t("connectWifi")}</button>
          </div>
        </section>

        <section class="section-block compact">
          <div class="section-title"><h2>${icon("database")}${t("history")}</h2></div>
          <label class="toggle-line"><span>${t("recordHistory")}</span><input type="checkbox" data-action="historyToggleInput" ${state.historyEnabled ? "checked" : ""} /></label>
        </section>

        <section class="section-block compact">
          <div class="section-title"><h2>${icon("moon")}${t("theme")}</h2></div>
          <div class="segmented">
            ${["device", "light", "dark"].map((theme) => `<button class="${state.theme === theme ? "active" : ""}" data-theme="${theme}">${t(theme)}</button>`).join("")}
          </div>
        </section>
      </section>
    `;
  }

  function renderHistory() {
    const entries = state.history;
    return `
      <section class="screen-grid">
        <div class="panel history-summary">
          <div><span class="eyebrow">${t("history")}</span><strong>${entries.length} ${t("samples")}</strong></div>
          <div class="button-row">
            <button class="icon-button" data-action="exportHistory" aria-label="${t("exportHistory")}" title="${t("exportHistory")}" ${entries.length ? "" : "disabled"}>${icon("export")}</button>
            <button class="icon-button danger" data-action="clearHistory" aria-label="${t("clearHistory")}" title="${t("clearHistory")}" ${entries.length ? "" : "disabled"}>${icon("trash")}</button>
          </div>
        </div>
        <section class="section-block">
          <div class="section-title"><h2>${icon("history")}${t("latestReadings")}</h2><button class="soft-button" data-action="saveReading">${icon("save")}${t("saveReading")}</button></div>
          <div class="history-list">${entries.length ? entries.slice(0, 60).map(historyCard).join("") : `<div class="empty-state">${icon("database", "empty-icon")}<strong>${t("historyEmpty")}</strong></div>`}</div>
        </section>
      </section>
    `;
  }

  function historyCard(entry) {
    const sensores = (entry.data && entry.data.sensores) || {};
    const ia = (entry.data && entry.data.ia) || {};
    return `
      <article class="history-card">
        <div class="history-card-head"><strong>${new Date(entry.timestamp).toLocaleString()}</strong><span>${profileLabel(ia.perfil)}</span></div>
        <div class="history-values">
          ${historyValue("thermometer", valueText(sensores.temperatura_ambiente, " °C"))}
          ${historyValue("droplet", valueText(sensores.humedad_suelo, " %"))}
          ${historyValue("sun", valueText(sensores.intensidad_luz, " lx", 0))}
          ${historyValue("activity", valueText(ia.intensidad_uv, "", 2))}
          ${historyValue("droplet", booleanText(ia.riego))}
          ${historyValue("wind", booleanText(ia.ventilacion))}
        </div>
      </article>
    `;
  }

  function historyValue(iconName, value) {
    return `<span>${icon(iconName)}${value}</span>`;
  }

  function renderCredits() {
    return `
      <section class="credits-screen">
        <div class="credits-box">
          <img src="./assets/icon.svg" alt="" class="credits-icon" />
          <span class="credits-label">${t("creditsTitle")}</span>
          <strong>${t("developedBy")}</strong>
          <p>Robert Rodriguez "Skellent"</p>
          <p>Christopher Ramirez</p>
          <p>Fabiana Hernandez</p>
        </div>
        <section class="section-block project-info">
          <div class="section-title"><h2>${icon("leaf")}${t("projectTitle")}</h2></div>
          <p>${t("projectText")}</p>
        </section>
        <section class="section-block">
          <div class="section-title"><h2>${icon("database")}${t("manualTitle")}</h2></div>
          <ol class="manual-list">
            <li>Configura la direccion de la API con la IP local del ESP32-S3. Puedes escribir solo la IP; la app agrega http:// automaticamente.</li>
            <li>Activa la lectura automatica y elige un intervalo de 1 o 2 segundos segun la estabilidad de tu red local.</li>
            <li>Usa Monitoreo para observar temperatura ambiente, temperatura del agua, humedad ambiente, humedad del suelo, luz, altura, estado de camara y salidas de IA.</li>
            <li>Activa Registrar historial cuando quieras guardar cada lectura correcta. El historial conserva todos los campos recibidos desde la API y se exporta en CSV.</li>
            <li>En Camara, Capturar realiza dos peticiones seguidas para evitar el bug de imagen cacheada del ESP32-S3; la segunda respuesta es la que se muestra.</li>
            <li>En Perfiles, Desertico, Tropical y Alpino envian 1, 2 o 3 al endpoint /api/perfil.</li>
            <li>En WiFi, envia red y contrasena al endpoint /api/wifi. La placa responde antes de reconectarse; espera alrededor de 15 segundos.</li>
            <li>La descarga de fotos y CSV usa almacenamiento de Documentos en Android cuando Capacitor esta disponible; en navegador usa descarga directa.</li>
          </ol>
        </section>
      </section>
    `;
  }

  function renderMain() {
    if (state.activeTab === "camera") return renderCamera();
    if (state.activeTab === "history") return renderHistory();
    if (state.activeTab === "settings") return renderSettings();
    if (state.activeTab === "credits") return renderCredits();
    return renderDashboard();
  }

  function render() {
    applyTheme();
    app.innerHTML = `
      <div class="app-shell">
        <header class="topbar">
          <div class="brand-lockup">
            <div class="brand-mark"><img src="./assets/icon.svg" alt="" /></div>
            <div><h1>${t("appName")}</h1><p>${t("appSubtitle")}</p></div>
          </div>
          <select class="language-select" data-action="language" aria-label="${t("language")}">
            ${Object.keys(languageNames).map((language) => `<option value="${language}" ${state.language === language ? "selected" : ""}>${languageNames[language]}</option>`).join("")}
          </select>
        </header>
        <main>
          <section class="connection-bar"><div class="status-dot status-${state.statusTone}"></div><span>${state.loadingData || state.loadingPhoto ? t("loading") : state.status}</span></section>
          ${renderMain()}
        </main>
        <nav class="tabbar" aria-label="App">
          ${tabs.map((tab) => `<button class="${state.activeTab === tab.id ? "active" : ""}" data-tab="${tab.id}" aria-label="${t(tab.id)}" title="${t(tab.id)}"><span>${icon(tab.icon)}</span><strong>${t(tab.id)}</strong></button>`).join("")}
        </nav>
      </div>
    `;
    bindEvents();
  }

  function bindEvents() {
    app.querySelectorAll("[data-tab]").forEach((button) => {
      button.addEventListener("click", () => {
        state.activeTab = button.dataset.tab;
        render();
        if (state.activeTab === "camera" && !state.photoUrl) refreshPhoto();
      });
    });

    app.querySelector("[data-action='refresh']")?.addEventListener("click", loadData);
    app.querySelector("[data-action='photo']")?.addEventListener("click", () => refreshPhoto());
    app.querySelector("[data-action='download']")?.addEventListener("click", downloadPhoto);
    app.querySelector("[data-action='saveApi']")?.addEventListener("click", saveApiBase);
    app.querySelector("[data-action='wifi']")?.addEventListener("click", postWifi);
    app.querySelectorAll("[data-action='saveReading']").forEach((button) => button.addEventListener("click", () => addHistoryEntry(state.data)));
    app.querySelector("[data-action='historyToggle']")?.addEventListener("click", () => setHistoryEnabled(!state.historyEnabled));
    app.querySelector("[data-action='clearHistory']")?.addEventListener("click", clearHistory);
    app.querySelector("[data-action='exportHistory']")?.addEventListener("click", exportHistory);
    app.querySelector("[data-action='language']")?.addEventListener("change", (event) => setLanguage(event.target.value));
    app.querySelector("[data-action='autoRefresh']")?.addEventListener("change", (event) => setAutoRefresh(event.target.checked));
    app.querySelector("[data-action='historyToggleInput']")?.addEventListener("change", (event) => setHistoryEnabled(event.target.checked));
    app.querySelectorAll("[data-profile]").forEach((button) => button.addEventListener("click", () => postProfile(Number(button.dataset.profile))));
    app.querySelectorAll("[data-theme]").forEach((button) => button.addEventListener("click", () => setTheme(button.dataset.theme)));
    app.querySelectorAll("[data-interval]").forEach((button) => button.addEventListener("click", () => setRefreshInterval(Number(button.dataset.interval))));
    app.querySelector("#wifiRed")?.addEventListener("input", (event) => { state.wifiRed = event.target.value; });
    app.querySelector("#wifiPsw")?.addEventListener("input", (event) => { state.wifiPsw = event.target.value; });
  }

  function escapeHtml(value) {
    return String(value).replace(/&/g, "&amp;").replace(/"/g, "&quot;").replace(/</g, "&lt;").replace(/>/g, "&gt;");
  }

  window.matchMedia("(prefers-color-scheme: dark)").addEventListener("change", () => {
    if (state.theme === "device") {
      applyTheme();
      render();
    }
  });

  render();
  startAutoRefresh();
  loadData();
})();
