#!/usr/bin/env python3
"""
Volume Meter for Vault35RP
Zeigt die macOS Systemlautstärke auf LEDs 36-53 an
"""

import sys
import struct
import time
import subprocess

try:
    import hid
except ImportError:
    print("❌ hidapi ist nicht installiert!")
    print("Bitte installiere es mit: pip3 install hidapi")
    sys.exit(1)

# Versuche native macOS API für bessere Performance
try:
    from AppKit import NSSound
    from Foundation import NSDistributedNotificationCenter, NSOperationQueue
    USE_NATIVE_API = True
    USE_EVENT_BASED = True
except ImportError:
    USE_NATIVE_API = False
    USE_EVENT_BASED = False
    print("ℹ️  AppKit nicht verfügbar, nutze osascript (etwas langsamer)")


# VIAL/VIA Protocol Constants
MSG_LEN = 32
VIAL_SERIAL_NUMBER_MAGIC = "vial:f64c2b3c"
VIALRGB_EFFECT_DIRECT = 1

CMD_VIA_LIGHTING_SET_VALUE = 0x07
CMD_VIA_LIGHTING_GET_VALUE = 0x08

VIALRGB_GET_SUPPORTED = 0x42
VIALRGB_SET_MODE = 0x41
VIALRGB_DIRECT_FASTSET = 0x42

# LED Configuration
VOLUME_LED_START = 53  # Erste LED für Volume Meter (leise)
VOLUME_LED_END = 38    # Letzte LED für Volume Meter (laut)
TOTAL_LEDS = 62        # Gesamt-LEDs auf der Tastatur
NUM_VOLUME_LEDS = VOLUME_LED_START - VOLUME_LED_END + 1  # 16 LEDs (53 bis 38)


def get_macos_volume():
    """Lese die aktuelle macOS Systemlautstärke aus (0-100)"""
    
    # Versuche zuerst native macOS API (10-100x schneller, weniger CPU)
    if USE_NATIVE_API:
        try:
            volume = NSSound.systemVolume()
            if volume is not None:
                return int(volume * 100)  # 0.0-1.0 -> 0-100
        except Exception as e:
            pass
    
    # Fallback auf osascript
    try:
        result = subprocess.run(
            ["osascript", "-e", "output volume of (get volume settings)"],
            capture_output=True,
            text=True,
            timeout=1
        )
        if result.returncode == 0:
            output = result.stdout.strip()
            
            # Prüfe ob die Ausgabe eine gültige Zahl ist
            if not output or "missing value" in output.lower() or not output.isdigit():
                # Fallback: Versuche alternative Methode
                result2 = subprocess.run(
                    ["osascript", "-e", "get volume settings"],
                    capture_output=True,
                    text=True,
                    timeout=1
                )
                if result2.returncode == 0 and "output volume:" in result2.stdout:
                    # Parse "output volume:75, input volume:50, ..."
                    for part in result2.stdout.split(','):
                        if 'output volume:' in part:
                            volume_str = part.split(':')[1].strip()
                            if volume_str.isdigit():
                                return int(volume_str)
                
                return None
            
            # Wenn es eine gültige Zahl ist, parse sie
            volume = int(output)
            return max(0, min(100, volume))  # Clamp auf 0-100
        else:
            return None
    except Exception as e:
        return None


class VolumeKeyboardController:
    def __init__(self):
        self.dev = None
        self.connected = False
        self.last_volume = -1
        self.led_states = [(0, 0, 0)] * TOTAL_LEDS  # (H, S, V) für jede LED
        self.volume_read_errors = 0
        self.notification_center = None
        self.volume_observer = None
    
    def check_direct_support(self):
        """Prüfe ob Direct Control unterstützt wird"""
        if not self.dev:
            return False
        
        try:
            data = bytearray(MSG_LEN)
            data[0] = CMD_VIA_LIGHTING_GET_VALUE
            data[1] = VIALRGB_GET_SUPPORTED
            self.dev.write(bytes([0]) + data)
            
            time.sleep(0.1)
            response = self.dev.read(MSG_LEN, timeout_ms=1000)
            
            if response and len(response) >= 2:
                modes = []
                for i in range(2, min(len(response), 32)):
                    if response[i] != 0:
                        modes.append(response[i])
                
                if VIALRGB_EFFECT_DIRECT in modes:
                    return True
            
            return False
        except Exception as e:
            return False
    
    def connect(self):
        """Verbinde mit der Vault35RP mit DirectControl"""
        print("🔍 Suche Vault35RP mit DirectControl Support...")
        
        vial_devices = []
        for device in hid.enumerate():
            if device["serial_number"] and device["serial_number"].startswith(VIAL_SERIAL_NUMBER_MAGIC):
                vial_devices.append(device)
        
        print(f"📋 {len(vial_devices)} VIAL-Tastatur(en) gefunden")
        
        for device in vial_devices:
            try:
                product = device.get('product_string', '')
                print(f"\n🔌 Teste: {device['manufacturer_string']} {product}")
                
                self.dev = hid.device()
                self.dev.open_path(device["path"])
                
                # Prüfe DirectControl Support
                if self.check_direct_support():
                    print("✅ Direct Control unterstützt!")
                    self.set_direct_mode()
                    self.connected = True
                    print(f"✅ Verbunden mit: {device['manufacturer_string']} {product}")
                    return True
                else:
                    print("⏭️  Kein DirectControl Support, überspringe...")
                    self.dev.close()
                    self.dev = None
                    
            except Exception as e:
                print(f"❌ Fehler: {e}")
                if self.dev:
                    try:
                        self.dev.close()
                    except:
                        pass
                    self.dev = None
                continue
        
        print("\n❌ Keine Tastatur mit DirectControl gefunden!")
        return False
    
    def set_direct_mode(self):
        """Aktiviere Direct Control Mode"""
        if not self.dev:
            return
        
        data = bytearray(MSG_LEN)
        data[0] = CMD_VIA_LIGHTING_SET_VALUE
        data[1] = VIALRGB_SET_MODE
        data[2] = VIALRGB_EFFECT_DIRECT
        self.dev.write(bytes([0]) + data)
        time.sleep(0.1)
    
    def disable_direct_mode(self):
        """Deaktiviere Direct Control Mode und setze auf normalen RGB-Modus zurück"""
        if not self.dev:
            return
        
        try:
            # Setze auf einen normalen RGB Matrix Effekt (z.B. Solid Color = Mode 2)
            # Mode 0 = aus, Mode 1 = Direct Control, Mode 2+ = normale RGB Effekte
            data = bytearray(MSG_LEN)
            data[0] = CMD_VIA_LIGHTING_SET_VALUE
            data[1] = VIALRGB_SET_MODE
            data[2] = 2  # Normaler RGB-Effekt (z.B. Solid Color oder erster verfügbarer)
            self.dev.write(bytes([0]) + data)
            time.sleep(0.1)
            print("   DirectControl Modus deaktiviert")
        except Exception as e:
            print(f"   Warnung: Konnte DirectControl nicht deaktivieren: {e}")
    
    def set_all_leds(self):
        """Sende alle LED-Zustände an die Tastatur"""
        if not self.dev or not self.connected:
            return
        
        leds_per_packet = 9
        
        try:
            for start_led in range(0, TOTAL_LEDS, leds_per_packet):
                led_count = min(leds_per_packet, TOTAL_LEDS - start_led)
                
                payload = struct.pack("BBHB", 
                    CMD_VIA_LIGHTING_SET_VALUE,
                    VIALRGB_DIRECT_FASTSET,
                    start_led,
                    led_count
                )
                
                buffer = []
                for i in range(led_count):
                    led_idx = start_led + i
                    h, s, v = self.led_states[led_idx]
                    buffer.extend([h, s, v])
                
                payload += bytes(buffer)
                payload += bytes(MSG_LEN - len(payload))
                
                self.dev.write(bytes([0]) + payload)
                
        except Exception as e:
            print(f"❌ Fehler beim Senden: {e}")
            self.connected = False
    
    def update_volume_display(self, volume):
        """Aktualisiere LED-Anzeige basierend auf Lautstärke (0-100)"""
        
        # Setze alle LEDs auf schwarz (aus)
        for i in range(TOTAL_LEDS):
            self.led_states[i] = (0, 0, 0)
        
        if volume == 0:
            # Lautstärke = 0: LED 53 leuchtet rot
            self.led_states[VOLUME_LED_START] = (0, 255, 100)  # Rot
        else:
            # Berechne wie viele LEDs leuchten sollen (1 bis NUM_VOLUME_LEDS)
            # Bei sehr geringer Lautstärke (1-5%): mindestens 1 LED
            if volume <= 5:
                num_leds_on = 1
            else:
                # Linear skalieren: 6-100% -> 1 bis 16 LEDs
                num_leds_on = int((volume / 100.0) * NUM_VOLUME_LEDS)
                num_leds_on = max(1, min(NUM_VOLUME_LEDS, num_leds_on))
            
            # Setze die entsprechenden LEDs auf weiß (von 53 rückwärts bis 38)
            for i in range(num_leds_on):
                led_idx = VOLUME_LED_START - i  # Von 53 runter zählen
                # Weiß = H=0, S=0, V=hoch
                self.led_states[led_idx] = (0, 0, 200)
        
        # Sende an Tastatur
        self.set_all_leds()
    
    def setup_volume_listener(self):
        """Event-Modus ist aktuell deaktiviert - zu komplex für PyObjC"""
        # Event-basierter Modus würde PyObjC callbacks und NSRunLoop erfordern
        # Das ist zu fragil und kompliziert. Polling mit 1 Sekunde ist effizienter.
        return False
    
    def run(self):
        """Hauptschleife: Überwache Lautstärke und aktualisiere LEDs"""
        print("\n🔊 Volume Meter gestartet!")
        print(f"📊 LEDs {VOLUME_LED_START} → {VOLUME_LED_END} zeigen Lautstärke an")
        print(f"   - Lautstärke 0%: LED {VOLUME_LED_START} rot")
        print(f"   - Lautstärke 1-5%: LED {VOLUME_LED_START} weiß")
        print(f"   - Lautstärke 100%: alle LEDs ({VOLUME_LED_START} bis {VOLUME_LED_END}) weiß")
        print("\n⌨️  Drücke Ctrl+C zum Beenden\n")
        
        # Nutze optimiertes Polling (Event-Modus ist zu komplex)
        print("ℹ️  Polling-Modus: Update alle 1 Sekunde (minimale CPU-Last)")
        
        # Initiale Anzeige
        volume = get_macos_volume()
        if volume is not None:
            print(f"🔊 Lautstärke: {volume}%")
            self.update_volume_display(volume)
            self.last_volume = volume
        
        # Erste Warnung wenn Lautstärke nicht gelesen werden kann
        first_read = True
        
        try:
            # Optimiertes Polling: 1 Sekunde Intervall
            while True:
                volume = get_macos_volume()
                
                if volume is None:
                    self.volume_read_errors += 1
                    if first_read or self.volume_read_errors == 1:
                        print("⚠️  Lautstärke kann nicht gelesen werden")
                        print("   Mögliche Ursachen:")
                        print("   - Kein Audio-Ausgabegerät aktiv")
                        print("   - Systemeinstellungen → Ton → Ausgabe prüfen")
                        print("   LEDs bleiben auf letztem Wert...\n")
                        first_read = False
                    time.sleep(1)
                    continue
                
                if self.volume_read_errors > 0:
                    print("✅ Lautstärke kann wieder gelesen werden")
                    self.volume_read_errors = 0
                
                first_read = False
                
                if volume != self.last_volume:
                    print(f"🔊 Lautstärke: {volume}%")
                    self.update_volume_display(volume)
                    self.last_volume = volume
                
                time.sleep(1.0)  # Update alle 1 Sekunde (minimale CPU-Last)
                
        except KeyboardInterrupt:
            print("\n\n✅ Beende Volume Meter...")
            self.cleanup()
    
    def cleanup(self):
        """Aufräumen beim Beenden"""
        # Entferne Event-Listener
        if self.notification_center and self.volume_observer:
            try:
                self.notification_center.removeObserver_(self.volume_observer)
            except:
                pass
        
        if self.dev and self.connected:
            print("🧹 Aufräumen...")
            
            # Alle LEDs ausschalten
            print("   Schalte LEDs aus...")
            for i in range(TOTAL_LEDS):
                self.led_states[i] = (0, 0, 0)
            self.set_all_leds()
            
            time.sleep(0.1)
            
            # Deaktiviere Direct Control Modus
            self.disable_direct_mode()
            
            time.sleep(0.1)
            self.dev.close()
            print("✅ Tastatur kann wieder normal verwendet werden")


def main():
    controller = VolumeKeyboardController()
    
    if not controller.connect():
        print("\n❌ Konnte nicht mit der Tastatur verbinden!")
        print("\nBitte stelle sicher:")
        print("  - Vault35RP ist per USB verbunden")
        print("  - VIAL ist geschlossen")
        print("  - Terminal hat Eingabe-Berechtigung")
        sys.exit(1)
    
    controller.run()


if __name__ == "__main__":
    main()

