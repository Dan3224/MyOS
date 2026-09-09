# MyOS 1.0 Graphical Core

MyOS 1.0 changes the visual foundation from a VGA text buffer to a 1024 x 768,
32-bit framebuffer supplied by GRUB through Multiboot.

## Boot flow

1. GRUB selects the preferred graphical mode.
2. MyOS validates the framebuffer information.
3. The boot screen marks boot loader, video, input and system stages.
4. The graphical home screen becomes active.

If the framebuffer is unavailable, MyOS safely falls back to the MyOS 0.5
VGA text shell rather than drawing to an unknown address.

## Screens

- Home dashboard
- Files
- System status
- App launcher
- Notes placeholder

Navigation uses H, F, S, A and N. Q requests a reboot.

## Reliability

The top-bar heartbeat changes while the graphical event loop is running.
MyOS also handles divide-by-zero, invalid-opcode, general-protection and page
fault exceptions. Those exceptions open a recovery screen and halt safely,
instead of failing silently.

## Current boundary

The desktop and application pages are real framebuffer output. Persistent
notes, mouse support and a complete window manager remain future MyOS 1.0
modules.
