# Guía de introducción a nabla-esp-ui

Manual corto en español para entender y empezar a usar nabla-esp-ui.

---

## Qué es nabla-esp-ui (en una frase)

**La plataforma común de pantallas para dispositivos ESP32** (ESPHome + LVGL):
misma filosofía visual y de navegación, mismos componentes reutilizables, y el
**menú / bindings / secretos de cada instalación fuera del repo público**.

No es Home Assistant. No es Nabla Edge. Es la **UI de dispositivo** que puede
hablar con ambos cuando hace falta, y **seguir siendo usable en local** si
Wi-Fi, VPN, Edge o HA no están.

---

## Qué no es

- Un firmware listo para instalar. El consumidor aporta su árbol de menú,
  hardware, credenciales y bindings.
- Un catálogo de instalaciones reales. Los ejemplos usan datos inventados
  (Site A, Site B, redes genéricas).
- Una fuente de secretos funcionales. `!secret wifi_ssid` se resuelve en la
  configuración privada del consumidor.

---

## El mapa Nabla (dónde encaja cada pieza)

| Pieza | Repo | Rol |
| --- | --- | --- |
| **nabla-esp-ui** (este repo) | [txemavs/nabla-esp-ui](https://github.com/txemavs/nabla-esp-ui) | UI de dispositivo ESPHome+LVGL: pantallas, tema, navegación, formularios, perfiles, simulador |
| **Nabla Edge** | [txemavs/nabla-edge](https://github.com/txemavs/nabla-edge) | Pi / router de sitio: uplink, NablaNet AP, Tailscale, servicios del borde |
| **Nabla Net** | [txemavs/nabla-net](https://github.com/txemavs/nabla-net) | Red / conectividad del sitio (contrato y piezas de red Nabla) |
| **Nabla Linux** | [txemavs/nabla-linux](https://github.com/txemavs/nabla-linux) | Imagen/OS Linux de los nodos Nabla |
| **Nabla Inference** | [txemavs/nabla-inference](https://github.com/txemavs/nabla-inference) | Stack opcional de servicios de cómputo/IA para la red (ver detalle abajo) |
| **Home Assistant** | *(instalación del sitio)* | Estados y comandos de entidades del sitio |
| **YAML privado del dispositivo** | *(fuera de GitHub público)* | Árbol de navegación, formularios, Wi-Fi/OTA/API, bindings reales, secretos |

**Nabla Inference** se instala en un PC o nodo con GPU para **ofrecer servicios
de inferencia (LLM Ollama/vLLM, generación de imágenes ComfyUI, voz STT/TTS) al
resto de la red Nabla**. No es la UI ni Home Assistant; es un Compose con
perfiles que publica sus capacidades en `capabilities.json`. Aplicaciones de la
red (o Edge) pueden consumir esas APIs, típicamente por red privada (p.ej.
Tailscale).

Los **ESP32** usan nabla-esp-ui para estandarizar **cómo se ve y se navega**,
sin reinventar la UI en cada dispositivo.

> **Nota:** El portal cautivo compartido vive en
> [txemavs/nabla-esphome-captive](https://github.com/txemavs/nabla-esphome-captive),
> importado como componente externo opcional.
>
> El paraguas de laboratorio está en
> [txemavs/nabla](https://github.com/txemavs/nabla) (índice de experimentos,
> no es un componente de runtime).

---

## Filosofía (lo que no negociamos)

1. **Local primero.** Luces, menús y ajustes del propio dispositivo no esperan
   a un servidor, VPN o HA.
2. **Un menú, varios tamaños.** El mismo árbol YAML se ve como tiles en
   pantalla grande o lista en 128×64; no es «escalar y listo».
3. **Misma interacción.** Todo lo que se hace con touch tiene camino con
   **Arriba / Abajo / Enter** (y Back/Cancel alcanzable).
4. **Significado ≠ transporte.** Un control de luz se pinta igual; el dato
   puede venir de local, MQTT, HA o Edge vía **adaptador**.
5. **Público vs privado.** Al repo público van widgets, tema, navegación,
   adapters genéricos y ejemplos sintéticos. A la instalación privada van
   entidades reales, SSIDs, URLs, secretos y el menú del sitio.

---

## Cómo está organizado el repo

- `packages/` — puntos de entrada públicos (`regular.yaml`, `compact.yaml`).
- `components/` — piezas visuales reutilizables (shell, toolbar, forms, etc.).
- `navigation/` — contrato del árbol, foco, render.
- `profiles/` — regular, portrait, tiny, readable, tft160.
- `external_components/` — esquemas ESPHome y generación C++.
- `adapters/` — puentes presentación ↔ datos (ha-lights, mqtt-lights).
- `hardware/` — placas (JC3248W535CN, NodeMCU-32S/ST7735).
- `simulator/` — ventana SDL en el PC (misma UI YAML, sin placa).
- `examples/` — composiciones de ejemplo (datos inventados).
- `theme/`, `locales/`, `assets/` — estilo visual, traducciones, recursos.
- `docs/` — arquitectura, instalaciones privadas, plan de plataforma.

---

## Cómo probar el simulador

El simulador compila ESPHome para tu ordenador y abre una ventana SDL.
No necesitas placa, Home Assistant ni credenciales.

### Requisitos

Ubuntu 24.04 (o WSL con WSLg), Python 3.12, libsdl2-dev.

### Instalación

```sh
cd nabla-esp-ui
sudo apt-get install -y libsdl2-dev build-essential python3-venv
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

### Abrir la ventana

```sh
./simulator/run.sh              # 480×320 landscape
./simulator/run.sh portrait     # 320×480 portrait
./simulator/run.sh tiny         # 128×64 compacto
./simulator/run.sh readable     # 128×64 legible
```

Navega con las teclas: Arriba/Abajo selecciona, Enter abre, Escape vuelve atrás.
El triángulo en la raíz alterna tiles/lista.

Flujo de prueba: **Ajustes → Conexiones → Wi-Fi** (demo sin efectos de red).

Más detalles en [simulator/README.md](../../simulator/README.md).

---

## Cómo consumir la librería desde GitHub

El dispositivo **posee** su menú. GitHub solo aporta presentación,
controladores, traducciones y assets.

### Pasos

1. Copiar [`examples/github/panel.yaml`](../../examples/github/panel.yaml) a
   Device Builder o tu configuración privada.
2. Fijar `nabla_ui_ref` a un **commit SHA completo** (40 caracteres).
3. Declarar `nabla_navigation.tree` y `forms` en el YAML raíz.
4. Importar hardware por separado.
5. Configurar Wi-Fi, API cifrada, OTA y secretos con `!secret` en la
   instalación.

### Ejemplo mínimo

```yaml
substitutions:
  nabla_ui_ref: <sha-de-40-caracteres>
  nabla_resource_root: https://raw.githubusercontent.com/txemavs/nabla-esp-ui/${nabla_ui_ref}

packages:
  library:
    url: https://github.com/txemavs/nabla-esp-ui
    ref: ${nabla_ui_ref}
    refresh: never
    files:
      - packages/regular.yaml
      - hardware/jc3248w535cn.yaml

wifi:
  networks:
    - ssid: !secret wifi_ssid
      password: !secret wifi_password

ota:
  - platform: esphome
    password: !secret ota_password
```

No uses `main` ni SHAs cortos para instalaciones desplegadas. Cambiar el pin es
una actualización deliberada.

### Documentación normativa (inglés)

- [Library contract v0.1](../LIBRARY-CONTRACT-v0.1.md) — qué ofrece la
  librería, reglas de pin, responsabilidades del consumidor.
- [GitHub library setup](../GITHUB-LIBRARY.md) — consumo remoto detallado.

---

## Dónde van secretos y menú real

Los secretos y el menú de tu instalación **no** viven en este repositorio.

- **Secretos:** en `secrets.yaml` de tu configuración ESPHome privada.
- **Menú:** en el YAML raíz de tu dispositivo, bajo `nabla_navigation.tree`.
- **Bindings reales:** entidades HA, topics MQTT, cámaras, sensores, etc., en
  tu configuración privada.

Nunca importes `examples/hello-world/ui.yaml` en una instalación real: trae un
menú de ejemplo. El menú vive en tu YAML.

Ver [Private installations](../PRIVATE-INSTALLATIONS.md) para la frontera
público/privado y [Private device template](../PRIVATE-DEVICE-TEMPLATE.md)
para el checklist de un dispositivo real.

---

## Estado actual (honestidad)

**Desarrollo activo, pre-1.0.** Baseline probado: ESPHome 2026.8.2; el
renderizador regular usa LVGL 9.5.

| Hito | Estado |
| --- | --- |
| M0 contratos / docs | Hecho |
| M1 perfiles (host) | Hecho (regular / portrait / tiny / readable) |
| M2 formularios (host) | Hecho (Wi-Fi demo, number/choice/toggle) |
| M3 placas físicas | En progreso: JC3248W535CN launcher táctil inicial OK; NodeMCU-32S/ST7735 encoder desplegado. Faltan pruebas de usabilidad prolongadas. |
| M4+ comisión BLE, peers, HA/Edge pleno | Planeado |

Muchas apps del escritorio (Fotos, Música, Cámaras…) son aún **placeholders**
de navegación, no aplicaciones implementadas.

Ver [Roadmap](../platform/ROADMAP.md) para las puertas de salida de cada
milestone.

---

## Cómo comprobar si «es como yo creo»

Si el modelo es correcto, deberías poder decir sí a:

- [ ] Un ESP sin HA/Edge/Wi-Fi sigue navegando su menú local.
- [ ] El mismo árbol se puede mostrar en panel color y en display pequeño.
- [ ] HA y Edge son **adaptadores opcionales**, no el núcleo de la UI.
- [ ] Secretos y entidades reales **no** viven en este repo público.
- [ ] Actualizar la librería es cambiar un pin de commit y validar; no mezcla
      menús de ejemplo en el dispositivo.

---

## Referencias rápidas (inglés)

- [README.md](../../README.md) — entrada del proyecto
- [AGENTS.md](../../AGENTS.md) — reglas para contribuidores
- [ARCHITECTURE.md](../ARCHITECTURE.md) — límite de implementación
- [ROADMAP.md](../platform/ROADMAP.md) — milestones y puertas de salida
- [simulator/README.md](../../simulator/README.md) — guía completa del simulador
