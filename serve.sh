#!/bin/bash

# Check if Python is available
if command -v python3 &> /dev/null; then
    echo "Starting server with Python 3..."
    cd web && python3 -m http.server
elif command -v python &> /dev/null; then
    echo "Starting server with Python..."
    cd web && python -m SimpleHTTPServer
else
    echo "Error: Python not found. Please install Python or use another HTTP server."
    exit 1
fi

echo ""
echo "Open http://localhost:8000 in your browser to access the Infusion Pump CTF Challenge."
echo "Admin password: SecureMed2025"
echo "Exploit hint: Try setting rate=1000 and duration=4294968 in the admin panel."