# Razer Nari Battery Level
## Research
I wanted to check the exact battery level of my wireless Razer Nari headset. Official Razer Synapse software (BTW only available on Windows) only shows a graphical battery indicator which is not very informative.<br/>
<img src="https://i.imgur.com/RmIfHqO.png"/>

Razer Nari communicates with a PC (host) through a USB dongle implementing some proprietary protocol in the 2.4 GHz ISM band.
With Wireshark and USBPcap I tried to observe frames exchanged between my PC running Windows and the dongle. I noticed that with Razer Synapse running every several seconds (sometimes tens of seconds) a recurring group of 4 frames occured.
| No. | Time      | Source | Destination | Protocol | Length | Info                |
|-----|-----------|--------|-------------|----------|--------|---------------------|
| 107 | 32.211417 | host   | 3.2.0       | USBHID   | 100    | SET_REPORT Request  |
| 108 | 32.212190 | 3.2.0  | host        | USBHID   | 28     | SET_REPORT Response |
| 109 | 32.323029 | host   | 3.2.0       | USBHID   | 36     | GET_REPORT Request  |
| 110 | 32.323992 | 3.2.0  | host        | USBHID   | 92     | GET_REPORT Response |

### 1. 'SET_REPORT Request' frame details:
```
Frame 107: 100 bytes on wire (800 bits), 100 bytes captured (800 bits) on interface wireshark_extcap904, id 0
USB URB
    [Source: host]
    [Destination: 3.2.0]
    USBPcap pseudoheader length: 28
    IRP ID: 0xffff8183b2ea4620
    IRP USBD_STATUS: USBD_STATUS_SUCCESS (0x00000000)
    URB Function: URB_FUNCTION_CLASS_INTERFACE (0x001b)
    IRP information: 0x00, Direction: FDO -> PDO
        0000 000. = Reserved: 0x00
        .... ...0 = Direction: FDO -> PDO (0x0)
    URB bus id: 3
    Device address: 2
    Endpoint: 0x00, Direction: OUT
        0... .... = Direction: OUT (0)
        .... 0000 = Endpoint number: 0
    URB transfer type: URB_CONTROL (0x02)
    Packet Data Length: 72
    [Response in: 108]
    Control transfer stage: Setup (0)
    [bInterfaceClass: HID (0x03)]
Setup Data
    bmRequestType: 0x21
        0... .... = Direction: Host-to-device
        .01. .... = Type: Class (0x1)
        ...0 0001 = Recipient: Interface (0x01)
    bRequest: SET_REPORT (0x09)
    wValue: 0x03ff
        ReportID: 255
        ReportType: Feature (3)
    wIndex: 5
    wLength: 64
    Data Fragment: ff0a00fd0412f10205000000000000000000000000000000000000000000000000000000…

0000   1c 00 20 46 ea b2 83 81 ff ff 00 00 00 00 1b 00   .. F............
0010   00 03 00 02 00 00 02 48 00 00 00 00 21 09 ff 03   .......H....!...
0020   05 00 40 00 ff 0a 00 fd 04 12 f1 02 05 00 00 00   ..@.............
0030   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
0040   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
0050   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
0060   00 00 00 00                                       ....
```

### 2. 'SET_REPORT Response' frame details:
```
Frame 108: 28 bytes on wire (224 bits), 28 bytes captured (224 bits) on interface wireshark_extcap904, id 0
USB URB
    [Source: 3.2.0]
    [Destination: host]
    USBPcap pseudoheader length: 28
    IRP ID: 0xffff8183b2ea4620
    IRP USBD_STATUS: USBD_STATUS_SUCCESS (0x00000000)
    URB Function: URB_FUNCTION_CONTROL_TRANSFER (0x0008)
    IRP information: 0x01, Direction: PDO -> FDO
        0000 000. = Reserved: 0x00
        .... ...1 = Direction: PDO -> FDO (0x1)
    URB bus id: 3
    Device address: 2
    Endpoint: 0x00, Direction: OUT
        0... .... = Direction: OUT (0)
        .... 0000 = Endpoint number: 0
    URB transfer type: URB_CONTROL (0x02)
    Packet Data Length: 0
    [Request in: 107]
    [Time from request: 0.000773000 seconds]
    Control transfer stage: Complete (3)
    [bInterfaceClass: HID (0x03)]

0000   1c 00 20 46 ea b2 83 81 ff ff 00 00 00 00 08 00   .. F............
0010   01 03 00 02 00 00 02 00 00 00 00 03               ............
```

### 3. 'GET_REPORT Request' frame details:
```
Frame 109: 36 bytes on wire (288 bits), 36 bytes captured (288 bits) on interface wireshark_extcap904, id 0
USB URB
    [Source: host]
    [Destination: 3.2.0]
    USBPcap pseudoheader length: 28
    IRP ID: 0xffff8183b36dd780
    IRP USBD_STATUS: USBD_STATUS_SUCCESS (0x00000000)
    URB Function: URB_FUNCTION_CLASS_INTERFACE (0x001b)
    IRP information: 0x00, Direction: FDO -> PDO
        0000 000. = Reserved: 0x00
        .... ...0 = Direction: FDO -> PDO (0x0)
    URB bus id: 3
    Device address: 2
    Endpoint: 0x80, Direction: IN
        1... .... = Direction: IN (1)
        .... 0000 = Endpoint number: 0
    URB transfer type: URB_CONTROL (0x02)
    Packet Data Length: 8
    [Response in: 110]
    Control transfer stage: Setup (0)
    [bInterfaceClass: HID (0x03)]
Setup Data
    bmRequestType: 0xa1
        1... .... = Direction: Device-to-host
        .01. .... = Type: Class (0x1)
        ...0 0001 = Recipient: Interface (0x01)
    bRequest: GET_REPORT (0x01)
    wValue: 0x03ff
        ReportID: 255
        ReportType: Feature (3)
    wIndex: 5
    wLength: 64

0000   1c 00 80 d7 6d b3 83 81 ff ff 00 00 00 00 1b 00   ....m...........
0010   00 03 00 02 00 80 02 08 00 00 00 00 a1 01 ff 03   ................
0020   05 00 40 00                                       ..@.
```

### 4. 'GET_REPORT Response' frame details:
```
Frame 110: 92 bytes on wire (736 bits), 92 bytes captured (736 bits) on interface wireshark_extcap904, id 0
USB URB
    [Source: 3.2.0]
    [Destination: host]
    USBPcap pseudoheader length: 28
    IRP ID: 0xffff8183b36dd780
    IRP USBD_STATUS: USBD_STATUS_SUCCESS (0x00000000)
    URB Function: URB_FUNCTION_CONTROL_TRANSFER (0x0008)
    IRP information: 0x01, Direction: PDO -> FDO
        0000 000. = Reserved: 0x00
        .... ...1 = Direction: PDO -> FDO (0x1)
    URB bus id: 3
    Device address: 2
    Endpoint: 0x80, Direction: IN
        1... .... = Direction: IN (1)
        .... 0000 = Endpoint number: 0
    URB transfer type: URB_CONTROL (0x02)
    Packet Data Length: 64
    [Request in: 109]
    [Time from request: 0.000963000 seconds]
    Control transfer stage: Complete (3)
    [bInterfaceClass: HID (0x03)]

0000   1c 00 80 d7 6d b3 83 81 ff ff 00 00 00 00 08 00   ....m...........
0010   01 03 00 02 00 80 02 40 00 00 00 03 ff 0f 05 fe   .......@........
0020   12 04 1f 08 05 03 05 02 0d e0 1e 6c 69 65 6e 74   ...........lient
0030   2c 32 30 31 39 2d 30 00 00 00 00 00 00 00 00 00   ,2019-0.........
0040   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
0050   00 00 00 00 00 00 00 00 00 00 00 00               ............
```

The 1-2 pair is a request-response pair and such is the 3-4. I suppose that these frames have the following meaning:
1. 'SET_REPORT Request'<br>
The host tells the device to prepare a report describing its current state.
2. 'SET_REPORT Response'<br>
The device tells if it managed to prepare the report.
3. 'GET_REPORT Request'<br>
The device is told to present the report to the host.
4. 'GET_REPORT Response'<br>
The host receives the report.

I was decoding and observing data in subsequent 'GET_REPORT Response' frames. When the green LED indicating full charge switched on and I disconnected the charging cable, bytes 40-41 (counting from 0) formed value 0de0 (hexadecimal) = 3552 (decimal) and when the headset went dead, they were equal to 3320. It led me to a conclusion that bytes 40-41 contain the headset's exact battery voltage in millivolts.

I created RazerNariBatteryLevel program used to fabricate 'SET_REPORT Request' and 'GET_REPORT Request' which are normally performed by Razer Synapse. Then RazerNariBatteryLevel decodes two aforementioned bytes and presents them as decimal battery voltage in millivolts.

## Building
RazerNariBatteryLevel uses libusb which is a C library that provides generic access to USB devices. You can install libusb on Ubuntu by running:
```
sudo apt install libusb-1.0-0-dev
```
Then build RazerNariBatteryLevel with GCC:
```
gcc RazerNariBatteryLevel.c -lusb-1.0 -o RazerNariBatteryLevel
```

## Running
Connect Razer Nari headset's USB dongle to your computer and run RazerNariBatteryLevel:
```
sudo ./RazerNariBatteryLevel
```

### Usage example
How the USB dongle looks like.<br/>
<img src="https://i.imgur.com/7i8HmNQ.jpg" width="500"/>

When the dongle is already connected to a PC but the headset isn't turned on yet, the dongle returns 0 millivolt battery voltage.
<img src="https://i.imgur.com/vE4Mpyl.jpg"/>

The headset has blue LED on shortly after it is turned on.<br/>
<img src="https://i.imgur.com/G1DQzHR.jpg" width="500"/>

It turns off after connecting to the dongle.<br/>
<img src="https://i.imgur.com/efieia4.jpg" width="500"/>

The reading of a discharging battery decreases everytime by 8 millivolts.<br/>
<img src="https://i.imgur.com/HeYQvFT.jpg"/>

When the charging cable is plugged in, the power supply pulls the voltage up to around 4.2 volts which is expected in order to charge a lithium-ion battery.<br/>
<img src="https://i.imgur.com/YcJ0ksX.jpg"/>

When the charging cable is disconnected, voltage drops to some value above that present when charging was started.<br/>
<img src="https://i.imgur.com/S02ROJO.jpg"/>
