#!/bin/bash
# LED Control Web-GUI Starter

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

echo "🎨 Vault35RP LED Control"
echo "========================"
echo ""

# Prüfe ob venv existiert
if [ ! -d "venv" ]; then
    echo "📦 Erstelle virtuelle Python-Umgebung..."
    python3 -m venv venv
fi

# Aktiviere venv
source venv/bin/activate

# Prüfe/installiere hidapi
if ! python -c "import hid" 2>/dev/null; then
    echo "📥 Installiere hidapi..."
    pip install hidapi
    echo ""
fi

echo "🚀 Starte Web-GUI..."
echo ""

# Starte die GUI
python led_control_web.py

