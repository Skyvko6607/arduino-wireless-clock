
# Arduino Wireless Clock

## **Information**

>Name: *Arduino Wireless Clock*
>
>Language: **Hungarian**
>
>Purpose: *Common home aesthetic item*

## **Description**

>**Features:**
 - Retrieving time from a wireless connection.
 - Commands which can be entered through **Serial Monitor** on the **Nano**.
 - Commands which have been entered on the **Nano** will communicate corresponding to the command with the **NodeMCU**.
>**Requirements:**
 - Nearby wireless connection.
 - **WiFi** requires a password in order for the **NodeMCU** to connect.
 - Arduino Nano
 - NodeMCU (could use an ESP07, but I only have a NodeMCU)
 - 4x **7 segment display**
 - 4x **PNP Transistor with 5V Base-Emitter voltage**
 - Wires
>**Commands:** (requires the '>' character in the command)
 - \>sethostname \<wifi_ssid> - Sets the **SSID**.
 - \>setpassword \<wifi_password> - Sets the **WiFi Password**.
 - \>cleardata - Clears the **SSID** and **Password** from the internal memory. (*EEPROM*) 
 - \>gettime - Sends a *GetTime* request to the **NodeMCU** which will get the *Epoch* time from the internet and return it.
>**Library Requirements:**
 - Time Library by Michael Margolis (Found in the Library Manager in the Arduino IDE or can be downloaded from [here](https://github.com/PaulStoffregen/Time))
 - [SevSeg Library](https://github.com/DeanIsMe/SevSeg)
 - ESP8266 Library (Found in the Library Manager in the Arduino IDE)
>**Schematic**
![](https://github.com/Skyvko6607/arduino-wireless-clock/blob/master/screenshots/Schematic.png)
