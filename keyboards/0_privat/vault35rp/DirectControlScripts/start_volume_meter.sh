#!/bin/bash
# Volume Meter Starter

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

echo "🔊 Vault35RP Volume Meter"
echo "=========================="
echo ""

# Aktiviere venv falls vorhanden
if [ -d "venv" ]; then
    source venv/bin/activate
fi

# Prüfe hidapi
if ! python -c "import hid" 2>/dev/null; then
    echo "❌ hidapi ist nicht installiert!"
    echo "Bitte installiere es mit: pip install hidapi"
    exit 1
fi

# Starte Volume Meter
python volume_meter.py

