# Senior Design Project  
## Table of Contents
- [Visual Studio Code set up](https://github.com/dsanborn16/SeniorDesignProject?tab=readme-ov-file#setting-up-vs-code) (reccomended)  
- [Arduino IDE set up](https://github.com/dsanborn16/SeniorDesignProject?tab=readme-ov-file#setting-up-arduino-ide)
- [MQTT set up](https://github.com/dsanborn16/SeniorDesignProject?tab=readme-ov-file#setting-up-mqtt) 
  
## Setting up VS Code  
### Install VS Code  
If you don't have it already, download and install Visual Studio Code from the [official website](https://code.visualstudio.com/).  

---
### Install PlatformIO IDE  
- Open VS Code
- Go to the Extensions view by clicking on the Extensions icon in the Activity Bar on the side of the window or by pressing `Ctrl+Shift+X`
- Search for "PlatformIO IDE" and install it
- After installation, you may be prompted to reload VS Code
  
### Creating a New PlatformIO Project:  
---
### Open PlatformIO Home
Click on the PlatformIO icon in the VS Code Activity Bar (it looks like an alien head). This will open the PlatformIO Home screen.  

---
### Create a new project
- From the PlatformIO Home screen, click on "**New Project**"
- A "Project Wizard" window will pop up
- **Name**: Give your project a name (e.g., "ESP32-Scale")
- **Board**: Select your ESP32 board. I used "Espressif ESP32 Dev Module"
- **Location**: You can use the default location or choose a different folder for your project.
- Click "**Finish**"  
PlatformIO will now create a new project with a specific folder structure. The most important files for us will be:  
- `src/main.cpp`: This is where our main code will go
- `platformio.ini`: The project configuration file
---
### Adding the HX711 Library
The project requires a library to communicate with the HX711 amplifier. Here's how to add it in PlatformIO:

- **Open the PlatformIO Home screen**
- Click on "**Libraries**" in the left-hand menu
- In the search bar, type "**HX711 Arduino Library**" and press Enter
- Find the library by **Bogdan Necula** and click on it
- Click the "**Add to Project**" button
- Select your "ESP32-Scale" project from the dropdown menu and click "**Add**"  

PlatformIO will automatically download the library and add it to your project's dependencies in the `platformio.ini` file.  

---
### Configuring the Project  
Now we need to make a small change to the `platformio.ini` file. This file is used to configure the project's settings.

- Open the `platformio.ini` file in your VS Code project.
- Add the following line to the file to set the serial monitor speed:
```
monitor_speed = 115200
```
Your `platform.ini` file should look something like this:
```
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
lib_deps =
    bogde/HX711
```  
---
### Uploading the code  
Now you are ready to upload the code to your ESP32.

- **Connect your ESP32** to your computer via USB
- In VS Code, click the **PlatformIO: Upload** button in the status bar at the bottom of the window (it looks like a right-pointing arrow)
- PlatformIO will now compile the code and upload it to your ESP32. You can see the progress in the terminal window at the bottom of the screen

## Setting up Arduino IDE  
### Add the ESP32 Board Manager URL

- Open the Arduino IDE
- Go to **File > Preferences**
- In the "Additional Board Manager URLs" field, enter the following URL:
```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```
- If you already have other URLs in this field, you can separate them with a comma
- Click **OK**

---
### Install the ESP32 Board Package  

- Go to **Tools > Board > Boards Manager...**
- In the search bar, type "**esp32**"
- You should see "esp32 by Espressif Systems". Click the **Install** button
- The installation process will begin. It may take a few minutes to complete

---
### Select Your ESP32 Board  

- Once the installation is complete, close the Boards Manager window
- Go to **Tools > Board > ESP32 Arduino**
- You will now see a list of ESP32 boards. Select the specific board you are using (e.g., "ESP32 Dev Module")
- Go to **Tools > Port** and select the COM port that your ESP32 is connected to.  

After following these steps, your Arduino IDE should recognize your ESP32 board, and you'll be able to upload sketches to it.  

**If your board is still not recognized, you likely need to install some drivers - see below**  

## Setting up MQTT