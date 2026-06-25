# HOME_HUB

## 📖 Overview

HOME_HUB is an IoT-based home security system built using ESP8266 microcontrollers. The system consists of multiple sensor nodes communicating with a central hub using the ESP-NOW protocol. The hub connects to Wi-Fi and securely uploads sensor events to AWS IoT Core using MQTT over TLS.

This project is designed to provide a low-power, reliable, and scalable wireless security solution for home automation and monitoring.

---

## ✨ Features

* ESP-NOW communication between sensor nodes and hub
* Secure MQTT communication with AWS IoT Core
* Wi-Fi connectivity
* OTA (Over-The-Air) firmware updates
* NTP time synchronization
* Heartbeat monitoring
* JSON-based message formatting
* Modular code architecture
* Automatic node event forwarding to the cloud

---

## 🏗 System Architecture

```
+-----------------+
| Sensor Node 1   |
+-----------------+
        |
        | ESP-NOW
        |
+-----------------+
| Sensor Node 2   |
+-----------------+
        |
        |
        ▼
+----------------------+
|      HOME HUB        |
|  ESP8266 NodeMCU     |
+----------------------+
        |
        | Wi-Fi
        |
        ▼
+----------------------+
| AWS IoT Core         |
+----------------------+
        |
        ▼
 Cloud Applications
```

---

## 📁 Project Structure

```
HOME_HUB/
│
├── src/
│   ├── main.cpp
│   ├── espnow_manager.cpp
│   ├── mqtt_manager.cpp
│   ├── ota_manager.cpp
│   ├── heartbeat.cpp
│   └── ...
│
├── include/
│   ├── espnow_manager.h
│   ├── mqtt_manager.h
│   ├── ota_manager.h
│   ├── heartbeat.h
│   └── ...
│
├── certs/
│   ├── device_cert.h
│   ├── private_key.h
│   └── aws_root_ca.h
│
├── platformio.ini
└── README.md
```

---

## 🔧 Hardware Requirements

* ESP8266 NodeMCU (Hub)
* ESP8266 NodeMCU / ESP32 (Sensor Nodes)
* Door/Window Magnetic Switch
* PIR Motion Sensor (Optional)
* Buzzer (Optional)
* LEDs (Status Indication)
* Wi-Fi Router

---

## 💻 Software Requirements

* Visual Studio Code
* PlatformIO IDE
* AWS IoT Core Account
* Git

---

## 🚀 Getting Started

### 1. Clone the repository

```bash
git clone https://github.com/yourusername/HOME_HUB.git
cd HOME_HUB
```

### 2. Open in VS Code

Open the project using Visual Studio Code with the PlatformIO extension installed.

### 3. Configure Wi-Fi

Update your Wi-Fi credentials.

```cpp
const char* WIFI_SSID = "YOUR_WIFI";
const char* WIFI_PASSWORD = "YOUR_PASSWORD";
```

### 4. Configure AWS IoT

Add:

* Root CA
* Device Certificate
* Private Key
* AWS Endpoint

### 5. Build

```bash
pio run
```

### 6. Upload

```bash
pio run --target upload
```

### 7. Monitor Serial Output

```bash
pio device monitor
```

---

## 🔐 Security

* MQTT over TLS
* AWS IoT Device Certificates
* Secure ESP-NOW communication
* OTA firmware update support

---

## 📡 Communication Flow

```
Sensor Trigger
      │
      ▼
ESP-NOW Packet
      │
      ▼
HOME HUB
      │
      ▼
JSON Formatting
      │
      ▼
MQTT Publish
      │
      ▼
AWS IoT Core
```

---

## 📊 Current Status

* ✅ ESP-NOW Communication
* ✅ Wi-Fi Connectivity
* ✅ MQTT Integration
* ✅ AWS IoT Connection
* ✅ OTA Updates
* ✅ NTP Synchronization
* ✅ Heartbeat Monitoring
* 🚧 Mobile Dashboard (Planned)
* 🚧 Multiple Sensor Types (Planned)

---

## 🛠 Future Improvements

* ESP32 Support
* Local Web Dashboard
* Mobile Application
* Encrypted Sensor Payloads
* Battery Monitoring
* Event History Storage
* AI-based Intrusion Detection
* Raspberry Pi Local Server Integration

---

## 🤝 Contributing

Contributions are welcome.

1. Fork the repository.
2. Create a feature branch.
3. Commit your changes.
4. Push to your branch.
5. Open a Pull Request.

---

## 📄 License

This project is licensed under the MIT License.

---

## 👨‍💻 Author

**Sumit Kumar**

Embedded Systems Engineer

Skills:

* Embedded C/C++
* ESP8266 / ESP32
* STM32
* IoT Development
* MQTT
* ESP-NOW
* AWS IoT Core
* PCB Design
