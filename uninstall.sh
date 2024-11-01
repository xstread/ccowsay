#!/bin/bash

BINARY_NAME="ccowsay"
INSTALL_BIN_DIR="/usr/bin"
INSTALL_SHARE_DIR="/usr/local/share/ccowsay/cows"

echo "Removing $BINARY_NAME from $INSTALL_BIN_DIR..."
sudo rm -f "$INSTALL_BIN_DIR/$BINARY_NAME"
if [ $? -ne 0 ]; then
  echo "Failed to remove $BINARY_NAME from $INSTALL_BIN_DIR"
  exit 1
fi

echo "Removing cows directory from $INSTALL_SHARE_DIR..."
sudo rm -rf "$INSTALL_SHARE_DIR"
if [ $? -ne 0 ]; then
  echo "Failed to remove cows directory from $INSTALL_SHARE_DIR"
  exit 1
fi

echo "Uninstallation complete."
