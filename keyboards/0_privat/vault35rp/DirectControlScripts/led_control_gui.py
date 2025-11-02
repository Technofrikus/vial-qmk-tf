#!/usr/bin/env python3
"""
Simple LED Control GUI for Vault35RP
Steuere alle 62 LEDs gleichzeitig mit Farbe und Helligkeit
"""

import sys
import struct
import tkinter as tk
from tkinter import ttk

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
VIALRGB_SET_MODE = 0x41
VIALRGB_DIRECT_FASTSET = 0x42


def find_vial_keyboard():
    """Finde die Vault35RP Tastatur"""
    for device in hid.enumerate():
        if device["serial_number"] and device["serial_number"].startswith(VIAL_SERIAL_NUMBER_MAGIC):
            return device
    return None


def hsv_to_rgb(h, s, v):
    """Konvertiere HSV zu RGB (0-255)"""
    h = h / 255.0
    s = s / 255.0
    v = v / 255.0
    
    if s == 0.0:
        r = g = b = v
    else:
        i = int(h * 6.0)
        f = (h * 6.0) - i
        p = v * (1.0 - s)
        q = v * (1.0 - s * f)
        t = v * (1.0 - s * (1.0 - f))
        i = i % 6
        
        if i == 0:
            r, g, b = v, t, p
        elif i == 1:
            r, g, b = q, v, p
        elif i == 2:
            r, g, b = p, v, t
        elif i == 3:
            r, g, b = p, q, v
        elif i == 4:
            r, g, b = t, p, v
        else:
            r, g, b = v, p, q
    
    return int(r * 255), int(g * 255), int(b * 255)


class LEDControlGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Vault35RP LED Control")
        self.root.geometry("400x300")
        
        # Finde und öffne die Tastatur
        desc = find_vial_keyboard()
        if desc is None:
            tk.messagebox.showerror("Fehler", "Vault35RP Tastatur nicht gefunden!\n\nBitte stelle sicher:\n- Tastatur ist per USB verbunden\n- VIAL ist geschlossen")
            sys.exit(1)
        
        self.dev = hid.device()
        try:
            self.dev.open_path(desc["path"])
        except Exception as e:
            tk.messagebox.showerror("Fehler", f"Konnte Tastatur nicht öffnen:\n{e}")
            sys.exit(1)
        
        # Setze Direct Control Mode
        self.set_direct_mode()
        
        # GUI erstellen
        self.create_widgets()
        
        # Initiale Werte setzen
        self.hue = 0
        self.saturation = 255
        self.brightness = 100
        self.update_leds()
        
        # Update-Timer
        self.root.after(50, self.periodic_update)
    
    def set_direct_mode(self):
        """Aktiviere Direct Control Mode"""
        data = bytearray(MSG_LEN)
        data[0] = CMD_VIA_LIGHTING_SET_VALUE
        data[1] = VIALRGB_SET_MODE
        data[2] = VIALRGB_EFFECT_DIRECT
        self.dev.write(bytes([0]) + data)
    
    def create_widgets(self):
        """Erstelle GUI-Elemente"""
        main_frame = ttk.Frame(self.root, padding="20")
        main_frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        
        # Titel
        title = ttk.Label(main_frame, text="🎨 LED Steuerung", font=("Helvetica", 18, "bold"))
        title.grid(row=0, column=0, columnspan=2, pady=(0, 20))
        
        # Farbe (Hue) Slider
        ttk.Label(main_frame, text="Farbe:").grid(row=1, column=0, sticky=tk.W, pady=5)
        self.hue_slider = ttk.Scale(main_frame, from_=0, to=255, orient=tk.HORIZONTAL, 
                                     command=self.on_hue_change, length=300)
        self.hue_slider.set(0)
        self.hue_slider.grid(row=1, column=1, pady=5)
        
        self.hue_label = ttk.Label(main_frame, text="Rot")
        self.hue_label.grid(row=2, column=1, sticky=tk.W)
        
        # Sättigung Slider
        ttk.Label(main_frame, text="Sättigung:").grid(row=3, column=0, sticky=tk.W, pady=5)
        self.sat_slider = ttk.Scale(main_frame, from_=0, to=255, orient=tk.HORIZONTAL,
                                     command=self.on_sat_change, length=300)
        self.sat_slider.set(255)
        self.sat_slider.grid(row=3, column=1, pady=5)
        
        self.sat_label = ttk.Label(main_frame, text="100%")
        self.sat_label.grid(row=4, column=1, sticky=tk.W)
        
        # Helligkeit Slider
        ttk.Label(main_frame, text="Helligkeit:").grid(row=5, column=0, sticky=tk.W, pady=5)
        self.bright_slider = ttk.Scale(main_frame, from_=0, to=255, orient=tk.HORIZONTAL,
                                        command=self.on_bright_change, length=300)
        self.bright_slider.set(100)
        self.bright_slider.grid(row=5, column=1, pady=5)
        
        self.bright_label = ttk.Label(main_frame, text="39%")
        self.bright_label.grid(row=6, column=1, sticky=tk.W)
        
        # Farb-Vorschau
        self.color_preview = tk.Canvas(main_frame, width=300, height=50, bg="#FF0000")
        self.color_preview.grid(row=7, column=0, columnspan=2, pady=20)
        
        # Info
        info = ttk.Label(main_frame, text="Alle 62 LEDs werden gleichzeitig gesteuert", 
                        font=("Helvetica", 9), foreground="gray")
        info.grid(row=8, column=0, columnspan=2, pady=(10, 0))
    
    def on_hue_change(self, value):
        """Farbe wurde geändert"""
        self.hue = int(float(value))
        color_names = {
            0: "Rot", 21: "Orange", 42: "Gelb", 85: "Grün", 
            127: "Cyan", 170: "Blau", 212: "Lila", 234: "Magenta"
        }
        closest = min(color_names.keys(), key=lambda x: abs(x - self.hue))
        self.hue_label.config(text=color_names.get(closest, f"H: {self.hue}"))
        self.update_preview()
    
    def on_sat_change(self, value):
        """Sättigung wurde geändert"""
        self.saturation = int(float(value))
        percent = int((self.saturation / 255) * 100)
        self.sat_label.config(text=f"{percent}%")
        self.update_preview()
    
    def on_bright_change(self, value):
        """Helligkeit wurde geändert"""
        self.brightness = int(float(value))
        percent = int((self.brightness / 255) * 100)
        self.bright_label.config(text=f"{percent}%")
        self.update_preview()
    
    def update_preview(self):
        """Aktualisiere Farb-Vorschau"""
        r, g, b = hsv_to_rgb(self.hue, self.saturation, self.brightness)
        color = f"#{r:02x}{g:02x}{b:02x}"
        self.color_preview.config(bg=color)
    
    def update_leds(self):
        """Sende HSV-Werte an alle 62 LEDs"""
        # VialRGB DIRECT_FASTSET: Setze alle LEDs auf einmal
        # Format: [CMD, LED_START, LED_COUNT, H, S, V, H, S, V, ...]
        
        # Sende in Paketen (max 9 LEDs pro Paket wegen MSG_LEN=32)
        leds_per_packet = 9
        total_leds = 62
        
        for start_led in range(0, total_leds, leds_per_packet):
            led_count = min(leds_per_packet, total_leds - start_led)
            
            data = bytearray(MSG_LEN)
            data[0] = CMD_VIA_LIGHTING_SET_VALUE
            data[1] = VIALRGB_DIRECT_FASTSET
            data[2] = start_led
            data[3] = led_count
            
            # Füge HSV-Werte für alle LEDs hinzu
            for i in range(led_count):
                offset = 4 + (i * 3)
                data[offset] = self.hue
                data[offset + 1] = self.saturation
                data[offset + 2] = self.brightness
            
            self.dev.write(bytes([0]) + data)
    
    def periodic_update(self):
        """Periodisches Update (20 FPS)"""
        self.update_leds()
        self.root.after(50, self.periodic_update)
    
    def on_closing(self):
        """Aufräumen beim Schließen"""
        try:
            # Setze LEDs zurück auf schwarz
            self.hue = 0
            self.saturation = 0
            self.brightness = 0
            self.update_leds()
            self.dev.close()
        except:
            pass
        self.root.destroy()


def main():
    root = tk.Tk()
    app = LEDControlGUI(root)
    root.protocol("WM_DELETE_WINDOW", app.on_closing)
    root.mainloop()


if __name__ == "__main__":
    main()

