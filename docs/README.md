# Documentation map

Start with [Getting started](GETTING-STARTED.md) to run a host fixture or consume
the library. Use this map to find the owner of each topic instead of copying
the same specification into several guides.

## Devices and their development

- [Display catalog](DISPLAY-CATALOG.md): reference formats, verified scope and
  remaining checks for each target. This is the central evidence register.
- [Device adoption](DEVICE-ADOPTION.md): the workflow from candidate hardware
  through shared UI adaptation to measured physical validation.
- [Hardware index](../hardware/README.md): reusable capabilities and board notes.
  Each board note owns wiring, build/recovery details and historical measurements.
- [Device compositions](../devices/README.md): complete firmware entry points
  versus packages and illustrative fragments.
- [Profiles](../profiles/README.md): geometry, renderer and presentation defaults.
- [Simulator](../simulator/README.md): host setup and runnable fixtures.

## Contracts and implementation

- [Library contract](LIBRARY-CONTRACT-v0.1.md): public entry points and ownership.
- [GitHub consumption](GITHUB-LIBRARY.md): pinned imports, resources and upgrades.
- [Architecture](ARCHITECTURE.md): current implementation boundaries.
- [UI consistency](UI-CONSISTENCY.md): shared appearance and interaction rules.
- [Navigation](../navigation/README.md) and [input](../navigation/INPUT.md):
  schemas and focus/input behavior.
- [Component catalog](../components/README.md): reusable modules and dependencies.
- [Brand](BRAND.md): triangle geometry and motion.

## Browser views and Home Assistant

- [Web service](../external_components/nabla_web_service/README.md): optional YAML modes.
- [Mirror HTTP contract](platform/DISPLAY-MIRROR-CONTRACT.md): firmware/consumer boundary.
- [Local gallery](../studio/README.md): select real connected displays.
- [Nabla Control](https://github.com/txemavs/nabla-hacs): HA integration, panel and card.
- [Studio plan](platform/WEB-SERVICE-AND-STUDIO.md): remaining simulator/editor work.

## Installation and future work

- [Private installations](PRIVATE-INSTALLATIONS.md) and
  [device checklist](PRIVATE-DEVICE-TEMPLATE.md): secrets and site-owned YAML.
- [Platform overview](platform/README.md) and [roadmap](platform/ROADMAP.md):
  planned capabilities and milestone exit gates. Proposal YAML is not firmware syntax.
- [Contributor rules](../AGENTS.md): practical workflow and verification.

Update the owning guide when behavior changes. Keep the root README as a short
introduction, linking to detailed evidence rather than repeating every test.
Historical measurements retain their date and scope; they are not current
firmware benchmarks.
