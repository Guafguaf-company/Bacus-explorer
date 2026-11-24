# 🐢 Bacus Explorer

<p align="center">
  <img src="https://img.shields.io/badge/Guafguaf_Company-Official-238636?style=for-the-badge" alt="Guafguaf Company">
  <img src="https://img.shields.io/badge/License-MPL--2.0-blue?style=for-the-badge" alt="MPL 2.0">
  <img src="https://img.shields.io/badge/Language-C%2B%2B20-00599C?style=for-the-badge&logo=cplusplus" alt="C++20">
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Engine-Chromium-4285F4?style=flat-square&logo=googlechrome&logoColor=white" alt="Chromium">
  <img src="https://img.shields.io/badge/Platform-Linux-FCC624?style=flat-square&logo=linux&logoColor=black" alt="Linux">
  <img src="https://img.shields.io/badge/Status-Early%20Development-orange?style=flat-square" alt="Early Development">
  <img src="https://img.shields.io/github/stars/ciberspar/bacus-explorer?style=flat-square&label=Stars" alt="GitHub Stars">
  <img src="https://img.shields.io/github/issues/ciberspar/bacus-explorer?style=flat-square&label=Issues" alt="GitHub Issues">
</p>

<p align="center">
  **Un navegador independiente. Un ecosistema abierto. Una experiencia segura.**
</p>

---

## 🌐 ¿Qué es Bacus Explorer?

**Bacus Explorer** es un navegador web de código abierto desarrollado por **Guafguaf Company**.

Su objetivo es ofrecer una experiencia de navegación moderna, rápida, compatible y segura, con especial atención a la privacidad, el rendimiento y la estabilidad.

El proyecto utiliza Chromium como motor de navegación y busca construir una interfaz y una arquitectura propias.

## 🎯 Objetivos

- 🌍 Compatibilidad con páginas web modernas.
- ⚡ Rendimiento adaptable a equipos modestos y potentes.
- 🔒 Seguridad como prioridad.
- 🛡️ Aislamiento de procesos y protección del contenido web.
- 🎬 Compatibilidad con vídeo, audio y multimedia.
- 🧩 Arquitectura modular y mantenible.
- 🔄 Actualizaciones frecuentes del motor y sus componentes.
- 🔐 Protección de datos y credenciales.

## 🔑 Bacus Vault

Bacus Vault será el gestor de contraseñas integrado en Bacus Explorer.

Sus funcionalidades previstas incluyen:

- Generador criptográficamente seguro de contraseñas.
- Contraseñas personalizadas con referencias mnemotécnicas.
- Almacenamiento cifrado.
- Autocompletado protegido por origen.
- Notas seguras.
- Compatibilidad futura con passkeys.
- Auditoría de contraseñas débiles o reutilizadas.
- Copias de seguridad cifradas.

La seguridad de las credenciales será independiente del motor Chromium.

## 🛡️ Seguridad

La seguridad será una parte fundamental de Bacus Explorer.

Principios del proyecto:

1. No desactivar el sandbox para mejorar el rendimiento.
2. Mantener el aislamiento entre sitios.
3. Solicitar permisos de manera explícita.
4. No ejecutar descargas automáticamente.
5. No almacenar contraseñas en texto plano.
6. Mantener actualizados los componentes vulnerables.
7. Evitar que un fallo de una pestaña cierre todo el navegador.

Los mecanismos de seguridad se incorporarán y verificarán progresivamente mediante pruebas.

## 🏗️ Arquitectura prevista

```text
bacus-explorer/
├── app/
│   ├── browser/
│   ├── ui/
│   └── platform/
├── bacus-core/
│   ├── navigation/
│   ├── tabs/
│   ├── sessions/
│   └── performance/
├── chromium/
│   ├── engine/
│   ├── process_manager/
│   └── compatibility/
├── bacus-security/
│   ├── sandbox/
│   ├── permissions/
│   ├── privacy/
│   └── downloads/
├── bacus-vault/
│   ├── include/
│   ├── src/
│   └── tests/
├── bacus-sync/
├── bacus-media/
├── bacus-settings/
├── tests/
└── docs/
```

Esta es la arquitectura inicial prevista. Los módulos se implementarán gradualmente.

## 💻 Plataformas

| Plataforma | Objetivo |
|---|---|
| Linux Mint 22.3 MATE | Primera plataforma |
| Otras distribuciones Linux | Futuro |
| Windows | Por evaluar |
| macOS | Por evaluar |

## ⚙️ Tecnologías previstas

| Tecnología | Uso |
|---|---|
| C++20 | Lenguaje principal |
| Chromium | Motor web |
| GTK | Interfaz gráfica Linux |
| CMake | Sistema de compilación |
| GitHub Actions | Integración continua y pruebas |

La integración concreta de Chromium y sus dependencias se validará durante el desarrollo.

## 🗺️ Hoja de ruta

- [ ] **0.1** — Ventana principal y navegación.
- [ ] **0.2** — Integración del motor Chromium.
- [ ] **0.3** — Seguridad, permisos y descargas.
- [ ] **0.4** — Multimedia y compatibilidad web.
- [ ] **0.5** — Gestión adaptativa de memoria.
- [ ] **0.6** — Historial, marcadores y privacidad.
- [ ] **0.7** — Primera versión de Bacus Vault.
- [ ] **1.0** — Primera versión estable para Linux.

## 📜 Licencia

Bacus Explorer utiliza **Mozilla Public License 2.0 (MPL-2.0)**.

Consulta el archivo `LICENSE` para conocer las condiciones de distribución y modificación.

Los componentes externos conservan sus respectivas licencias y avisos.

## 🏢 Guafguaf Company

Bacus Explorer forma parte del ecosistema tecnológico de **Guafguaf Company**.

Nuestro objetivo es desarrollar software abierto, independiente, mantenible y accesible.

---

<p align="center">
  🐢 **Bacus Explorer — Built by Guafguaf Company**
</p>

<p align="center">
  *Explora libremente. Navega con seguridad.*
</p>
