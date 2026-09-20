# Mirror de pantalla (experimental)

Un canvas offline recibe píxeles lógicos desde el path de dibujado real del
dispositivo. Ver [DISPLAY-MIRROR-CONTRACT.md](../../docs/platform/DISPLAY-MIRROR-CONTRACT.md)
para la especificación completa de endpoints HTTP y documentación de disposición
de bytes.

Las composiciones Compact Display API usan begin/end alrededor de su llamada
render existente. Las composiciones LVGL envuelven el callback flush de ESPHome
y ensamblan rectángulos parciales, publicando cuando se hace flush de la última
área. Ninguna dibuja la UI dos veces.

## Configuración

Los valores por defecto monocromo 128x64 originales siguen siendo compatibles.
La captura color usa RGB332 (256 colores) para limitar RAM y ancho de banda;
la geometría se preserva pero los colores se cuantizan desde la pantalla física.
Width/height son dimensiones lógicas.

~~~yaml
nabla_display_mirror:
  id: screen_mirror
  width: 160
  height: 128
  color: true
~~~

Para pantallas compact, mantener la llamada render existente pero reemplazar
su display:

    auto &surface=id(screen_mirror).begin(it);
    // shell.render(surface, ...); existente
    id(screen_mirror).end();

Para LVGL usar lvgl_id en lugar del wrapper lambda de display:

~~~yaml
nabla_display_mirror:
  id: screen_mirror
  width: 240
  height: 240
  color: true
  lvgl_id: nabla_lvgl
~~~

Tamaños de configuración probados: OLED 128x64, ST7735 160x128, LVGL 240x240 y
480x320. El hook LVGL es específico de la implementación flush RGB565 de ESPHome.
Reenvía cada flush a LvglComponent::static_flush_cb, reteniendo el user data
original del display. Los cambios de rotación en runtime no están calificados:
configurar dimensiones/orientación para coincidir con la pantalla LVGL activa
al arranque.

Sin on_action significa solo lectura. El on_action opcional recibe
up/down/enter/back; mapearlos al controlador existente. Los callbacks HTTP
encolan una acción pendiente, ejecutada por el bucle principal. No hay inyección
genérica de touch LVGL implementada. La geometría de píxeles permanece
sincronizada con la entrada física.

## Memoria y transporte

Dos buffers de frame persistentes usan RAMAllocator (PSRAM preferido, fallback
a RAM interna). Una petición HTTP asigna una copia temporal del frame. El fallo
de asignación devuelve una vista no disponible en lugar de modificar el display.
Un frame monocromo 128x64 son 1024 bytes; frames RGB332 son 20.480 / 57.600 /
153.600 bytes para los tres tamaños color. Medir heap y latencia de entrada
antes de subir tasas de frame o añadir visores; sin calificación de carga
prolongada aún.

GET /mirror/capabilities describe dimensiones, formato y disponibilidad de entrada.
GET /mirror/frame devuelve bytes mono1 (MSB primero) o RGB332 por filas.
El navegador hace polling secuencial hasta 10 FPS monocromo o ~2.8 FPS color,
excluyendo latencia de red. Las páginas ocultas pausan y las desconectadas reintentan.
Los snapshots se comparten entre visores; la captura actualmente sigue corriendo
sin ellos.

La entrada usa un X-Nabla-Token por arranque, no autenticación de usuario. Usar
una LAN de confianza o AP protegido; no exponer contenido privado de pantalla
en Internet público. El modo standalone posee / y /mirror. nabla_web_service
coordina raíces responsive/mirror combinadas. ESPHome web_server sigue siendo
incompatible.

## Evidencia

- OLED: captura física inspeccionada; Down remoto cambió el frame y Up restauró
  selección; token inválido devolvió 401.
- Kit1: escritura USB verificada; frame real 160x128 recibido e inspeccionado
  visualmente.
- Panel LVGL grande: OTA exitoso; frame color real 480x320 recibido e inspeccionado.
- T-Watch: firmware 240x240 compilado; verificación física pendiente de energía
  del dispositivo.
- El cambio de perfil en navegador es una galería de equipos en vivo, no un
  simulador de menú.

Inversión de hardware/efectos de backlight después de dibujar no se reflejan.
Cuantización RGB332, rotaciones en runtime, clientes superpuestos, coexistencia
de audio y comportamiento de heap de larga duración siguen siendo límites
explícitos de calificación.
