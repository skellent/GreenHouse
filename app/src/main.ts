import { Capacitor } from "@capacitor/core";
import "./styles.css";

type ThemeMode = "device" | "light" | "dark";
type Language = "es" | "en" | "ru" | "de" | "zh";
type Tab = "dashboard" | "camera" | "history" | "settings" | "credits";

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

interface HistoryEntry {
  id: string;
  timestamp: string;
  data: GreenhouseData;
}

interface AppState {
  apiBase: string;
  activeTab: Tab;
  autoRefresh: boolean;
  refreshIntervalMs: number;
  historyEnabled: boolean;
  history: HistoryEntry[];
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
  refreshIntervalMs: "greenhouse.refreshIntervalMs",
  historyEnabled: "greenhouse.historyEnabled",
  history: "greenhouse.history",
};

const translations: Record<Language, Translation> = {
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
    exportHistory: "Exportar",
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
    ultrasonic: "Altura",
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
    exportHistory: "Export",
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
    ultrasonic: "Height",
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
    appName: "GreenHouse",
    appSubtitle: "Автономная теплица с ИИ",
    dashboard: "Мониторинг",
    camera: "Камера",
    history: "История",
    settings: "Настройки",
    credits: "Авторы",
    apiAddress: "Адрес API",
    refresh: "Обновить",
    save: "Сохранить",
    autoRefresh: "Авто",
    refreshInterval: "Интервал",
    every1s: "1 секунда",
    every2s: "2 секунды",
    recordHistory: "Запись истории",
    saveReading: "Сохранить замер",
    historySaved: "Замер сохранен",
    historyEmpty: "Сохраненных замеров пока нет",
    clearHistory: "Очистить",
    exportHistory: "Экспорт",
    samples: "записей",
    latestReadings: "Последние замеры",
    historyOn: "История активна",
    historyOff: "История остановлена",
    historyCleared: "История очищена",
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
    ultrasonic: "Высота",
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
    profile1: "Пустынный",
    profile2: "Тропический",
    profile3: "Альпийский",
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
    appName: "GreenHouse",
    appSubtitle: "Autonomes Gewachshaus mit KI",
    dashboard: "Monitoring",
    camera: "Kamera",
    history: "Historie",
    settings: "Einstellungen",
    credits: "Credits",
    apiAddress: "API-Adresse",
    refresh: "Aktualisieren",
    save: "Speichern",
    autoRefresh: "Auto",
    refreshInterval: "Intervall",
    every1s: "1 Sekunde",
    every2s: "2 Sekunden",
    recordHistory: "Historie aufzeichnen",
    saveReading: "Messung speichern",
    historySaved: "Messung gespeichert",
    historyEmpty: "Noch keine gespeicherten Messungen",
    clearHistory: "Leeren",
    exportHistory: "Exportieren",
    samples: "Eintrage",
    latestReadings: "Letzte Messungen",
    historyOn: "Historie aktiv",
    historyOff: "Historie pausiert",
    historyCleared: "Historie geleert",
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
    ultrasonic: "Höhe",
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
    profile1: "Wuste",
    profile2: "Tropisch",
    profile3: "Alpin",
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
    appName: "GreenHouse",
    appSubtitle: "自主 AI 温室",
    dashboard: "监控",
    camera: "相机",
    history: "历史",
    settings: "设置",
    credits: "鸣谢",
    apiAddress: "API 地址",
    refresh: "刷新",
    save: "保存",
    autoRefresh: "自动",
    refreshInterval: "间隔",
    every1s: "1 秒",
    every2s: "2 秒",
    recordHistory: "记录历史",
    saveReading: "保存读数",
    historySaved: "读数已保存",
    historyEmpty: "暂无保存读数",
    clearHistory: "清空",
    exportHistory: "导出",
    samples: "条记录",
    latestReadings: "最近读数",
    historyOn: "历史记录开启",
    historyOff: "历史记录暂停",
    historyCleared: "历史已清空",
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
    ultrasonic: "高度",
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
    profile1: "沙漠",
    profile2: "热带",
    profile3: "高山",
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

type IconName =
  | "activity"
  | "camera"
  | "clock"
  | "cloud"
  | "database"
  | "download"
  | "droplet"
  | "export"
  | "globe"
  | "history"
  | "image"
  | "leaf"
  | "moon"
  | "refresh"
  | "save"
  | "settings"
  | "spark"
  | "sun"
  | "thermometer"
  | "trash"
  | "wifi"
  | "wind";

const tabs: Array<{ id: Tab; icon: IconName }> = [
  { id: "dashboard", icon: "activity" },
  { id: "camera", icon: "camera" },
  { id: "history", icon: "history" },
  { id: "settings", icon: "settings" },
  { id: "credits", icon: "spark" },
];

function icon(name: IconName, className = "ui-icon"): string {
  const paths: Record<IconName, string> = {
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

  return `<svg class="${className}" viewBox="0 0 24 24" aria-hidden="true">${paths[name]}</svg>`;
}

const appRoot = document.querySelector<HTMLDivElement>("#app");
if (!appRoot) {
  throw new Error("App root not found");
}
const app: HTMLDivElement = appRoot;

let refreshTimer: number | undefined;

function readRefreshInterval(): number {
  const value = Number(localStorage.getItem(storageKeys.refreshIntervalMs));
  return value === 1000 || value === 2000 ? value : 2000;
}

function readHistory(): HistoryEntry[] {
  try {
    const raw = localStorage.getItem(storageKeys.history);
    if (!raw) return [];
    const parsed = JSON.parse(raw) as HistoryEntry[];
    return Array.isArray(parsed) ? parsed.slice(0, 200) : [];
  } catch {
    return [];
  }
}

const state: AppState = {
  apiBase: localStorage.getItem(storageKeys.apiBase) ?? "http://192.168.1.50",
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

function profileLabel(profile: unknown): string {
  const value = typeof profile === "number" ? profile : Number(profile);
  if (value === 1 || value === 2 || value === 3) return t(`profile${value}`);
  return "--";
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
    if (state.historyEnabled) {
      addHistoryEntry(data, false);
    }
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

  const fileName = `greenhouse-${new Date().toISOString().replace(/[:.]/g, "-")}.jpg`;

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

function setRefreshInterval(value: number): void {
  state.refreshIntervalMs = value === 1000 ? 1000 : 2000;
  localStorage.setItem(storageKeys.refreshIntervalMs, String(state.refreshIntervalMs));
  startAutoRefresh();
  render();
}

function setHistoryEnabled(enabled: boolean): void {
  state.historyEnabled = enabled;
  localStorage.setItem(storageKeys.historyEnabled, String(enabled));
  setStatus(enabled ? t("historyOn") : t("historyOff"), enabled ? "ok" : "idle");
  render();
}

function persistHistory(): void {
  localStorage.setItem(storageKeys.history, JSON.stringify(state.history));
}

function addHistoryEntry(data: GreenhouseData | null, notify = true): void {
  if (!data) {
    setStatus(t("never"), "warn");
    render();
    return;
  }

  state.history = [
    {
      id: `${Date.now()}-${Math.random().toString(16).slice(2)}`,
      timestamp: new Date().toISOString(),
      data,
    },
    ...state.history,
  ].slice(0, 200);
  persistHistory();

  if (notify) {
    setStatus(t("historySaved"), "ok");
    render();
  }
}

function clearHistory(): void {
  state.history = [];
  persistHistory();
  setStatus(t("historyCleared"), "ok");
  render();
}

async function exportHistory(): Promise<void> {
  const fileName = `greenhouse-history-${new Date().toISOString().replace(/[:.]/g, "-")}.csv`;
  const content = historyToCsv(state.history);

  if (Capacitor.isNativePlatform()) {
    const { Filesystem, Directory, Encoding } = await import("@capacitor/filesystem");
    await Filesystem.writeFile({
      path: fileName,
      data: content,
      directory: Directory.Documents,
      encoding: Encoding.UTF8,
    });
  } else {
    const blob = new Blob([content], { type: "text/csv;charset=utf-8" });
    const link = document.createElement("a");
    link.href = URL.createObjectURL(blob);
    link.download = fileName;
    link.click();
    URL.revokeObjectURL(link.href);
  }

  setStatus(t("exportHistory"), "ok");
  render();
}

function historyToCsv(entries: HistoryEntry[]): string {
  const headers = [
    "timestamp",
    "temperatura_ambiente",
    "temperatura_agua",
    "humedad_ambiente",
    "humedad_suelo",
    "intensidad_luz",
    "altura",
    "esp32_temperatura",
    "esp32_camStatus",
    "ia_intensidad_uv",
    "ia_riego",
    "ia_ventilacion",
    "ia_perfil_numero",
    "ia_perfil_nombre",
  ];

  const rows = entries.map((entry) => {
    const sensores = entry.data.sensores;
    const esp32 = entry.data.esp32;
    const ia = entry.data.ia;
    return [
      entry.timestamp,
      sensores?.temperatura_ambiente,
      sensores?.temperatura_agua,
      sensores?.humedad_ambiente,
      sensores?.humedad_suelo,
      sensores?.intensidad_luz,
      sensores?.ultrasonido,
      esp32?.temperatura,
      esp32?.camStatus,
      ia?.intensidad_uv,
      ia?.riego,
      ia?.ventilacion,
      ia?.perfil,
      profileLabel(ia?.perfil),
    ];
  });

  return [headers, ...rows]
    .map((row) => row.map(csvCell).join(","))
    .join("\n");
}

function csvCell(value: unknown): string {
  if (value === undefined || value === null) return "";
  const text = String(value).replace(/"/g, '""');
  return /[",\n\r]/.test(text) ? `"${text}"` : text;
}

function startAutoRefresh(): void {
  if (refreshTimer) window.clearInterval(refreshTimer);
  if (!state.autoRefresh) return;
  refreshTimer = window.setInterval(() => {
    if (!state.loadingData) {
      void loadData();
    }
  }, state.refreshIntervalMs);
}

function metricCard(label: string, value: string, tone: string, iconName: IconName): string {
  return `
    <article class="metric metric-${tone}">
      <span>${icon(iconName)}${label}</span>
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
        <div class="button-row">
          <button class="soft-button ${state.historyEnabled ? "active" : ""}" data-action="historyToggle" aria-label="${t("recordHistory")}" title="${t("recordHistory")}">
            ${icon("database")}${state.historyEnabled ? t("historyOn") : t("historyOff")}
          </button>
          <button class="icon-button" data-action="saveReading" aria-label="${t("saveReading")}" title="${t("saveReading")}">${icon("save")}</button>
        </div>
      </div>

      <section class="section-block">
        <div class="section-title">
          <h2>${t("environment")}</h2>
          <button class="icon-button ${state.loadingData ? "is-spinning" : ""}" data-action="refresh" aria-label="${t("refresh")}" title="${t("refresh")}">${icon("refresh")}</button>
        </div>
        <div class="metric-grid">
          ${metricCard(t("ambientTemperature"), valueText(sensores?.temperatura_ambiente, " °C"), "warm", "thermometer")}
          ${metricCard(t("waterTemperature"), valueText(sensores?.temperatura_agua, " °C"), "water", "droplet")}
          ${metricCard(t("ambientHumidity"), valueText(sensores?.humedad_ambiente, " %"), "cool", "cloud")}
          ${metricCard(t("soilHumidity"), valueText(sensores?.humedad_suelo, " %"), "soil", "leaf")}
          ${metricCard(t("lightIntensity"), valueText(sensores?.intensidad_luz, " lx", 0), "light", "sun")}
          ${metricCard(t("ultrasonic"), valueText(sensores?.ultrasonido, " cm"), "neutral", "activity")}
        </div>
      </section>

      <section class="section-block two-column">
        <div>
          <div class="section-title">
            <h2>${t("automation")}</h2>
          </div>
          <div class="signal-list">
            ${signalRow(t("uvIntensity"), valueText(ia?.intensidad_uv, "", 2), "sun")}
            ${signalRow(t("irrigation"), booleanText(ia?.riego), "droplet")}
            ${signalRow(t("ventilation"), booleanText(ia?.ventilacion), "wind")}
            ${signalRow(t("profile"), profileLabel(ia?.perfil), "leaf")}
          </div>
        </div>
        <div>
          <div class="section-title">
            <h2>${t("hardware")}</h2>
          </div>
          <div class="signal-list">
            ${signalRow(t("espTemperature"), valueText(esp32?.temperatura, " °C"), "thermometer")}
            ${signalRow(t("camStatus"), cameraStatusText(esp32?.camStatus), "camera")}
          </div>
        </div>
      </section>
    </section>
  `;
}

function signalRow(label: string, value: string, iconName: IconName): string {
  return `
    <div class="signal-row">
      <span>${icon(iconName)}${label}</span>
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
            <button class="icon-button ${state.loadingPhoto ? "is-spinning" : ""}" data-action="photo" aria-label="${t("capture")}" title="${t("capture")}" ${state.loadingPhoto ? "disabled" : ""}>${icon("camera")}</button>
            <button class="icon-button" data-action="download" aria-label="${t("download")}" title="${t("download")}" ${state.loadingPhoto ? "disabled" : ""}>${icon("download")}</button>
          </div>
        </div>
        <div class="photo-frame">
          ${
            state.photoUrl
              ? `<img src="${state.photoUrl}" alt="${t("photo")}" />`
              : `<div class="photo-empty">${icon("image", "empty-icon")}<strong>${t("noPhoto")}</strong></div>`
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
          <h2>${icon("globe")}${t("apiAddress")}</h2>
        </div>
        <div class="form-line">
          <input id="apiBase" value="${escapeHtml(state.apiBase)}" placeholder="${t("apiPlaceholder")}" inputmode="url" />
          <button class="primary-button" data-action="saveApi">${icon("save")}${t("save")}</button>
        </div>
        <label class="toggle-line">
          <span>${icon("refresh")}${t("autoRefresh")}</span>
          <input type="checkbox" data-action="autoRefresh" ${state.autoRefresh ? "checked" : ""} />
        </label>
        <div class="setting-group">
          <span>${icon("clock")}${t("refreshInterval")}</span>
          <div class="segmented interval-segmented">
            ${([1000, 2000] as const).map((interval) => `
              <button class="${state.refreshIntervalMs === interval ? "active" : ""}" data-interval="${interval}">
                ${interval === 1000 ? t("every1s") : t("every2s")}
              </button>
            `).join("")}
          </div>
        </div>
      </section>

      <section class="section-block">
        <div class="section-title">
          <h2>${icon("leaf")}${t("plantProfile")}</h2>
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
          <h2>${icon("wifi")}${t("wifi")}</h2>
        </div>
        <div class="form-stack">
          <input id="wifiRed" value="${escapeHtml(state.wifiRed)}" placeholder="${t("ssid")}" autocomplete="off" />
          <input id="wifiPsw" value="${escapeHtml(state.wifiPsw)}" placeholder="${t("password")}" type="password" autocomplete="new-password" />
          <button class="primary-button full" data-action="wifi">${icon("wifi")}${t("connectWifi")}</button>
        </div>
      </section>

      <section class="section-block compact">
        <div class="section-title">
          <h2>${icon("database")}${t("history")}</h2>
        </div>
        <label class="toggle-line">
          <span>${t("recordHistory")}</span>
          <input type="checkbox" data-action="historyToggleInput" ${state.historyEnabled ? "checked" : ""} />
        </label>
      </section>

      <section class="section-block compact">
        <div class="section-title">
          <h2>${icon("moon")}${t("theme")}</h2>
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

function renderHistory(): string {
  const entries = state.history;
  return `
    <section class="screen-grid">
      <div class="panel history-summary">
        <div>
          <span class="eyebrow">${t("history")}</span>
          <strong>${entries.length} ${t("samples")}</strong>
        </div>
        <div class="button-row">
          <button class="icon-button" data-action="exportHistory" aria-label="${t("exportHistory")}" title="${t("exportHistory")}" ${entries.length ? "" : "disabled"}>${icon("export")}</button>
          <button class="icon-button danger" data-action="clearHistory" aria-label="${t("clearHistory")}" title="${t("clearHistory")}" ${entries.length ? "" : "disabled"}>${icon("trash")}</button>
        </div>
      </div>

      <section class="section-block">
        <div class="section-title">
          <h2>${icon("history")}${t("latestReadings")}</h2>
          <button class="soft-button" data-action="saveReading">${icon("save")}${t("saveReading")}</button>
        </div>
        <div class="history-list">
          ${entries.length ? entries.slice(0, 40).map(historyCard).join("") : `<div class="empty-state">${icon("database", "empty-icon")}<strong>${t("historyEmpty")}</strong></div>`}
        </div>
      </section>
    </section>
  `;
}

function historyCard(entry: HistoryEntry): string {
  const sensores = entry.data.sensores;
  const ia = entry.data.ia;
  const timestamp = new Date(entry.timestamp).toLocaleString();
  return `
    <article class="history-card">
      <div class="history-card-head">
        <strong>${timestamp}</strong>
        <span>${profileLabel(ia?.perfil)}</span>
      </div>
      <div class="history-values">
        ${historyValue("thermometer", valueText(sensores?.temperatura_ambiente, " °C"))}
        ${historyValue("droplet", valueText(sensores?.humedad_suelo, " %"))}
        ${historyValue("sun", valueText(sensores?.intensidad_luz, " lx", 0))}
        ${historyValue("activity", valueText(ia?.intensidad_uv, "", 2))}
        ${historyValue("droplet", booleanText(ia?.riego))}
        ${historyValue("wind", booleanText(ia?.ventilacion))}
      </div>
    </article>
  `;
}

function historyValue(iconName: IconName, value: string): string {
  return `<span>${icon(iconName)}${value}</span>`;
}

function renderCredits(): string {
  return `
    <section class="credits-screen">
      <div class="credits-box">
        <img src="/assets/icon.svg" alt="" class="credits-icon" />
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
  if (state.activeTab === "history") return renderHistory();
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
          <div class="brand-mark"><img src="/assets/icon.svg" alt="" /></div>
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
            <span>${icon(tab.icon)}</span>
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
  app.querySelectorAll<HTMLButtonElement>("[data-action='saveReading']").forEach((button) => {
    button.addEventListener("click", () => addHistoryEntry(state.data));
  });
  app.querySelector<HTMLButtonElement>("[data-action='historyToggle']")?.addEventListener("click", () => {
    setHistoryEnabled(!state.historyEnabled);
  });
  app.querySelector<HTMLButtonElement>("[data-action='clearHistory']")?.addEventListener("click", clearHistory);
  app.querySelector<HTMLButtonElement>("[data-action='exportHistory']")?.addEventListener("click", exportHistory);

  app.querySelector<HTMLSelectElement>("[data-action='language']")?.addEventListener("change", (event) => {
    setLanguage((event.target as HTMLSelectElement).value as Language);
  });

  app.querySelector<HTMLInputElement>("[data-action='autoRefresh']")?.addEventListener("change", (event) => {
    setAutoRefresh((event.target as HTMLInputElement).checked);
  });
  app.querySelector<HTMLInputElement>("[data-action='historyToggleInput']")?.addEventListener("change", (event) => {
    setHistoryEnabled((event.target as HTMLInputElement).checked);
  });

  app.querySelectorAll<HTMLButtonElement>("[data-profile]").forEach((button) => {
    button.addEventListener("click", () => {
      void postProfile(Number(button.dataset.profile));
    });
  });

  app.querySelectorAll<HTMLButtonElement>("[data-theme]").forEach((button) => {
    button.addEventListener("click", () => setTheme(button.dataset.theme as ThemeMode));
  });
  app.querySelectorAll<HTMLButtonElement>("[data-interval]").forEach((button) => {
    button.addEventListener("click", () => setRefreshInterval(Number(button.dataset.interval)));
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
