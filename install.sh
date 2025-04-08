#!/bin/bash

# System Resource Monitor Installer Script

# Check if running as root
if [ "$EUID" -ne 0 ]; then
  echo "Please run as root"
  exit 1
fi

echo "Installing System Resource Monitor..."

# Install dependencies
echo "Installing dependencies..."
apt-get update
apt-get install -y build-essential libgtk-3-dev libnotify-dev pkg-config

# Create build directory
echo "Building application..."
mkdir -p build
cd build

# Build the application
cmake ..
make

# Install the application
echo "Installing application..."
make install

# Create desktop entry
echo "Creating desktop shortcut..."
cat > /usr/share/applications/system-resource-monitor.desktop << EOF
[Desktop Entry]
Name=System Resource Monitor
Comment=Monitor system resources and get alerts for high usage
Exec=/usr/local/bin/system-monitor
Icon=/usr/local/share/system-monitor/application.svg
Terminal=false
Type=Application
Categories=System;Monitor;
Keywords=system;monitor;resources;cpu;memory;disk;
EOF

# Create directory for application data
mkdir -p /usr/local/share/system-monitor/
cp ../assets/application.svg /usr/local/share/system-monitor/

echo "Installation complete! You can now run System Resource Monitor from your applications menu."
