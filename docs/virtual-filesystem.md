# MyOS 0.4: Virtual Filesystem

MyOS 0.4 adds a lightweight virtual filesystem stored in memory.

## Limits

- Up to 8 files
- File names up to 15 characters
- File contents up to 127 characters
- Contents exist only until reboot

## Shell commands

- `ls` — list files
- `cat <name>` — print a file
- `write <name> <text>` — create or replace a file
- `sysinfo` — show uptime and file count

The first boot creates `readme` and `version` automatically.
