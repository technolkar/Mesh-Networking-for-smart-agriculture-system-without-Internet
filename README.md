<div align="center">

<img src="https://capsule-render.vercel.app/api?type=waving&color=0:134E5E,100:71B280&height=200&section=header&text=Offline%20Mesh%20Farm%20Monitor&fontSize=40&fontColor=ffffff&desc=Smart%20agriculture%20with%20no%20internet%20needed&descAlignY=72&descSize=18&animation=fadeIn" width="100%" alt="Offline Mesh Farm Monitor banner"/>

### 📡 Sensor nodes that talk to each other — and water the field on their own.

![ESP8266](https://img.shields.io/badge/NodeMCU%20ESP8266-E7352C?style=for-the-badge&logo=espressif&logoColor=white)
![painlessMesh](https://img.shields.io/badge/painlessMesh-134E5E?style=for-the-badge)
![C++](https://img.shields.io/badge/Arduino%20C%2B%2B-00599C?style=for-the-badge&logo=arduino&logoColor=white)
![JSON](https://img.shields.io/badge/ArduinoJson-000000?style=for-the-badge&logo=json&logoColor=white)
![OLED](https://img.shields.io/badge/SSD1306%20OLED-444444?style=for-the-badge)

![Offline](https://img.shields.io/badge/internet-not%20required-71B280?style=flat-square)
![Year](https://img.shields.io/badge/built-2021-134E5E?style=flat-square)

</div>

---

## 📌 At a glance

> Many farms have **no reliable internet**. This system builds its **own wireless network**: several NodeMCU sensor nodes form a **self-organising mesh**, share soil and climate readings with each other, show them on a small **OLED screen**, and **switch the water pump on automatically** when the soil gets dry.

<div align="center">
  <img src="IMG20210725023643.jpg" width="85%" alt="Several NodeMCU sensor nodes with OLED displays on the bench"/>
  <br/><sub><i>The mesh on the bench — each node with its own sensors and OLED display</i></sub>
</div>

---

## ✨ What it does

<table>
<tr>
<td width="25%" align="center" valign="top">

### 🌐 Builds its own network
Nodes join a **painlessMesh** network automatically — no router, no internet.

</td>
<td width="25%" align="center" valign="top">

### 🌡️ Measures
**Soil moisture**, **temperature** and **humidity** (DHT11) at every node.

</td>
<td width="25%" align="center" valign="top">

### 📢 Shares
Each node **broadcasts its readings as JSON** to the whole mesh.

</td>
<td width="25%" align="center" valign="top">

### 🚿 Acts
If the soil reads **dry**, the node **switches its pump on**; when wet, off.

</td>
</tr>
</table>

---

## 🔄 How it works

```mermaid
flowchart LR
    subgraph N1[Node 1]
        S1[Soil + DHT11] --> M1[NodeMCU]
        M1 --> O1[OLED]
        M1 --> P1[💧 Pump]
    end
    subgraph N2[Node 2]
        S2[Soil + DHT11] --> M2[NodeMCU]
        M2 --> O2[OLED]
        M2 --> P2[💧 Pump]
    end
    subgraph N3[Node 3]
        S3[Soil + DHT11] --> M3[NodeMCU]
        M3 --> O3[OLED]
    end
    M1 <-.->|mesh| M2
    M2 <-.->|mesh| M3
    M1 <-.->|mesh| M3
```

Every node is equal: there is no central server that can fail. A node that can't reach another directly passes the message through its neighbours.

---

## 🔌 Circuit

<div align="center">
<img src="mesh%20networking%20circuit.jpg" width="75%" alt="Circuit of one mesh node"/>
<br/><sub><b>One node:</b> NodeMCU · DHT11 · soil-moisture sensor · OLED · relay and pump</sub>
</div>

| Part | Pin |
|---|---|
| DHT11 data | GPIO 2 |
| Soil-moisture sensor | A0 |
| Pump relay | GPIO 12 |
| Manual switch | GPIO 13 |
| OLED (SSD1306, I²C) | SDA / SCL, address 0x3C |

---

## 💻 Code

[`Mesh_Networking.ino`](Mesh_Networking.ino):

```cpp
void sendMessage() {
  DynamicJsonDocument doc(1024);
  doc["MOISTURE"]    = analogRead(A0) <= 1000 ? "Wet" : "Dry";
  doc["TEMPERATURE"] = dht.readTemperature();
  doc["HUMIDITY"]    = dht.readHumidity();
  String msg; serializeJson(doc, msg);
  mesh.sendBroadcast(msg);            // every node hears it
}

void loop() {
  if (analogRead(A0) <= 1000) digitalWrite(trigPin, LOW);   // wet → pump off
  else                        digitalWrite(trigPin, HIGH);  // dry → pump on
  mesh.update();
}
```

<details>
<summary><b>▶ How to run it</b></summary>

1. Install **painlessMesh**, **ArduinoJson**, **DHT sensor library** and **Adafruit SSD1306** in the Arduino IDE
2. Set your own `MESH_PREFIX` and `MESH_PASSWORD` — the same on every node
3. Upload the sketch to two or more NodeMCUs
4. Power them up: they find each other and start sharing readings on the Serial Monitor (115200 baud)

</details>

---

## 🧠 What I learned

- 📡 **Mesh networking** — self-healing, router-free communication between microcontrollers
- 🧾 **Structured messages** with JSON on tiny devices
- 🌱 **Closed-loop control** — sensing soil moisture and acting on it automatically

---

<div align="center">

### 👋 Built by **Ajay Arjun Mannolkar**
[![Portfolio](https://img.shields.io/badge/Portfolio-technolkar.github.io-134E5E?style=for-the-badge&logo=githubpages&logoColor=white)](https://technolkar.github.io)
[![LinkedIn](https://img.shields.io/badge/LinkedIn-ajaymannolkar-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/ajaymannolkar)
[![YouTube](https://img.shields.io/badge/YouTube-TECHNOL%20KAR-FF0000?style=for-the-badge&logo=youtube&logoColor=white)](https://www.youtube.com/@TechnolkarInnovations)

<img src="https://capsule-render.vercel.app/api?type=waving&color=0:71B280,100:134E5E&height=100&section=footer" width="100%"/>

</div>
