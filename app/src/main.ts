import { Capacitor } from "@capacitor/core";
import "./styles.css";

type ThemeMode = "device" | "light" | "dark";
type Language = "es" | "en" | "ru" | "de" | "zh";
type Tab = "dashboard" | "camera" | "settings" | "credits";

interface GreenhouseData {
  sensores?: {
    temperatura_ambiente?: number;
    temperatura_agua?: number;
    humedad_ambiente?: number;
    humedad_suelo?: number;
    intensidad_luz?: number;
    ultrasonido?: number;
  };
  esp32?: {
    temperatura?: number;
    camStatus?: boolean | number | string;
  };
  ia?: {
    intensidad_uv?: number;
    riego?: boolean | number | string;
    ventilacion?: boolean | number | string;
    perfil?: number;
  };
  error?: string;
}

interface AppState {
  apiBase: string;
  activeTab: Tab;
  autoRefresh: boolean;
  data: GreenhouseData | null;
  lastUpdate: Date | null;
  loadingData: boolean;
  loadingPhoto: boolean;
  photoUrl: string | null;
  photoBlob: Blob | null;
  status: string;
  statusTone: "idle" | "ok" | "warn" | "error";
  theme: ThemeMode;
  language: Language;
  wifiRed: string;
  wifiPsw: string;
}

type Translation = Record<string, string>;

const storageKeys = {
  apiBase: "greenhouse.apiBase",
  theme: "greenhouse.theme",
  language: "greenhouse.language",
  autoRefresh: "greenhouse.autoRefresh",
};

const translations: Record<Language, Translation> = {
  es: {
    appName: "Skell's GreenHouse",
    appSubtitle: "Invernadero autonomo con IA",
    dashboard: "Monitoreo",
    camera: "Camara",
    settings: "Configurar",
    credits: "Creditos",
    apiAddress: "Direccion API",
    refresh: "Actualizar",
    save: "Guardar",
    autoRefresh: "Auto",
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
    ultrasonic: "Ultrasonido",
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
    profile1: "Perfil 1",
    profile2: "Perfil 2",
    profile3: "Perfil 3",
    applyProfile: "Aplicar perfil",
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
    nativeMode: "Android nativo",
    webMode: "Vista web",
    apiPlaceholder: "http://192.168.1.50",
    creditsTitle: "SKELL'S GREENHOUSE V3.0",
    developedBy: "Developed By:",
    inferenceBusy: "Inferencia en progreso",
    invalidApi: "Introduce una direccion valida",
  },
  en: {
    appName: "Skell's GreenHouse",
    appSubtitle: "Autonomous AI greenhouse",
    dashboard: "Monitor",
    camera: "Camera",
    settings: "Settings",
    credits: "Credits",
    apiAddress: "API address",
    refresh: "Refresh",
    save: "Save",
    autoRefresh: "Auto",
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
    ultrasonic: "Ultrasonic",
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
    profile1: "Profile 1",
    profile2: "Profile 2",
    profile3: "Profile 3",
    applyProfile: "Apply profile",
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
    nativeMode: "Native Android",
    webMode: "Web view",
    apiPlaceholder: "http://192.168.1.50",
    creditsTitle: "SKELL'S GREENHOUSE V3.0",
    developedBy: "Developed By:",
    inferenceBusy: "Inference in progress",
    invalidApi: "Enter a valid address",
  },
  ru: {
    appName: "Skell's GreenHouse",
    appSubtitle: "Автономная теплица с ИИ",
    dashboard: "Мониторинг",
    camera: "Камера",
    settings: "Настройки",
    credits: "Авторы",
    apiAddress: "Адрес API",
    refresh: "Обновить",
    save: "Сохранить",
    autoRefresh: "Авто",
    connected: "Данные обновлены",
    loading: "Связь с теплицей...",
    offline: "Нет связи с API",
    ready: "Готово",
    lastUpdate: "Последние данные",
    never: "Нет данных",
    environment: "Среда",
    automation: "ИИ и приводы",
    hardware: "ESP32",
    ambientTemperature: "Температура воздуха",
    waterTemperature: "Температура воды",
    ambientHumidity: "Влажность воздуха",
    soilHumidity: "Влажность почвы",
    lightIntensity: "Освещенность",
    ultrasonic: "Ультразвук",
    espTemperature: "Температура ESP32",
    camStatus: "Состояние камеры",
    uvIntensity: "Интенсивность УФ",
    irrigation: "Полив",
    ventilation: "Вентиляция",
    profile: "Профиль",
    active: "Активно",
    inactive: "Неактивно",
    unknown: "Неизвестно",
    online: "Онлайн",
    offlineShort: "Откл.",
    photo: "Фото",
    capture: "Снимок",
    download: "Скачать",
    noPhoto: "Нет фото",
    photoSaved: "Фото скачано",
    photoError: "Не удалось получить фото",
    plantProfile: "Профиль растения",
    profile1: "Профиль 1",
    profile2: "Профиль 2",
    profile3: "Профиль 3",
    applyProfile: "Применить",
    wifi: "WiFi",
    ssid: "Сеть",
    password: "Пароль",
    connectWifi: "Переподключить",
    wifiSent: "Команда отправлена. Подождите ~15 с.",
    profileSaved: "Профиль обновлен",
    theme: "Тема",
    light: "Светлая",
    dark: "Темная",
    device: "Система",
    language: "Язык",
    nativeMode: "Android",
    webMode: "WebView",
    apiPlaceholder: "http://192.168.1.50",
    creditsTitle: "SKELL'S GREENHOUSE V3.0",
    developedBy: "Developed By:",
    inferenceBusy: "Идет вывод ИИ",
    invalidApi: "Введите корректный адрес",
  },
  de: {
    appName: "Skell's GreenHouse",
    appSubtitle: "Autonomes Gewachshaus mit KI",
    dashboard: "Monitoring",
    camera: "Kamera",
    settings: "Einstellungen",
    credits: "Credits",
    apiAddress: "API-Adresse",
    refresh: "Aktualisieren",
    save: "Speichern",
    autoRefresh: "Auto",
    connected: "Daten aktualisiert",
    loading: "Gewachshaus wird abgefragt...",
    offline: "Keine Verbindung zur API",
    ready: "Bereit",
    lastUpdate: "Letzte Messung",
    never: "Keine Messungen",
    environment: "Umgebung",
    automation: "KI und Aktoren",
    hardware: "ESP32",
    ambientTemperature: "Lufttemperatur",
    waterTemperature: "Wassertemperatur",
    ambientHumidity: "Luftfeuchte",
    soilHumidity: "Bodenfeuchte",
    lightIntensity: "Lichtintensitat",
    ultrasonic: "Ultraschall",
    espTemperature: "ESP32-Temperatur",
    camStatus: "Kamerastatus",
    uvIntensity: "UV-Intensitat",
    irrigation: "Bewasserung",
    ventilation: "Beluftung",
    profile: "Profil",
    active: "Aktiv",
    inactive: "Inaktiv",
    unknown: "Unbekannt",
    online: "Online",
    offlineShort: "Offline",
    photo: "Foto",
    capture: "Aufnehmen",
    download: "Herunterladen",
    noPhoto: "Kein Foto",
    photoSaved: "Foto gespeichert",
    photoError: "Foto konnte nicht geladen werden",
    plantProfile: "Pflanzenprofil",
    profile1: "Profil 1",
    profile2: "Profil 2",
    profile3: "Profil 3",
    applyProfile: "Profil anwenden",
    profileSaved: "Profil aktualisiert",
    wifi: "WiFi",
    ssid: "Netzwerk",
    password: "Passwort",
    connectWifi: "Neu verbinden",
    wifiSent: "Auftrag gesendet. Warte etwa 15 s.",
    theme: "Theme",
    light: "Hell",
    dark: "Dunkel",
    device: "System",
    language: "Sprache",
    nativeMode: "Android nativ",
    webMode: "WebView",
    apiPlaceholder: "http://192.168.1.50",
    creditsTitle: "SKELL'S GREENHOUSE V3.0",
    developedBy: "Developed By:",
    inferenceBusy: "Inferenz lauft",
    invalidApi: "Gultige Adresse eingeben",
  },
  zh: {
    appName: "Skell's GreenHouse",
    appSubtitle: "自主 AI 温室",
    dashboard: "监控",
    camera: "相机",
    settings: "设置",
    credits: "鸣谢",
    apiAddress: "API 地址",
    refresh: "刷新",
    save: "保存",
    autoRefresh: "自动",
    connected: "数据已更新",
    loading: "正在连接温室...",
    offline: "无法连接 API",
    ready: "就绪",
    lastUpdate: "最近读数",
    never: "无读数",
    environment: "环境",
    automation: "AI 与执行器",
    hardware: "ESP32",
    ambientTemperature: "环境温度",
    waterTemperature: "水温",
    ambientHumidity: "环境湿度",
    soilHumidity: "土壤湿度",
    lightIntensity: "光照强度",
    ultrasonic: "超声波",
    espTemperature: "ESP32 温度",
    camStatus: "相机状态",
    uvIntensity: "UV 强度",
    irrigation: "灌溉",
    ventilation: "通风",
    profile: "配置",
    active: "开启",
    inactive: "关闭",
    unknown: "未知",
    online: "在线",
    offlineShort: "离线",
    photo: "照片",
    capture: "拍摄",
    download: "下载",
    noPhoto: "无照片",
    photoSaved: "照片已下载",
    photoError: "无法获取照片",
    plantProfile: "植物配置",
    profile1: "配置 1",
    profile2: "配置 2",
    profile3: "配置 3",
    applyProfile: "应用配置",
    profileSaved: "配置已更新",
    wifi: "WiFi",
    ssid: "网络",
    password: "密码",
    connectWifi: "重新连接",
    wifiSent: "指令已发送。请等待约 15 秒。",
    theme: "主题",
    light: "浅色",
    dark: "深色",
    device: "系统",
    language: "语言",
    nativeMode: "原生 Android",
    webMode: "网页视图",
    apiPlaceholder: "http://192.168.1.50",
    creditsTitle: "SKELL'S GREENHOUSE V3.0",
    developedBy: "Developed By:",
    inferenceBusy: "正在推理",
    invalidApi: "请输入有效地址",
  },
};

const languageNames: Record<Language, string> = {
  es: "Español",
  en: "English",
  ru: "Русский",
  de: "Deutsch",
  zh: "中文",
};

const tabs: Array<{ id: Tab; icon: string }> = [
  { id: "dashboard", icon: "◌" },
  { id: "camera", icon: "▣" },
  { id: "settings", icon: "⚙" },
  { id: "credits", icon: "☆" },
];

const appRoot = document.querySelector<HTMLDivElement>("#app");
if (!appRoot) {
  throw new Error("App root not found");
}
const app: HTMLDivElement = appRoot;

let refreshTimer: number | undefined;

const state: AppState = {
  apiBase: localStorage.getItem(storageKeys.apiBase) ?? "http://192.168.1.50",
  activeTab: "dashboard",
  autoRefresh: localStorage.getItem(storageKeys.autoRefresh) !== "false",
  data: null,
  lastUpdate: null,
  loadingData: false,
  loadingPhoto: false,
  photoUrl: null,
  photoBlob: null,
  status: translations.es.ready ?? "Ready",
  statusTone: "idle",
  theme: readTheme(),
  language: readLanguage(),
  wifiRed: "",
  wifiPsw: "",
};

function readTheme(): ThemeMode {
  const value = localStorage.getItem(storageKeys.theme);
  return value === "light" || value === "dark" || value === "device" ? value : "device";
}

function readLanguage(): Language {
  const value = localStorage.getItem(storageKeys.language);
  return value === "es" || value === "en" || value === "ru" || value === "de" || value === "zh"
    ? value
    : "es";
}

function t(key: string): string {
  return translations[state.language][key] ?? translations.es[key] ?? key;
}

function normalizeBaseUrl(value: string): string {
  const trimmed = value.trim();
  if (!trimmed) return "";
  const withProtocol = /^https?:\/\//i.test(trimmed) ? trimmed : `http://${trimmed}`;
  return withProtocol.replace(/\/+$/, "");
}

function applyTheme(): void {
  const deviceDark = window.matchMedia("(prefers-color-scheme: dark)").matches;
  const resolved = state.theme === "device" ? (deviceDark ? "dark" : "light") : state.theme;
  document.documentElement.dataset.theme = resolved;
  document.documentElement.dataset.themeMode = state.theme;
  document.documentElement.lang = state.language;
}

function setStatus(message: string, tone: AppState["statusTone"] = "idle"): void {
  state.status = message;
  state.statusTone = tone;
}

function valueText(value: unknown, unit = "", fractionDigits = 1): string {
  if (typeof value !== "number" || Number.isNaN(value)) return "--";
  const formatted = Number.isInteger(value) ? String(value) : value.toFixed(fractionDigits);
  return `${formatted}${unit}`;
}

function booleanText(value: unknown): string {
  if (value === true || value === 1 || value === "1" || value === "true" || value === "on") return t("active");
  if (value === false || value === 0 || value === "0" || value === "false" || value === "off") return t("inactive");
  return t("unknown");
}

function cameraStatusText(value: unknown): string {
  if (value === true || value === 1 || value === "1" || value === "true" || value === "on") return t("online");
  if (value === false || value === 0 || value === "0" || value === "false" || value === "off") return t("offlineShort");
  if (value === undefined || value === null || value === "") return t("unknown");
  return String(value);
}

function friendlyError(error: unknown): string {
  const message = error instanceof Error ? error.message : String(error || "");
  const normalized = message.toLowerCase();
  if (
    normalized.includes("failed to fetch") ||
    normalized.includes("load failed") ||
    normalized.includes("networkerror") ||
    normalized.includes("aborted")
  ) {
    return t("offline");
  }
  if (message.includes("Inferencia")) {
    return t("inferenceBusy");
  }
  return message || t("offline");
}

function apiUrl(path: string): string {
  return `${state.apiBase}${path}`;
}

async function readError(response: Response): Promise<string> {
  try {
    const body = await response.json() as { error?: string; mensaje?: string };
    return body.error ?? body.mensaje ?? response.statusText;
  } catch {
    return response.statusText;
  }
}

async function fetchWithTimeout(url: string, init: RequestInit = {}, timeoutMs = 8000): Promise<Response> {
  const controller = new AbortController();
  const timeout = window.setTimeout(() => controller.abort(), timeoutMs);
  try {
    return await fetch(url, {
      ...init,
      signal: controller.signal,
    });
  } finally {
    window.clearTimeout(timeout);
  }
}

async function loadData(): Promise<void> {
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
    const response = await fetchWithTimeout(apiUrl("/api/datos"), {
      headers: { Accept: "application/json" },
    });

    if (!response.ok) {
      const error = await readError(response);
      throw new Error(error || t("offline"));
    }

    const data = await response.json() as GreenhouseData;
    if (data.error) {
      throw new Error(data.error);
    }

    state.data = data;
    state.lastUpdate = new Date();
    setStatus(t("connected"), "ok");
  } catch (error) {
    setStatus(friendlyError(error), "error");
  } finally {
    state.loadingData = false;
    render();
  }
}

async function refreshPhoto(downloadAfterFetch = false): Promise<void> {
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
    const response = await fetchWithTimeout(apiUrl("/api/foto"), {
      headers: { Accept: "image/jpeg" },
    }, 12000);

    if (!response.ok) {
      throw new Error(await response.text());
    }

    const blob = await response.blob();
    if (state.photoUrl) URL.revokeObjectURL(state.photoUrl);
    state.photoBlob = blob;
    state.photoUrl = URL.createObjectURL(blob);
    setStatus(t("connected"), "ok");

    if (downloadAfterFetch) {
      await downloadPhoto();
    }
  } catch {
    setStatus(t("photoError"), "error");
  } finally {
    state.loadingPhoto = false;
    render();
  }
}

async function downloadPhoto(): Promise<void> {
  if (!state.photoBlob) {
    await refreshPhoto(true);
    return;
  }

  const fileName = `skells-greenhouse-${new Date().toISOString().replace(/[:.]/g, "-")}.jpg`;

  if (Capacitor.isNativePlatform()) {
    const { Filesystem, Directory } = await import("@capacitor/filesystem");
    const data = await blobToBase64(state.photoBlob);
    await Filesystem.writeFile({
      path: fileName,
      data,
      directory: Directory.Documents,
    });
  } else {
    const link = document.createElement("a");
    link.href = URL.createObjectURL(state.photoBlob);
    link.download = fileName;
    link.click();
    URL.revokeObjectURL(link.href);
  }

  setStatus(t("photoSaved"), "ok");
  render();
}

function blobToBase64(blob: Blob): Promise<string> {
  return new Promise((resolve, reject) => {
    const reader = new FileReader();
    reader.onload = () => {
      const result = String(reader.result);
      resolve(result.includes(",") ? result.split(",")[1] ?? result : result);
    };
    reader.onerror = () => reject(reader.error);
    reader.readAsDataURL(blob);
  });
}

async function postProfile(profile: number): Promise<void> {
  state.loadingData = true;
  setStatus(t("loading"), "idle");
  render();

  try {
    const response = await fetchWithTimeout(apiUrl("/api/perfil"), {
      method: "POST",
      headers: { "Content-Type": "text/plain" },
      body: String(profile),
    });

    if (!response.ok) {
      throw new Error(await readError(response));
    }

    const body = await response.json() as { perfil?: number };
    state.data = {
      ...state.data,
      ia: {
        ...state.data?.ia,
        perfil: body.perfil ?? profile,
      },
    };
    setStatus(t("profileSaved"), "ok");
  } catch (error) {
    setStatus(friendlyError(error), "error");
  } finally {
    state.loadingData = false;
    render();
  }
}

async function postWifi(): Promise<void> {
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
      headers: {
        "Content-Type": "application/json",
        Accept: "application/json",
      },
      body: JSON.stringify({ red: state.wifiRed.trim(), psw: state.wifiPsw }),
    });

    if (!response.ok) {
      throw new Error(await readError(response));
    }

    setStatus(t("wifiSent"), "ok");
    state.wifiPsw = "";
  } catch (error) {
    setStatus(friendlyError(error), "error");
  } finally {
    state.loadingData = false;
    render();
  }
}

function saveApiBase(): void {
  const normalized = normalizeBaseUrl((document.querySelector<HTMLInputElement>("#apiBase")?.value ?? state.apiBase));
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

function setTheme(theme: ThemeMode): void {
  state.theme = theme;
  localStorage.setItem(storageKeys.theme, theme);
  applyTheme();
  render();
}

function setLanguage(language: Language): void {
  state.language = language;
  localStorage.setItem(storageKeys.language, language);
  applyTheme();
  setStatus(t("ready"), "idle");
  render();
}

function setAutoRefresh(enabled: boolean): void {
  state.autoRefresh = enabled;
  localStorage.setItem(storageKeys.autoRefresh, String(enabled));
  startAutoRefresh();
  render();
}

function startAutoRefresh(): void {
  if (refreshTimer) window.clearInterval(refreshTimer);
  if (!state.autoRefresh) return;
  refreshTimer = window.setInterval(() => {
    if (!state.loadingData && state.activeTab === "dashboard") {
      void loadData();
    }
  }, 10000);
}

function metricCard(label: string, value: string, tone = "neutral"): string {
  return `
    <article class="metric metric-${tone}">
      <span>${label}</span>
      <strong>${value}</strong>
    </article>
  `;
}

function renderDashboard(): string {
  const sensores = state.data?.sensores;
  const esp32 = state.data?.esp32;
  const ia = state.data?.ia;
  const last = state.lastUpdate ? state.lastUpdate.toLocaleTimeString() : t("never");

  return `
    <section class="screen-grid">
      <div class="panel status-panel">
        <div>
          <span class="eyebrow">${t("lastUpdate")}</span>
          <strong>${last}</strong>
        </div>
        <span class="mode-pill">${Capacitor.isNativePlatform() ? t("nativeMode") : t("webMode")}</span>
      </div>

      <section class="section-block">
        <div class="section-title">
          <h2>${t("environment")}</h2>
          <button class="icon-button" data-action="refresh" aria-label="${t("refresh")}" title="${t("refresh")}">↻</button>
        </div>
        <div class="metric-grid">
          ${metricCard(t("ambientTemperature"), valueText(sensores?.temperatura_ambiente, " °C"), "warm")}
          ${metricCard(t("waterTemperature"), valueText(sensores?.temperatura_agua, " °C"), "water")}
          ${metricCard(t("ambientHumidity"), valueText(sensores?.humedad_ambiente, " %"), "cool")}
          ${metricCard(t("soilHumidity"), valueText(sensores?.humedad_suelo, " %"), "soil")}
          ${metricCard(t("lightIntensity"), valueText(sensores?.intensidad_luz, " lx", 0), "light")}
          ${metricCard(t("ultrasonic"), valueText(sensores?.ultrasonido, " cm"), "neutral")}
        </div>
      </section>

      <section class="section-block two-column">
        <div>
          <div class="section-title">
            <h2>${t("automation")}</h2>
          </div>
          <div class="signal-list">
            ${signalRow(t("uvIntensity"), valueText(ia?.intensidad_uv, "", 2))}
            ${signalRow(t("irrigation"), booleanText(ia?.riego))}
            ${signalRow(t("ventilation"), booleanText(ia?.ventilacion))}
            ${signalRow(t("profile"), ia?.perfil ? `${t("profile")} ${ia.perfil}` : "--")}
          </div>
        </div>
        <div>
          <div class="section-title">
            <h2>${t("hardware")}</h2>
          </div>
          <div class="signal-list">
            ${signalRow(t("espTemperature"), valueText(esp32?.temperatura, " °C"))}
            ${signalRow(t("camStatus"), cameraStatusText(esp32?.camStatus))}
          </div>
        </div>
      </section>
    </section>
  `;
}

function signalRow(label: string, value: string): string {
  return `
    <div class="signal-row">
      <span>${label}</span>
      <strong>${value}</strong>
    </div>
  `;
}

function renderCamera(): string {
  return `
    <section class="screen-grid">
      <section class="section-block">
        <div class="section-title">
          <h2>${t("photo")}</h2>
          <div class="button-row">
            <button class="icon-button" data-action="photo" aria-label="${t("capture")}" title="${t("capture")}" ${state.loadingPhoto ? "disabled" : ""}>▣</button>
            <button class="icon-button" data-action="download" aria-label="${t("download")}" title="${t("download")}" ${state.loadingPhoto ? "disabled" : ""}>⇩</button>
          </div>
        </div>
        <div class="photo-frame">
          ${
            state.photoUrl
              ? `<img src="${state.photoUrl}" alt="${t("photo")}" />`
              : `<div class="photo-empty"><span>▣</span><strong>${t("noPhoto")}</strong></div>`
          }
        </div>
      </section>
    </section>
  `;
}

function renderSettings(): string {
  const currentProfile = state.data?.ia?.perfil ?? 1;
  return `
    <section class="screen-grid">
      <section class="section-block">
        <div class="section-title">
          <h2>${t("apiAddress")}</h2>
        </div>
        <div class="form-line">
          <input id="apiBase" value="${escapeHtml(state.apiBase)}" placeholder="${t("apiPlaceholder")}" inputmode="url" />
          <button class="primary-button" data-action="saveApi">${t("save")}</button>
        </div>
        <label class="toggle-line">
          <span>${t("autoRefresh")}</span>
          <input type="checkbox" data-action="autoRefresh" ${state.autoRefresh ? "checked" : ""} />
        </label>
      </section>

      <section class="section-block">
        <div class="section-title">
          <h2>${t("plantProfile")}</h2>
        </div>
        <div class="segmented profile-segmented">
          ${[1, 2, 3].map((profile) => `
            <button class="${currentProfile === profile ? "active" : ""}" data-profile="${profile}">
              ${t(`profile${profile}`)}
            </button>
          `).join("")}
        </div>
      </section>

      <section class="section-block">
        <div class="section-title">
          <h2>${t("wifi")}</h2>
        </div>
        <div class="form-stack">
          <input id="wifiRed" value="${escapeHtml(state.wifiRed)}" placeholder="${t("ssid")}" autocomplete="off" />
          <input id="wifiPsw" value="${escapeHtml(state.wifiPsw)}" placeholder="${t("password")}" type="password" autocomplete="new-password" />
          <button class="primary-button full" data-action="wifi">${t("connectWifi")}</button>
        </div>
      </section>

      <section class="section-block compact">
        <div class="section-title">
          <h2>${t("theme")}</h2>
        </div>
        <div class="segmented">
          ${(["device", "light", "dark"] as ThemeMode[]).map((theme) => `
            <button class="${state.theme === theme ? "active" : ""}" data-theme="${theme}">${t(theme)}</button>
          `).join("")}
        </div>
      </section>
    </section>
  `;
}

function renderCredits(): string {
  return `
    <section class="credits-screen">
      <div class="credits-box">
        <span class="credits-label">${t("creditsTitle")}</span>
        <strong>${t("developedBy")}</strong>
        <p>Robert Rodríguez "Skellent"</p>
        <p>Christopher Ramirez</p>
        <p>Fabiana Hernandez</p>
      </div>
    </section>
  `;
}

function renderMain(): string {
  if (state.activeTab === "camera") return renderCamera();
  if (state.activeTab === "settings") return renderSettings();
  if (state.activeTab === "credits") return renderCredits();
  return renderDashboard();
}

function render(): void {
  applyTheme();
  app.innerHTML = `
    <div class="app-shell">
      <header class="topbar">
        <div class="brand-lockup">
          <div class="brand-mark">SG</div>
          <div>
            <h1>${t("appName")}</h1>
            <p>${t("appSubtitle")}</p>
          </div>
        </div>
        <select class="language-select" data-action="language" aria-label="${t("language")}">
          ${(Object.keys(languageNames) as Language[]).map((language) => `
            <option value="${language}" ${state.language === language ? "selected" : ""}>${languageNames[language]}</option>
          `).join("")}
        </select>
      </header>

      <main>
        <section class="connection-bar">
          <div class="status-dot status-${state.statusTone}"></div>
          <span>${state.loadingData || state.loadingPhoto ? t("loading") : state.status}</span>
        </section>
        ${renderMain()}
      </main>

      <nav class="tabbar" aria-label="App">
        ${tabs.map((tab) => `
          <button class="${state.activeTab === tab.id ? "active" : ""}" data-tab="${tab.id}" aria-label="${t(tab.id)}" title="${t(tab.id)}">
            <span>${tab.icon}</span>
            <strong>${t(tab.id)}</strong>
          </button>
        `).join("")}
      </nav>
    </div>
  `;
  bindEvents();
}

function bindEvents(): void {
  app.querySelectorAll<HTMLButtonElement>("[data-tab]").forEach((button) => {
    button.addEventListener("click", () => {
      state.activeTab = button.dataset.tab as Tab;
      render();
      if (state.activeTab === "camera" && !state.photoUrl) {
        void refreshPhoto();
      }
    });
  });

  app.querySelector<HTMLButtonElement>("[data-action='refresh']")?.addEventListener("click", () => void loadData());
  app.querySelector<HTMLButtonElement>("[data-action='photo']")?.addEventListener("click", () => void refreshPhoto());
  app.querySelector<HTMLButtonElement>("[data-action='download']")?.addEventListener("click", () => void downloadPhoto());
  app.querySelector<HTMLButtonElement>("[data-action='saveApi']")?.addEventListener("click", saveApiBase);
  app.querySelector<HTMLButtonElement>("[data-action='wifi']")?.addEventListener("click", () => void postWifi());

  app.querySelector<HTMLSelectElement>("[data-action='language']")?.addEventListener("change", (event) => {
    setLanguage((event.target as HTMLSelectElement).value as Language);
  });

  app.querySelector<HTMLInputElement>("[data-action='autoRefresh']")?.addEventListener("change", (event) => {
    setAutoRefresh((event.target as HTMLInputElement).checked);
  });

  app.querySelectorAll<HTMLButtonElement>("[data-profile]").forEach((button) => {
    button.addEventListener("click", () => {
      void postProfile(Number(button.dataset.profile));
    });
  });

  app.querySelectorAll<HTMLButtonElement>("[data-theme]").forEach((button) => {
    button.addEventListener("click", () => setTheme(button.dataset.theme as ThemeMode));
  });

  app.querySelector<HTMLInputElement>("#wifiRed")?.addEventListener("input", (event) => {
    state.wifiRed = (event.target as HTMLInputElement).value;
  });
  app.querySelector<HTMLInputElement>("#wifiPsw")?.addEventListener("input", (event) => {
    state.wifiPsw = (event.target as HTMLInputElement).value;
  });
}

function escapeHtml(value: string): string {
  return value
    .replace(/&/g, "&amp;")
    .replace(/"/g, "&quot;")
    .replace(/</g, "&lt;")
    .replace(/>/g, "&gt;");
}

window.matchMedia("(prefers-color-scheme: dark)").addEventListener("change", () => {
  if (state.theme === "device") {
    applyTheme();
    render();
  }
});

render();
startAutoRefresh();
void loadData();
