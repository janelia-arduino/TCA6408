- [Library Information](#org39bdf4b)
- [Host Computer Setup](#orge5dc9ac)

    <!-- This file is generated automatically from metadata -->
    <!-- File edits may be overwritten! -->


<a id="org39bdf4b"></a>

# Library Information

-   **Name:** TCA6408
-   **Version:** 0.1.0
-   **License:** BSD
-   **URL:** <https://github.com/janelia-arduino/TCA6408>
-   **Author:** Peter Polidoro
-   **Email:** peter@polidoro.io


## Description


<a id="orge5dc9ac"></a>

# Host Computer Setup


## Download this repository

<https://github.com/janelia-arduino/TCA6408.git>

```sh
git clone https://github.com/janelia-arduino/TCA6408.git
```


## PlatformIO


### Install PlatformIO Core

<https://docs.platformio.org/en/latest/core/installation/index.html>

```sh
python3 -m venv .venv
source .venv/bin/activate
pip install platformio
pio --version
```


### 99-platformio-udev.rules

Linux users have to install udev rules for PlatformIO supported boards/devices.

1.  Download udev rules file to /etc/udev/rules.d

    ```sh
    curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core/develop/platformio/assets/system/99-platformio-udev.rules | sudo tee /etc/udev/rules.d/99-platformio-udev.rules
    ```

2.  Restart udev management tool

    ```sh
    sudo service udev restart
    ```

3.  Add user to groups

    ```sh
    sudo usermod -a -G dialout $USER && sudo usermod -a -G plugdev $USER
    ```

4.  Remove modemmanager

    ```sh
    sudo apt-get purge --auto-remove modemmanager
    ```

5.  After setting up rules and groups

    You will need to log out and log back in again (or reboot) for the user group changes to take effect.
    
    After this file is installed, physically unplug and reconnect your board.


### Compile the firmware

1.  Gnu/Linux

    ```sh
    make teensy-firmware
    make mega-firmware
    make uno-firmware
    ```

2.  Other

    ```sh
    pio run -e teensy40
    pio run -e mega
    pio run -e uno
    ```


### Upload the firmware

1.  Gnu/Linux

    ```sh
    make teensy-upload
    make mega-upload
    make uno-upload
    ```

2.  Other

    ```sh
    pio run -e teensy40 -t upload
    pio run -e mega -t upload
    pio run -e uno -t upload
    ```


### Serial Terminal Monitor

1.  Gnu/Linux

    ```sh
    make monitor
    ```

2.  Other

    ```sh
    pio device monitor
    ```
