# ∇ nabla-esp-ui

Biblioteca modular de interfaces para **ESPHome + LVGL**, con un estilo común, componentes reutilizables y navegación adaptada a distintas pantallas y métodos de entrada.

**Define qué quieres controlar; reutiliza cómo se presenta y cómo funciona.**

> **Estado: diseño inicial.** Este README recoge la arquitectura y el primer alcance del proyecto. Los paquetes, perfiles y aplicaciones descritos son objetivos de implementación; todavía no hay una versión instalable ni hardware validado.

## La idea

Queremos montar paneles sin volver a diseñar botones, barras, menús y controles para cada dispositivo.

Un menú con ocho aplicaciones debe poder mostrarse como ocho iconos en una pantalla táctil o como una lista de ocho opciones en una pantalla pequeña con encoder. La estructura de navegación y las funciones se comparten; cada perfil aporta una presentación apropiada.

El mismo panel de luces debe poder utilizarse en distintas habitaciones, edificios o instalaciones de Home Assistant cambiando sus entidades, sin copiar su implementación.

## Qué configurará cada panel

- **Hardware:** placa, pantalla, táctil, encoder y retroiluminación.
- **Perfil de interfaz:** formato, densidad y método de entrada.
- **Navegación:** aplicaciones disponibles, orden y accesos favoritos.
- **Entidades:** luces, sensores y acciones de esa instalación.
- **Tema:** estilo común con ajustes opcionales.

La configuración se basará inicialmente en **packages, includes y variables nativos de ESPHome**. No se requiere un generador propio. La sintaxis pública se concretará con los primeros ejemplos compilables.

## Capas de la biblioteca

### Tema

Colores, tipografías, iconos, espaciados, tamaños y estados visuales compartidos. Las variantes compacta, normal y grande deben mantener una identidad común.

### Componentes visuales

Piezas como botones con icono, barras, indicadores, tarjetas, cabeceras y filas de menú.

Cada pieza se define una vez, admite parámetros y evita asumir una pantalla o entidad concreta. Una barra debe conservar su aspecto y comportamiento allí donde se utilice.

### Aplicaciones

Módulos funcionales compuestos con las piezas de la biblioteca: luces, sensores, escenas, clima, multimedia y ajustes.

Cada aplicación declara sus dependencias y agrupa la conexión con los datos, las acciones y sus presentaciones. Los datos particulares de una instalación quedan fuera del módulo.

### Navegación

Un contrato común para abrir aplicaciones, volver, ir al inicio, seleccionar y confirmar.

El perfil táctil inicial tendrá un escritorio de hasta ocho accesos y un menú global desplegable desde una esquina, con un acceso visible alternativo. El menú permitirá volver al escritorio y abrir ajustes o aplicaciones favoritas.

El perfil con encoder utilizará menús estructurados: girar para seleccionar, pulsar para entrar y una acción de retroceso configurable.

### Perfiles

La presentación depende de la resolución, orientación, espacio útil y método de entrada; no solamente de las pulgadas.

Una aplicación de luces puede representarse como tarjetas y controles de detalle en una táctil, y como una lista con ajuste secuencial en una pantalla pequeña. Compartirá funciones y configuración siempre que resulte práctico.

No se pretende reducir un escritorio completo hasta que quepa en cualquier pantalla: cada formato tendrá una composición apropiada.

### Hardware

Los controladores y conexiones físicas se mantienen separados de la UI.

La **JC3248W535CN** será el primer dispositivo objetivo. Su configuración exacta y compatibilidad se verificarán durante la implementación.

## Estructura prevista

Las siguientes carpetas se irán creando a medida que se implementen módulos:

```text
theme/          Colores, estilos, fuentes e iconos
components/     Piezas visuales parametrizables
apps/           Aplicaciones y sus presentaciones
navigation/     Menús y acciones comunes
profiles/       Composición por formato y método de entrada
hardware/       Configuraciones de placas y periféricos
examples/       Paneles completos y configuración de entidades
simulator/      Configuraciones host con SDL2 y datos de demostración
docs/catalog/   Fichas y ejemplos de los módulos disponibles
```

## Contrato de los módulos

- Cada instancia tendrá identificadores únicos, definidos al preparar la configuración.
- Añadir otra instancia no debe requerir copiar scripts ni editar la implementación del módulo.
- Cada módulo declarará sus dependencias y parámetros obligatorios.
- Eliminar un módulo debe retirar su lógica asociada, evitando referencias colgantes.
- Los componentes visuales no contendrán entidades de Home Assistant fijadas a una instalación.
- Las aplicaciones definirán el tratamiento de estados desconocidos, desconectados y pendientes.
- Los perfiles compartirán las acciones de navegación, aunque cambie su representación.
- Se priorizarán las acciones nativas de ESPHome y LVGL; el C++ adicional tendrá un alcance limitado y documentado.
- Los ejemplos mantendrán las credenciales en archivos de secretos excluidos del repositorio.

ESPHome resuelve los IDs y la composición durante la compilación. La modularidad de esta biblioteca se construirá sobre ese modelo, sin depender de descubrimiento dinámico de widgets.

## Catálogo de piezas

Cada componente o aplicación tendrá una ficha con:

- Nombre, finalidad y estado: experimental o validado.
- Parámetros, valores predeterminados e identificadores.
- Dependencias y perfiles compatibles.
- Ejemplo mínimo de inclusión.
- Captura o demostración, cuando esté disponible.
- Verificaciones realizadas y limitaciones conocidas.

El catálogo distinguirá lo disponible de lo planificado.

## Desarrollo en el ordenador

El entorno de desarrollo previsto es **ESPHome host + SDL2**: una ventana ejecutará los mismos includes de interfaz que se utilizarán en el dispositivo.

Esto permitirá:

- Revisar estilos y distribuciones en distintas resoluciones.
- Probar interacción táctil con el ratón.
- Asociar teclas a acciones de navegación para probar menús.
- Usar datos ficticios para verificar estados y transiciones.
- Ejecutar ejemplos de componentes sin grabar firmware en una placa.

En Windows se contempla WSL con soporte gráfico. Las instrucciones reproducibles y la versión de ESPHome se fijarán cuando el primer ejemplo esté validado.

Los cambios de YAML requieren recompilar. La ejecución en el ordenador no valida el consumo de memoria, el rendimiento ni los controladores físicos del ESP: también habrá compilaciones y pruebas en hardware real.

## Primer alcance

- [ ] Fijar una versión de ESPHome y un ejemplo host + SDL2 reproducible.
- [ ] Definir el tema inicial y las piezas visuales básicas.
- [ ] Definir el contrato de navegación y registro de aplicaciones.
- [ ] Mostrar el mismo menú como escritorio de iconos y lista para encoder.
- [ ] Implementar las aplicaciones de luces y ajustes.
- [ ] Preparar y validar el perfil de hardware JC3248W535CN.
- [ ] Documentar los primeros módulos en el catálogo.

Las ocho posiciones del escritorio no implican ocho aplicaciones terminadas. Los accesos se mostrarán según los módulos configurados.

### Criterio de éxito

Poder añadir, repetir o quitar un control y reutilizar una aplicación en otro perfil sin modificar los archivos internos de los componentes. Los cambios esperados deben concentrarse en la composición del panel, su hardware y sus entidades.

## Referencias

- [ESPHome LVGL](https://esphome.io/components/lvgl/)
- [Widgets LVGL](https://esphome.io/components/lvgl/widgets/)
- [Layouts LVGL](https://esphome.io/components/lvgl/layouts/)
- [Packages de ESPHome](https://esphome.io/components/packages/)
- [Recetario LVGL](https://esphome.io/cookbook/lvgl/)
- [Pantalla SDL2 para host](https://esphome.io/components/display/sdl/)
- [ESPHome UI Kit](https://github.com/mplogas/esphome-ui-kit)
- [ESPHome Modular LVGL Buttons](https://github.com/agillis/esphome-modular-lvgl-buttons)

Estos proyectos sirven como referencias de diseño. Cualquier reutilización de código deberá conservar su licencia y atribución correspondiente.
