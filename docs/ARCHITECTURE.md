
# Bacus Explorer Architecture

## Overview

Bacus Explorer is a modular browser developed by
Guafguaf Company.

## Main components

### app/
Desktop application and graphical interface.

### bacus-core/
Navigation, tabs, sessions and browser configuration.

### chromium/
Browser engine integration and process management.

### bacus-security/
Sandbox, site isolation, permissions and privacy.

### bacus-vault/
Encrypted password manager and credential generator.

### bacus-sync/
Future synchronization services.

### bacus-media/
Audio, video and multimedia integration.

### bacus-settings/
Browser preferences and configuration.

## Engineering principles

- Modular architecture.
- Secure defaults.
- Chromium compatibility.
- Independent browser modules.
- Automated testing.
- Explicit permission management.
- No silent security bypasses.

## Current development stage

Version: 0.1.0

Current milestone:
- Initial GTK application.
- Address bar interface.
- CMake build system.
- Initial automated tests.

Chromium integration is not implemented yet.
