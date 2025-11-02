#!/usr/bin/env python3
"""
Web-based LED Control for Vault35RP
Öffnet eine GUI im Browser zur Steuerung aller 62 LEDs
"""

import sys
import struct
import threading
import time
from http.server import HTTPServer, BaseHTTPRequestHandler
from urllib.parse import parse_qs, urlparse
import json

try:
    import hid
except ImportError:
    print("❌ hidapi ist nicht installiert!")
    print("Bitte installiere es mit: pip3 install hidapi")
    sys.exit(1)


# VIAL/VIA Protocol Constants
MSG_LEN = 32
VIAL_SERIAL_NUMBER_MAGIC = "vial:f64c2b3c"
VIALRGB_EFFECT_DIRECT = 1

CMD_VIA_LIGHTING_SET_VALUE = 0x07
CMD_VIA_LIGHTING_GET_VALUE = 0x08

VIALRGB_GET_INFO = 0x40
VIALRGB_GET_MODE = 0x41
VIALRGB_GET_SUPPORTED = 0x42
VIALRGB_GET_NUMBER_LEDS = 0x43
VIALRGB_GET_LED_INFO = 0x44

VIALRGB_SET_MODE = 0x41
VIALRGB_DIRECT_FASTSET = 0x42


class KeyboardController:
    def __init__(self):
        self.dev = None
        self.hue = 0
        self.saturation = 255
        self.brightness = 100
        self.connected = False
        self.connect()
    
    def get_supported_modes(self):
        """Frage die unterstützten VIALRGB Modi ab"""
        if not self.dev:
            return []
        
        try:
            data = bytearray(MSG_LEN)
            data[0] = CMD_VIA_LIGHTING_GET_VALUE
            data[1] = VIALRGB_GET_SUPPORTED
            self.dev.write(bytes([0]) + data)
            
            # Warte auf Antwort
            time.sleep(0.1)
            response = self.dev.read(MSG_LEN, timeout_ms=1000)
            
            if response and len(response) >= 2:
                # Die Antwort enthält die unterstützten Modi als Bitmaske
                # Wir prüfen einfach ob VIALRGB_EFFECT_DIRECT (1) gesetzt ist
                modes = []
                for i in range(2, min(len(response), 32)):
                    if response[i] != 0:
                        modes.append(response[i])
                return modes
        except Exception as e:
            print(f"⚠️  Konnte unterstützte Modi nicht abfragen: {e}")
        
        return []
    
    def check_direct_support(self):
        """Prüfe ob Direct Control unterstützt wird"""
        modes = self.get_supported_modes()
        if VIALRGB_EFFECT_DIRECT in modes:
            print(f"✅ Direct Control unterstützt (Modi: {modes})")
            return True
        else:
            print(f"❌ Direct Control NICHT unterstützt (Modi: {modes})")
            return False
    
    def connect(self):
        """Verbinde mit der Tastatur"""
        print("🔍 Suche VIAL-Tastaturen mit DirectControl...")
        devices_found = 0
        vial_devices = []
        
        # Erst alle VIAL-Geräte sammeln
        for device in hid.enumerate():
            devices_found += 1
            if device["serial_number"] and device["serial_number"].startswith(VIAL_SERIAL_NUMBER_MAGIC):
                vial_devices.append(device)
        
        print(f"📋 {len(vial_devices)} VIAL-Tastatur(en) gefunden von {devices_found} Geräten")
        
        # Jetzt jedes VIAL-Gerät testen
        for device in vial_devices:
            try:
                print(f"\n🔌 Teste: {device['manufacturer_string']} {device['product_string']}")
                self.dev = hid.device()
                self.dev.open_path(device["path"])
                
                # Prüfe DirectControl Support
                if self.check_direct_support():
                    self.set_direct_mode()
                    self.connected = True
                    print(f"✅ Verbunden mit: {device['manufacturer_string']} {device['product_string']}")
                    print(f"   Device Path: {device['path']}")
                    return True
                else:
                    print(f"⏭️  Überspringe (kein DirectControl Support)")
                    self.dev.close()
                    self.dev = None
                    
            except Exception as e:
                print(f"❌ Fehler beim Öffnen: {e}")
                if self.dev:
                    try:
                        self.dev.close()
                    except:
                        pass
                    self.dev = None
                continue
        
        print(f"\n❌ Keine Tastatur mit DirectControl gefunden!")
        return False
    
    def set_direct_mode(self):
        """Aktiviere Direct Control Mode"""
        if not self.dev:
            return
        print("🎛️  Aktiviere Direct Control Mode...")
        data = bytearray(MSG_LEN)
        data[0] = CMD_VIA_LIGHTING_SET_VALUE
        data[1] = VIALRGB_SET_MODE
        data[2] = VIALRGB_EFFECT_DIRECT
        result = self.dev.write(bytes([0]) + data)
        print(f"   Mode-Befehl gesendet (result={result})")
        time.sleep(0.1)  # Kurz warten damit die Tastatur umschalten kann
    
    def update_leds(self):
        """Sende HSV-Werte an alle 62 LEDs"""
        if not self.dev or not self.connected:
            return
        
        leds_per_packet = 9
        total_leds = 62
        
        try:
            for start_led in range(0, total_leds, leds_per_packet):
                led_count = min(leds_per_packet, total_leds - start_led)
                
                # Baue das Paket wie im Original-Skript mit struct.pack
                # Format: BBHB = Byte, Byte, unsigned short (2 bytes), Byte
                payload = struct.pack("BBHB", 
                    CMD_VIA_LIGHTING_SET_VALUE,
                    VIALRGB_DIRECT_FASTSET,
                    start_led,  # unsigned short (2 bytes)
                    led_count
                )
                
                # Füge HSV-Werte hinzu
                buffer = []
                for i in range(led_count):
                    buffer.extend([self.hue, self.saturation, self.brightness])
                
                payload += bytes(buffer)
                
                # Fülle mit Nullen auf MSG_LEN
                payload += bytes(MSG_LEN - len(payload))
                
                # Sende mit führendem Report-ID Byte (0)
                result = self.dev.write(bytes([0]) + payload)
                
                # Debug nur beim ersten Paket
                if start_led == 0:
                    print(f"📤 LED Update: HSV=({self.hue},{self.saturation},{self.brightness}) - {total_leds} LEDs, payload_len={len(payload)}")
        except Exception as e:
            print(f"❌ Fehler beim Senden: {e}")
            import traceback
            traceback.print_exc()
            self.connected = False
    
    def set_hsv(self, h, s, v):
        """Setze HSV-Werte"""
        self.hue = max(0, min(255, h))
        self.saturation = max(0, min(255, s))
        self.brightness = max(0, min(255, v))
        self.update_leds()
    
    def close(self):
        """Schließe Verbindung"""
        if self.dev:
            # LEDs ausschalten
            self.set_hsv(0, 0, 0)
            self.dev.close()


# Globale Keyboard-Instanz
keyboard = KeyboardController()


HTML_PAGE = """<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Vault35RP LED Control</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 20px;
        }
        .container {
            background: white;
            border-radius: 20px;
            padding: 40px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            max-width: 500px;
            width: 100%;
        }
        h1 {
            text-align: center;
            color: #333;
            margin-bottom: 10px;
            font-size: 28px;
        }
        .subtitle {
            text-align: center;
            color: #666;
            margin-bottom: 30px;
            font-size: 14px;
        }
        .control-group {
            margin-bottom: 25px;
        }
        label {
            display: block;
            margin-bottom: 8px;
            color: #555;
            font-weight: 500;
            font-size: 14px;
        }
        .value-display {
            float: right;
            color: #667eea;
            font-weight: 600;
        }
        input[type="range"] {
            width: 100%;
            height: 8px;
            border-radius: 5px;
            background: #ddd;
            outline: none;
            -webkit-appearance: none;
        }
        input[type="range"]::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 20px;
            height: 20px;
            border-radius: 50%;
            background: #667eea;
            cursor: pointer;
            box-shadow: 0 2px 5px rgba(0,0,0,0.2);
        }
        input[type="range"]::-moz-range-thumb {
            width: 20px;
            height: 20px;
            border-radius: 50%;
            background: #667eea;
            cursor: pointer;
            box-shadow: 0 2px 5px rgba(0,0,0,0.2);
            border: none;
        }
        .color-preview {
            width: 100%;
            height: 80px;
            border-radius: 10px;
            margin-top: 20px;
            box-shadow: 0 4px 15px rgba(0,0,0,0.2);
            transition: background-color 0.1s ease;
        }
        .status {
            text-align: center;
            margin-top: 20px;
            padding: 10px;
            background: #f0f0f0;
            border-radius: 8px;
            font-size: 13px;
            color: #666;
        }
        .status.connected {
            background: #d4edda;
            color: #155724;
        }
        .presets {
            margin-top: 20px;
            display: grid;
            grid-template-columns: repeat(4, 1fr);
            gap: 10px;
        }
        .preset-btn {
            padding: 10px;
            border: none;
            border-radius: 8px;
            cursor: pointer;
            font-size: 11px;
            font-weight: 500;
            transition: transform 0.1s;
        }
        .preset-btn:hover {
            transform: scale(1.05);
        }
        .preset-btn:active {
            transform: scale(0.95);
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🎨 LED Control</h1>
        <div class="subtitle">Vault35RP • Alle 62 LEDs</div>
        
        <div class="control-group">
            <label>
                Farbe <span class="value-display" id="hue-label">Rot</span>
            </label>
            <input type="range" id="hue" min="0" max="255" value="0">
        </div>
        
        <div class="control-group">
            <label>
                Sättigung <span class="value-display" id="sat-label">100%</span>
            </label>
            <input type="range" id="saturation" min="0" max="255" value="255">
        </div>
        
        <div class="control-group">
            <label>
                Helligkeit <span class="value-display" id="bright-label">39%</span>
            </label>
            <input type="range" id="brightness" min="0" max="255" value="100">
        </div>
        
        <div class="color-preview" id="preview"></div>
        
        <div class="presets">
            <button class="preset-btn" style="background: #ff0000; color: white;" onclick="setPreset(0, 255, 100)">Rot</button>
            <button class="preset-btn" style="background: #00ff00; color: white;" onclick="setPreset(85, 255, 100)">Grün</button>
            <button class="preset-btn" style="background: #0000ff; color: white;" onclick="setPreset(170, 255, 100)">Blau</button>
            <button class="preset-btn" style="background: #ffff00; color: black;" onclick="setPreset(42, 255, 150)">Gelb</button>
            <button class="preset-btn" style="background: #ff00ff; color: white;" onclick="setPreset(212, 255, 100)">Magenta</button>
            <button class="preset-btn" style="background: #00ffff; color: black;" onclick="setPreset(127, 255, 100)">Cyan</button>
            <button class="preset-btn" style="background: #ffffff; color: black;" onclick="setPreset(0, 0, 200)">Weiß</button>
            <button class="preset-btn" style="background: #000000; color: white;" onclick="setPreset(0, 0, 0)">Aus</button>
        </div>
        
        <div class="status connected">✅ Verbunden</div>
    </div>
    
    <script>
        const hueSlider = document.getElementById('hue');
        const satSlider = document.getElementById('saturation');
        const brightSlider = document.getElementById('brightness');
        const preview = document.getElementById('preview');
        
        const hueLabel = document.getElementById('hue-label');
        const satLabel = document.getElementById('sat-label');
        const brightLabel = document.getElementById('bright-label');
        
        const colorNames = {
            0: 'Rot', 21: 'Orange', 42: 'Gelb', 85: 'Grün',
            127: 'Cyan', 170: 'Blau', 212: 'Lila', 234: 'Magenta'
        };
        
        function hsv2rgb(h, s, v) {
            h = h / 255.0;
            s = s / 255.0;
            v = v / 255.0;
            
            let r, g, b;
            const i = Math.floor(h * 6);
            const f = h * 6 - i;
            const p = v * (1 - s);
            const q = v * (1 - f * s);
            const t = v * (1 - (1 - f) * s);
            
            switch (i % 6) {
                case 0: r = v; g = t; b = p; break;
                case 1: r = q; g = v; b = p; break;
                case 2: r = p; g = v; b = t; break;
                case 3: r = p; g = q; b = v; break;
                case 4: r = t; g = p; b = v; break;
                case 5: r = v; g = p; b = q; break;
            }
            
            return [Math.round(r * 255), Math.round(g * 255), Math.round(b * 255)];
        }
        
        function updatePreview() {
            const h = parseInt(hueSlider.value);
            const s = parseInt(satSlider.value);
            const v = parseInt(brightSlider.value);
            
            const [r, g, b] = hsv2rgb(h, s, v);
            preview.style.backgroundColor = `rgb(${r}, ${g}, ${b})`;
            
            // Update labels
            const closest = Object.keys(colorNames).reduce((a, b) => 
                Math.abs(b - h) < Math.abs(a - h) ? b : a
            );
            hueLabel.textContent = colorNames[closest] || `H: ${h}`;
            satLabel.textContent = Math.round((s / 255) * 100) + '%';
            brightLabel.textContent = Math.round((v / 255) * 100) + '%';
        }
        
        function sendUpdate() {
            const h = parseInt(hueSlider.value);
            const s = parseInt(satSlider.value);
            const v = parseInt(brightSlider.value);
            
            fetch('/update', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({h: h, s: s, v: v})
            });
        }
        
        function setPreset(h, s, v) {
            hueSlider.value = h;
            satSlider.value = s;
            brightSlider.value = v;
            updatePreview();
            sendUpdate();
        }
        
        hueSlider.addEventListener('input', () => { updatePreview(); sendUpdate(); });
        satSlider.addEventListener('input', () => { updatePreview(); sendUpdate(); });
        brightSlider.addEventListener('input', () => { updatePreview(); sendUpdate(); });
        
        updatePreview();
        sendUpdate();
    </script>
</body>
</html>
"""


class LEDControlHandler(BaseHTTPRequestHandler):
    def log_message(self, format, *args):
        pass  # Unterdrücke Log-Ausgaben
    
    def do_GET(self):
        if self.path == '/':
            self.send_response(200)
            self.send_header('Content-type', 'text/html; charset=utf-8')
            self.end_headers()
            self.wfile.write(HTML_PAGE.encode('utf-8'))
        else:
            self.send_response(404)
            self.end_headers()
    
    def do_POST(self):
        if self.path == '/update':
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            data = json.loads(post_data.decode('utf-8'))
            
            keyboard.set_hsv(data['h'], data['s'], data['v'])
            
            self.send_response(200)
            self.send_header('Content-type', 'application/json')
            self.end_headers()
            self.wfile.write(json.dumps({'status': 'ok'}).encode('utf-8'))
        else:
            self.send_response(404)
            self.end_headers()


def main():
    if not keyboard.connected:
        print("\n❌ Konnte keine Vault35RP Tastatur finden!")
        print("\nBitte stelle sicher:")
        print("  - Tastatur ist per USB verbunden")
        print("  - VIAL ist geschlossen")
        sys.exit(1)
    
    PORT = 8080
    server = HTTPServer(('localhost', PORT), LEDControlHandler)
    
    print(f"\n🌐 Web-GUI gestartet!")
    print(f"📱 Öffne im Browser: http://localhost:{PORT}")
    print(f"⌨️  Drücke Ctrl+C zum Beenden\n")
    
    try:
        # Öffne Browser automatisch
        import webbrowser
        webbrowser.open(f'http://localhost:{PORT}')
        
        server.serve_forever()
    except KeyboardInterrupt:
        print("\n\n✅ Beende Server...")
        keyboard.close()
        server.shutdown()


if __name__ == "__main__":
    main()

