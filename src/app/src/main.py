import asyncio
import random
import socket
import concurrent.futures
import time
import requests

import flet as ft


METRIC_CARDS = [
    {"api_key": "TEMPERATURA", "label": "TEMPERATURA", "suffix": " °C"},
    {"api_key": "HUMEDAD", "label": "HUMEDAD", "suffix": " %"},
    {"api_key": "SUELO", "label": "SUELO", "suffix": " %"},
    {"api_key": "ALTURA", "label": "ALTURA", "suffix": " cm"},
    {"api_key": "TANQUE", "label": "TANQUE", "suffix": " %"},
    {"api_key": "TEMP. TANQUE", "label": "TEMP. TANQUE", "suffix": " °C"},
]


class MetricCard:
    def __init__(self, label: str, suffix: str = ""):
        self.label = label
        self.suffix = suffix
        self.value_text = ft.Text("--", size=42, weight=ft.FontWeight.BOLD)

        self.container = ft.Container(
            expand=True,
            border_radius=24,
            bgcolor=ft.Colors.SURFACE_CONTAINER_HIGHEST,
            padding=ft.padding.all(20),
            content=ft.Column(
                spacing=12,
                alignment=ft.MainAxisAlignment.SPACE_BETWEEN,
                expand=True,
                controls=[
                    ft.Text(label, size=16, weight=ft.FontWeight.BOLD),
                    ft.Row(
                        alignment=ft.CrossAxisAlignment.END,
                        controls=[self.value_text],
                    ),
                ],
            ),
        )

    def set_value(self, value: str):
        self.value_text.value = f"{value}{self.suffix}"
        # actualizar sólo el Text control es suficiente
        try:
            self.value_text.update()
        except Exception:
            # si update falla por contexto, forzar redraw del contenedor
            try:
                self.container.update()
            except Exception:
                pass


async def fetch_metrics() -> dict:
    # placeholder kept for compatibility; prefer fetch from real API via fetch_metrics_from_api
    return {
        "TEMPERATURA": f"{random.uniform(18.0, 30.0):.1f}",
        "HUMEDAD": f"{random.uniform(35.0, 75.0):.0f}",
        "SUELO": f"{random.uniform(20.0, 90.0):.0f}",
        "ALTURA": f"{random.uniform(10.0, 120.0):.0f}",
        "TANQUE": f"{random.uniform(0.0, 100.0):.0f}",
        "TEMP. TANQUE": f"{random.uniform(10.0, 35.0):.1f}",
    }


def get_local_ip() -> str:
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.connect(("8.8.8.8", 80))
        ip = s.getsockname()[0]
    except Exception:
        ip = "127.0.0.1"
    finally:
        s.close()
    return ip


def probe_ip(ip: str, port: int = 5000, timeout: float = 0.4) -> str | None:
    url = f"http://{ip}:{port}/data"
    try:
        r = requests.get(url, timeout=timeout)
        if r.status_code == 200:
            return url
    except Exception:
        return None
    return None


def find_api_on_network(port: int = 5000, timeout: float = 0.4, max_workers: int = 40) -> str | None:
    local_ip = get_local_ip()
    if local_ip.startswith("127."):
        return None
    base = ".".join(local_ip.split(".")[:3])
    ips = [f"{base}.{i}" for i in range(1, 255) if f"{base}.{i}" != local_ip]

    with concurrent.futures.ThreadPoolExecutor(max_workers=max_workers) as ex:
        futures = {ex.submit(probe_ip, ip, port, timeout): ip for ip in ips}
        for fut in concurrent.futures.as_completed(futures):
            res = fut.result()
            if res:
                return res
    return None


def fetch_metrics_from_api(api_url: str) -> dict:
    try:
        r = requests.get(api_url, timeout=1.0)
        if r.status_code == 200:
            return r.json()
    except Exception:
        return {}
    return {}


def build_dashboard():
    cards = [MetricCard(item["label"], item["suffix"]) for item in METRIC_CARDS]

    rows = [
        ft.Row(controls=[c.container for c in cards[i : i + 2]], spacing=20, expand=True)
        for i in range(0, len(cards), 2)
    ]

    return rows, cards


async def refresh_metrics(cards: list[MetricCard], api_url: str | None = None):
    while True:
        if api_url:
            values = await asyncio.to_thread(fetch_metrics_from_api, api_url)
        else:
            values = await fetch_metrics()

        for card in cards:
            # use api_key mapping from METRIC_CARDS
            api_key = None
            for m in METRIC_CARDS:
                if m["label"] == card.label:
                    api_key = m.get("api_key")
                    break

            value = None
            if api_key and api_key in values:
                value = values.get(api_key)
            elif card.label in values:
                value = values.get(card.label)

            card.set_value(value if value is not None else "--")
        await asyncio.sleep(1)


async def main(page: ft.Page):
    page.title = "Skell's GreenHouse - Skellent"
    page.theme = ft.Theme(color_scheme_seed=ft.Colors.PURPLE, use_material3=True)
    page.padding = 20
    page.spacing = 20

    page.appbar = ft.AppBar(
        title=ft.Text("Skell's GreenHouse"),
        center_title=True,
        bgcolor=ft.Colors.SURFACE_CONTAINER,
        actions=[ft.IconButton(ft.Icons.HELP, tooltip="Ayuda")],
    )

    # mostrar mensaje mientras buscamos la API en la LAN
    status_text = ft.Text("Buscando API en la red local...", size=16)
    page.add(status_text)
    page.update()

    api_url = await asyncio.to_thread(find_api_on_network)

    if api_url:
        status_text.value = f"API encontrada: {api_url}"
    else:
        status_text.value = "API no encontrada en la LAN — usando datos simulados"
    page.update()

    rows, cards = build_dashboard()
    page.controls.clear()
    page.add(
        ft.Column(
            controls=rows,
            expand=True,
            spacing=20,
        )
    )

    page.update()
    asyncio.create_task(refresh_metrics(cards, api_url))


ft.run(main)