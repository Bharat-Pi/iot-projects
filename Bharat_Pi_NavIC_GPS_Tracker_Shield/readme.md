<img width="868" alt="Screenshot 2024-08-06 at 9 07 26 AM" src="https://github.com/user-attachments/assets/0f353bfc-638b-41fe-bffd-b7df93223f1d">

## Introduction
Bharat Pi NavIC GPS Tracker Shield helps to build rapid GPS tracking solutions especially for India specific tracker applications. Bharat Pi NavIC module is AIS140 certified and you can directly use it for building automotive tracking applications. 

The NavIC shield is designed to work on all Bharat Pi boards like 4G, LoRa etc. and you can mount it as a shield. Refer the image below on how to mount the shield on top of Bharat Pi boards. Below is an example of a 4G board mounted with the NavIC shield.

![Bharat-Pi-NavIC-Shield-Mount-Instructions](https://github.com/user-attachments/assets/c440d976-1970-406a-b53e-71de0ef6cc89)

## USB based NavIC GPS Tracker
If you intend to use Bharat Pi NavIC module as a standalone USB based GPS tracker device then you can connect a USB-TTL converter and directly get data from the USB to your desktop/laptop tracker softwares. Ensure to connect the 5V and GND pins and as well the TX pin of NavIC to RX pin of USB-TTL and the RX pin of NavIC to TX ping of USB-TTL as show in below image. Incorrect/reverse power and ground connections might short the NavIC device so please ensure the power (5V) and ground (GND) pins of NavIC and USB-TTL are connected properly. 

![NavIC-using-USB-TTL-Convertor](https://github.com/user-attachments/assets/4c566dfa-77e8-4646-8277-b37e37ce1995)

## IMPORTANT: 
When used as shield the device uses hardware serial and you need to pay attention while using this code with 4G module. There is a seperate code for [Bharat Pi 4G board + NavIC shield](https://github.com/Bharat-Pi/iot-projects/tree/main/Bharat_Pi_NavIC_GPS_Tracker_using_SIMCOM_4G_Module_A7672S) for pushiing data to cloud in real time. You might want to use that directly than porting this code on to 4G.

## Bharat Pi NavIC Shield Datasheet
[Download](https://bharatpi.net/wp-content/uploads/2024/04/Bharat-Pi-NavIC-Module-Datasheet.pdf)

## LICENSE TYPE:
MIT License

Copyright (c) [year] [fullname]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
