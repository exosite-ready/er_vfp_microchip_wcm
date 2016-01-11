# Summary
This is an Exostie Embedded Client Demo using Microchip's WCM board
For released binaries clone the Release binary repo from git@i.exosite.com:exositeready/er_release_binaries.git
The releases for this device can be found in the microchip_wcm folder

# Demo Description
The demo does the following:
  1.) The client will activate the device is not already activated.
      If the device config memory does not contain a valid configuration
      (i.e after a firmware update) then make sure that the device
      is not yet activated, otherwise the activation will fail and 
      the device won't work
  2.) It Checks the state of 4 buttons on the WCM Board (S1..S4) and
  sends a notification to the server if a button is pressed.
  The notification value is 1 for the 1st button press, 0 for the
  next button press, then 0 and so on.

  3.) Parallel, it subscribes to the "led" data resource and toggles
  four LEDs (D1 .. D4) if it is set on the server side. 
  The value of "led" contains the (desired) binary encoded states of the 
  four LEDs, e.g.:
    value = 1 turns on 1st led
    value = 2 turns on 2nd led
    value = 3 turns on 1st and 2nd led
    value = 4 turns on 3rd led
    value = 5 turns off 1st and 3rd led
    value = 6 turns off 2nd and 3rd led
    value = 7 turns off 1st, 2nd and 3rd led
    value = 8 turns off 4th led

   4.) It sends the value of the potentiometer every 5 seconds

# Install and build
This demo application needs Microchip's WCM SDK!!! 
This SDK shall be installed to a given path so that the project file in the application can find it!!!
MPLABX and XC32 compiler have to installed separately, the MPLAB installer won't install XC32
XC32 1.34 is needed to build this application!!!

1. git clone https://github.com/exosite-ready/er_vfp_microchip_wcm.git
2. cd er_vfp_microchip_wcm
3. git submodule update --init --recursive
4. Download Microchip's WCM SDK from [WCM Firmware  01.00.00](http://www.microchip.com/mymicrochip/filehandler.aspx?ddocname=en571775)
5. Install this SDK by unzipping WCM_DK1_01.00.00.zip into a folder named '3rd_party' relative to the ../er_vfp_microchip_wcm folder
   Example:
     - If the client is cloned to: /opt/er_vfp_microchip_wcm
     - Then the WCM DK should be in /opt/3rd_party/WCM_DK1_01.00.00
6. run ./install.sh from the er_vfp_microchip_wcm folder
7. Open the project EmbeddedClient-Microchip-WCM from MPLAB
8. Build and download the firwmare with MPLAB
9. Find detailed instruction on how to Download and use a binary below

# Hardware Setup
1. Connect a PICkit3 to the WCM's "ICSP" connector.
2. Connect a USB cable to the WCM's "J3" connector.
3. (Optional) Connect a USB to TTL UART converter to "J2". J2.GND-GND. J2.RF5-RXD.
3. Turn on power switch ("S5").

# Flashing the Firmware
1. Clone the release binaries repository from git@i.exosite.com:exositeready/er_release_binaries.git
   (git clone git@i.exosite.com:exositeready/er_release_binaries.git)
2. Open "MPLAB IPE" v2.35 or greater
3. Select "Device" -> PIC32MX695F512H
4. Press "Connect" button.
5. You should see the message "Connecting to MPLAB PICkit 3..."
6. Press "Browse" button, and choose "er_release_binaries/er_plat_microchip_wcm/vfp/ec_microchip_wcm-x.y.z.hex",
   where x.y.z is the version number.
7. Press "Program" button.
8. After programming, WCM D1 will be on; if the board could connect with wifi to an AP D5 will be on, D1 will be off

# Network Setup
1. Connect to the SSID "xxxxxx_WCM", take note of the value of the "xxxxxx" part, you will need it later.
   Note: If you SSID does not match this format you'll need to up the WiFi module's firmware.
2. Open web browser, and navigate to http://192.168.1.25
3. Input your AP router information.

# Debug Terminal (Optional):
1. Open serial terminal application with baudrate of 115200.
2. Press WCM S6. Observe MAC information if needed.

# Cloud Setup
1. Go to http://microchip.exosite.com
2. Add a "WCM Development Kit 1" device with its MAC address, the first 6 digits will be auto filled and the last 6 are the "xxxxxx" portion of the SSID you connected to earlier.
3. If the board fails to activate (the following message shows up in the terminal: "Error [-106]: Activate request returned error") you may need to re-enable the client in Portals or press the reset button on the board.

If you see "Error [-107]: Activate request returned error" this means your device was not yet added to the Server; please follow the instructions in Cloud setup: step 1 and 2

# Resetting AP setting
1. Turn off the board
2. Press S1 and keep pressing while turning on the board
3. Turn on the board
4. Keep pressing S1 until D1 is turned on
5. Now you can reconfigure the AP settings with the method in chapter #Network Setup

