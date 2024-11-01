#!/bin/bash
SOURCE_FILE="main.c"
BINARY_NAME="ccowsay"
INSTALL_BIN_DIR="/usr/bin"
INSTALL_SHARE_DIR="/usr/local/share/ccowsay/cows"

echo "Compiling $SOURCE_FILE..."
gcc -o "$BINARY_NAME" "$SOURCE_FILE"
if [ $? -ne 0 ]; then
  echo "Compilation failed!"
  exit 1
fi

echo "Moving $BINARY_NAME to $INSTALL_BIN_DIR..."
sudo mv "$BINARY_NAME" "$INSTALL_BIN_DIR"
if [ $? -ne 0 ]; then
  echo "Failed to move binary to $INSTALL_BIN_DIR"
  exit 1
fi

echo "Copying cows directory to $INSTALL_SHARE_DIR..."
sudo mkdir -p "$INSTALL_SHARE_DIR"
sudo cp -r cows/* "$INSTALL_SHARE_DIR"
if [ $? -ne 0 ]; then
  echo "Failed to copy cows directory to $INSTALL_SHARE_DIR"
  exit 1
fi

echo "Installation complete. You can now run the program with '$BINARY_NAME'"
