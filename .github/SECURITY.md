# Security Policy

The Evitania project team takes the security and integrity of our codebase, build pipelines, and users seriously. This document outlines our security policies, supported versions, and how to responsibly report vulnerabilities.

## Supported Versions

Security patches and vulnerability fixes are actively maintained only for the latest release series:

| Version | Supported | Notes |
|:---|:---:|:---|
| `v0.320.x` (Latest) | ✅ | Active support & security fixes |
| `< v0.320.0` | ❌ | End of Life (Upgrade to latest release) |

## Scope & Threat Model

Evitania is an internal game modification and research tool for *Evitania Online*. Given its architecture (injected dynamic link library and process launcher), the security scope is defined as follows:

### In Scope
- **Injector & Loader Security**: Buffer overflow, arbitrary code execution, or path traversal in `injector.cpp` and `dll.cpp`.
- **Memory Safety & Integrity**: Uncontrolled write operations, out-of-bounds reads/writes, or heap corruptions outside intended game manipulation buffers.
- **Config & Serialization Security**: Crashes, unhandled exceptions, integer overflows, or arbitrary file write vulnerabilities in configuration parsers (`Config.cpp`).
- **Tooling & Automation**: Command injection, arbitrary code execution, or unsafe deserialization in build scripts and Python utilities (`resources/Tools/*.py`).
- **CI/CD & Supply Chain**: Vulnerabilities in GitHub Actions workflows (script injection, untrusted checkout, artifact poisoning).

### Out of Scope
- **Game Anti-Cheat Bans**: Client-side game modifications carry an inherent risk of detection by game servers. Anticheat detections or server-side bans do not constitute security vulnerabilities in this project.
- **Signature Invalidation**: Signatures or semantic offset extraction failures caused by game updates (please use the [Game Update Issue Template](ISSUE_TEMPLATE/game-update.yml) instead).
- **Physical Device Access**: Attacks requiring local physical access to an unlocked operating system.
- **Upstream Vendor Vulnerabilities**: Vulnerabilities in unmodified third-party libraries (`ImGui`, `MinHook`, `Il2CppResolver`) unless they are exposed or weaponized through Evitania's implementation.


## Reporting a Vulnerability

If you discover a security vulnerability, **please do not open a public issue, discussion, or pull request.** Public disclosure before a fix is available puts users at risk.

### Preferred: GitHub Private Vulnerability Reporting
Please submit a confidential report through GitHub's Security Advisories:
**[Report a Vulnerability via GitHub](https://github.com/RiiK26/Evitania/security/advisories/new)**

When reporting, please provide:
1. **Description**: Clear description of the vulnerability and its potential impact.
2. **Reproduction Steps**: Detailed step-by-step instructions or proof-of-concept (PoC) code.
3. **Affected Components**: Specific files, functions, or commit SHAs involved.
4. **Environment**: Operating System (Windows / Linux Proton), compiler (MinGW-w64 / MSVC), and version tag.

## Vulnerability Handling Process

1. **Initial Acknowledgment**: We will acknowledge receipt of your report within **48 hours**.
2. **Triage & Assessment**: We will assess severity, reproduction, and impact within **5 business days**.
3. **Patch Development & Validation**: A fix will be developed in a private security fork, verified against CI and static analysis (CodeQL), and prepared for release.
4. **Coordinated Disclosure**: A security advisory and patched release will be published. Security researchers will receive public credit in the release notes and advisory unless anonymity is requested.

## Safe Harbor

We consider security research conducted under this policy to be:
- Authorized in view of any applicable anti-hacking laws.
- In good faith, and we will not initiate legal action against researchers for accidental, good-faith violations of this policy.
