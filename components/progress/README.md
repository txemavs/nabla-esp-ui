# Progress footer

widget.yaml uses the shared bar surface, at the same height as the header.
It has a gray background, cyan fill and a bottom-right percentage.
Include it with components/toolbar/progress.yaml and the current navigation
renderer. The existing script names remain for compatibility.

Call toolbar_set_progress(value: 0) to start, then values up to 100.
Negative values hide the footer. Values are clamped to -2..100.
There is no header divider or header progress indicator anymore.
toolbar_demo_load runs twenty 50 ms steps and hides the footer at completion.
The example starts this simulated load alongside the toolbar triangle spin.
Real work should stop the demo script and supply measured progress.

This first progress instance uses fixed IDs; multiple simultaneous progress
bars need a parameterized state/controller before they are supported.

The desktop footer is always present. After startup reaches 100% (held for
100 ms), progress is replaced by Soluciones Lógicas Nabla on the left and
HH:MM on the right. The shared footer_set_clock(text) action accepts clock
updates from hardware adapters; the simulator uses host time, Europe/Madrid.
Application views hide the idle desktop footer and may later supply their own.
Negative progress restores this route-specific state rather than hiding all footers.
