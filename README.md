# System Resource Monitor / Монитор системных ресурсов

A C++ desktop application for Linux that monitors system resources (CPU, RAM, disk) and provides notifications for high resource usage. The user interface is fully localized in Russian.

## Features / Возможности

- Real-time monitoring of CPU, RAM, and disk usage
- Graphical representation of resource usage over time
- System notifications when resource usage exceeds configurable thresholds
- Settings for customizing notification thresholds
- Historical data display for the last 10 minutes
- Complete Russian language localization of the user interface

## Screenshots

*The application features a clean, intuitive interface with real-time graphs showing resource usage.*

## Requirements

- Linux operating system
- GTK+ 3.0 or higher
- libnotify

## Building from Source

### Install Dependencies

```bash
sudo apt-get update
sudo apt-get install build-essential libgtk-3-dev libnotify-dev pkg-config
```

### Compile and Install

Use the provided Makefile to build the application:

```bash
make
```

Run the application:

```bash
./bin/system-monitor
```

Alternatively, use the install script to install system-wide:

```bash
sudo ./install.sh
```

## Configuration

The application uses a configuration file located at `~/.config/system-monitor/settings.conf`. 
If the file does not exist, default settings will be used.

Example configuration:

```
cpu_threshold=80
memory_threshold=85
disk_threshold=90
notification_frequency=30
```

## Localization / Локализация

This application's user interface is fully localized in Russian. All UI elements, graphs, 
notifications, and messages display text in Russian. Units are also adapted (e.g., "ГБ" instead of "GB").

## Troubleshooting

### Notifications Not Working

If desktop notifications are not working, the application will gracefully fall back to 
displaying notification messages in the console output. This typically happens when running
in environments without a proper D-Bus session bus.
