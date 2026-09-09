# MyOS 0.5 Visual Shell

MyOS 0.5 introduces the first visual shell. It is rendered directly into the
VGA text buffer (80 x 25) and therefore works reliably in the current QEMU
and WSL setup without a separate graphical-window backend.

## Navigation

- H - home screen
- F - files screen
- S - system information
- T - command terminal
- Q - reboot

The terminal preserves the MyOS command tools: help, ls, cat, write,
sysinfo, uptime, reboot. The new home command returns to the visual shell.

## Scope

This is a colored text-mode user interface, not yet a pixel framebuffer,
window manager, mouse driver, or mobile launcher. Those are candidates for
the future MyOS 1.0 milestone.
