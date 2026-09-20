# Contrato HTTP del mirror de pantalla

Versión: Fase 1 (2026-09-20). Relacionado: [issue #34](https://github.com/txemavs/nabla-esp-ui/issues/34).

El mirror de pantalla expone el contenido del framebuffer lógico por HTTP para
visualización remota y entrada opcional de encoder. Home Assistant u otros
consumidores convierten los frames fuera del dispositivo; el ESP sirve bytes
crudos sin codificación JPEG.

## Perfiles de referencia

| Perfil | Dimensiones | Formato | Tamaño frame | Entrada |
|--------|-------------|---------|--------------|---------|
| Kit1 (ST7735) | 160×128 | `rgb332` | 20.480 bytes | encoder |
| T-Call (SSD1309) | 128×64 | `mono1` | 1.024 bytes | encoder |

Estas son las dos configuraciones de hardware verificadas usadas para congelar
este contrato. Otros tamaños (LVGL 240×240, 480×320) siguen las mismas reglas
de formato.

## Endpoints

Todos los endpoints son relativos a la raíz HTTP del dispositivo
(ej. `http://<ip-dispositivo>/`).

### `GET /mirror/capabilities`

Devuelve JSON describiendo la configuración de pantalla y entrada.

**Respuesta**: `200 OK`, `Content-Type: application/json`

```json
{
  "width": 160,
  "height": 128,
  "format": "rgb332",
  "input": true
}
```

| Campo | Tipo | Descripción |
|-------|------|-------------|
| `width` | entero | Ancho lógico de pantalla en píxeles |
| `height` | entero | Alto lógico de pantalla en píxeles |
| `format` | cadena | Formato de píxel: `"mono1"` o `"rgb332"` |
| `input` | booleano | Si se acepta entrada de encoder |

Extensiones futuras pueden añadir `touch`, `encoder`, `token_required` u otros
flags de capacidad. Los consumidores deben ignorar campos desconocidos.

### `GET /mirror/frame`

Devuelve el contenido crudo del framebuffer.

**Respuesta**: `200 OK`, `Content-Type: application/octet-stream`  
**Cabeceras**: `Cache-Control: no-store`

**Respuestas de error**:
- `409 Conflict`: Frame aún no listo o fallo de asignación de memoria

#### Disposición de bytes

**`mono1` (monocromo)**:
- Tamaño: `ceil(width * height / 8)` bytes
- Orden de bits: MSB primero, por filas
- Píxel 0 es bit 7 del byte 0; píxel 7 es bit 0 del byte 0
- Píxel blanco = bit activo (1); píxel negro = bit inactivo (0)

Para pantalla 128×64: 128 × 64 / 8 = 1.024 bytes.

```
Byte 0: [px0 px1 px2 px3 px4 px5 px6 px7]  (MSB a LSB)
Byte 1: [px8 px9 px10 px11 px12 px13 px14 px15]
...
```

**`rgb332` (color 8 bits)**:
- Tamaño: `width * height` bytes (un byte por píxel)
- Disposición de byte: `RRRGGGBB` (3 bits rojo, 3 bits verde, 2 bits azul)
- Orden por filas, de arriba-izquierda a abajo-derecha

Para pantalla 160×128: 160 × 128 = 20.480 bytes.

```
bits[7:5] = rojo  (0-7 → 0-255 como r * 255 / 7)
bits[4:2] = verde (0-7 → 0-255 como g * 255 / 7)
bits[1:0] = azul  (0-3 → 0-255 como b * 255 / 3)
```

### `GET /mirror/token`

Devuelve un token CSRF por arranque requerido para acciones de entrada.

**Respuesta**: `200 OK`, `Content-Type: text/plain`  
**Cabeceras**: `Cache-Control: no-store`

El token es una cadena hexadecimal de 16 caracteres, regenerada en cada
arranque del dispositivo.

### `POST /mirror/action`

Envía una acción de encoder/botón al controlador del dispositivo.

**Petición**:
- Cabecera: `X-Nabla-Token: <token>` (de `/mirror/token`)
- Cuerpo: `action=<valor>` (form-urlencoded)

**Acciones permitidas**: `up`, `down`, `enter`, `back`

**Respuesta**:
- `200 OK`: Acción encolada
- `400 Bad Request`: Acción faltante o inválida
- `401 Unauthorized`: Token faltante o inválido
- `404 Not Found`: Entrada deshabilitada (sin `on_action` configurado)
- `409 Conflict`: Acción previa aún pendiente

Solo puede haber una acción pendiente a la vez. El bucle principal la procesa
y limpia la cola antes de aceptar otra.

### `GET /` y `GET /mirror`

Devuelve una página HTML de visor con renderizado canvas y controles opcionales.
Disponible cuando el componente mirror posee la raíz (modo standalone) o cuando
se coordina a través de `nabla_web_service`.

## Seguridad

- **Solo LAN**: No exponer el mirror a Internet público.
- **Protección por token**: La entrada requiere un token por arranque; esto es
  protección CSRF, no autenticación de usuario. Cualquiera en la LAN puede
  obtener el token.
- **Sin secretos en el repo**: El YAML de dispositivo con credenciales reales
  permanece en instalaciones privadas, nunca en este repositorio.

## Ejemplos de conversión

**rgb332 → RGB888** (Python):
```python
def rgb332_to_rgb888(byte):
    r = ((byte >> 5) & 0x07) * 255 // 7
    g = ((byte >> 2) & 0x07) * 255 // 7
    b = (byte & 0x03) * 255 // 3
    return (r, g, b)
```

**mono1 → píxeles** (Python):
```python
def mono1_to_pixels(data, width, height):
    pixels = []
    for i in range(width * height):
        byte_idx = i // 8
        bit_idx = 7 - (i % 8)
        pixels.append(255 if (data[byte_idx] >> bit_idx) & 1 else 0)
    return pixels
```

## Tasas de frame y ancho de banda

| Formato | Tamaño frame | ~10 FPS | Típico |
|---------|--------------|---------|--------|
| mono1 128×64 | 1 KB | 10 KB/s | 10 FPS polling |
| rgb332 160×128 | 20 KB | 200 KB/s | ~2-3 FPS polling |
| rgb332 240×240 | 56 KB | 560 KB/s | ~1-2 FPS |
| rgb332 480×320 | 150 KB | 1,5 MB/s | <1 FPS |

El visor del navegador hace polling secuencial; el FPS real depende de la
latencia de red. Las pestañas ocultas pausan el polling. Múltiples visores
comparten el mismo frame capturado.

## Extensiones futuras (no implementadas)

- **Entrada táctil**: Coordenadas absolutas vía `POST /mirror/touch`
- **rgb565**: Color 16 bits para mayor fidelidad (duplica ancho de banda)
- **Rectángulos sucios**: Actualizaciones parciales para reducir ancho de banda
- **WebSocket**: Push de frames en lugar de polling

Estas se mencionan en el issue #34 como fases posteriores. Este contrato cubre
solo lo implementado y verificado hoy.

## Integración con el componente

Ver el [componente nabla_display_mirror](../../external_components/nabla_display_mirror/README.md)
para configuración ESPHome. El README del componente cubre configuración YAML,
uso de memoria y evidencia física. Este documento especifica el contrato HTTP
para consumidores.
