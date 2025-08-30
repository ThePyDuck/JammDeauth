# JammDeauth (ESP8266 Auto DeAuth Tool)

**JammDeauth** is a fully automated **ESP8266 Wi-Fi jammer**.  
Once flashed, the ESP8266 will instantly start sending **deauthentication frames** to every Wi-Fi network it detects — no configuration needed.  

You can watch which networks it’s attacking using the **Arduino Serial Monitor** or any serial terminal.

⚠️ **This project is for educational and research purposes only!**  
Do not use it on networks you don’t own or without explicit permission. Misuse may be **illegal**.

---

## 🔥 Features

- **Plug and play**: just flash and power it up  
- **Automatic scanning + attacking** (no setup required)  
- **Serial Monitor feedback**: see SSIDs being targeted in real time  
- **Lightweight**: runs on any ESP8266 board (NodeMCU, Wemos D1 Mini, ESP-01, etc.)  

---

## 🛠 Requirements

- ESP8266 development board (e.g. NodeMCU, Wemos D1 Mini, ESP-01)  
- USB cable + PC  
- Flashing tool:  
  - [ESP8266Flasher (Windows)](https://github.com/nodemcu/nodemcu-flasher)  
  - [esptool.py (cross-platform)](https://github.com/espressif/esptool)  
  - or **Arduino IDE** (to build from source)  

---

## 🚀 Flashing the Firmware

### Option 1: Use Precompiled `.bin`  
1. Download the `.bin` from the `/firmware` folder or Visit Realeases to see advance versions 
2. Connect your ESP8266 to USB.  
3. Flash with your tool of choice:

   **Windows (ESP8266Flasher):**
   - Select COM port  
   - Load the `.bin`  
   - Click *Flash*  

   **Command line (esptool.py):**
   ```bash
   esptool.py --port COM3 write_flash 0x00000 JammDeauth.bin

*(replace `COM3` with your port and filename if different)*

4. Reboot the ESP8266 → it will instantly start jamming.

---

### Option 2: Build from Source (Arduino IDE)

1. Install [Arduino IDE](https://www.arduino.cc/en/software).
2. Add ESP8266 board support in *Preferences* → Board Manager URL:

   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
3. Open the code in `/src`.
4. Select your ESP8266 board (e.g. NodeMCU 1.0).
5. Hit **Upload**.

---

## 📡 Usage

* Just **power up the ESP8266** → it automatically scans and sends DeAuth packets to all Wi-Fi APs in range.
* Open **Serial Monitor** (115200 baud) to see the Wi-Fi Networks it’s attacking in real time.

Example output:

```
[SCAN] Found 5 networks:
==========================================
ID | RSSI(dBm) | CH | SSID
------------------------------------------
0  | -45    | 6  | MyHomeNetwork
1  | -62    | 11 | NeighborWiFi
2  | -68    | 1  | GuestNetwork
==========================================

[ATTACK] Targeting: MyHomeNetwork (RSSI: -45dBm) - Sending 10 packets
[ATTACK] Targeting: NeighborWiFi (RSSI: -62dBm) - Sending 8 packets
[ATTACK] Targeting: GuestNetwork (RSSI: -68dBm) - Sending 6 packets

[STATUS] Networks found: 15 | Active networks: 3 | Packets sent: 45 | Successful: 45
[TARGETS] Currently targeting: MyHomeNetw, NeighborWiF, GuestNetwork...
...
```

---

## ⚠️ Disclaimer

This project **interferes with Wi-Fi communications**. Running it on public or private networks without permission is **illegal** in many countries.
Only use in controlled environments for **educational, security testing, or research purposes**.

The author is **not responsible** for any misuse.

---

It's Written by ChatGPT :) nevermind
