# Warning and Measuring Noise Level from Outside Effect to Classroom
`⬇️ The project has the spiritual support of NTKA 💌 aka honey ⬇️`
- Overview about this project, this device measures the noise/sound level thourgh sound sensor (Simple) to give a warning (Text/LED !Not Any Sound) and use Wifi to send data to Cloud Server for Dashboard Management.
- In AWS, I use the service EC2 for deploy Server and DynamoDB for storing and querying data.
- This is the perfect combination of embedded systems, electronic circuits and IT.
- I just focus on Hardware and Firmware in this section description.
## A - Introduction Device
This device uses `STM32F103C8T6 for MCU` and `ESP12F for Wireless Connection`. ESP12F is the main factor of this description and helps STM32F103 connect Internet to send or recevive through HTTP protocol. The photo below is a combination of the components together. Additionally, it use `MAX4466(Sensor Microphone)` which has amplifier to measure sound around in the environment. To know which level is called "Noise" , it depends on the level and threshold you want to config (_Depend on your option_)

<p align="center">
  <img src="https://github.com/vinhdevED/SoundDeviceEmbedded/blob/main/assets/Hardware%20Connection.jpg" alt="Hardware Connection"/>
</p>

## B - Schematic Hardware
### Pin Connection Table
| **STM32F1** | **ESP12F**  | **DS3231**  | **MAX4466**  | **Description**        |
|-----------------|-----------------|-----------------|------------------|------------------------|
| 3.3V            | VCC             | VCC             | VCC              | Power supply (3.3V)    |
| GND             | GND             | GND             | GND              | Ground                 |
| PB6 (I2C1_SCL)  | -               | SCL             | -                | I2C Clock              |
| PB7 (I2C1_SDA)  | -               | SDA             | -                | I2C Data               |
| PA9 (USART1_TX) | RXD             | -               | -                | UART Transmit          |
| PA10 (USART1_RX)| TXD             | -               | -                | UART Receive           |
| PA0 (ADC_IN0)   | -               | -               | OUT              | Audio input from MAX4466|

- **STM32F1**: Uses PB6 and PB7 for I2C communication with DS3231 (real-time clock). PA9 and PA10 are used for UART communication with ESP12F (Wi-Fi module). PA0 is used for analog input from the MAX4466 (microphone amplifier).
- **ESP12F**: Communicates with STM32F1 via UART using TXD and RXD.
- **DS3231**: Real-time clock module uses I2C communication with STM32F1.
- **MAX4466**: Outputs audio signal to STM32F1’s ADC pin (PA0).

### ESP12F Module Wifi
The pictures show the schematic and hardware board of the WiFi module, respectively on the left and right. 
<p align="center">
  <img src="https://github.com/vinhdevED/SoundDeviceEmbedded/blob/main/assets/ESP12F%20Schematic.jpg" alt="Ảnh 1" width="400"/>
  <img src="https://github.com/vinhdevED/SoundDeviceEmbedded/blob/main/assets/ESP12F-Module.png" alt="Ảnh 2" width="300"/>
</p>

> ⚠️ **Important:** You need to use an external 3.3V power source.

The ESP12F has three different boot modes selected by the state of **GPIO15**, **GPIO0**, and **GPIO2** at start-up.
| MODE                  | GPIO_15 | GPIO_0 | GPIO_2       |
|-----------------------|---------|--------|--------------|
| SDIO (Boot SD Card)    | 1       | x      | x            |
| UART (Upload Code)     | 0       | 0      | x or 1       |
| FLASH (Normal Running) | 0       | x or 1 | x or 1       |
- Sign "x or 1" must be left either floating or pulled up to 3.3V.
- The "chip enable" (EN/CH_PD) pin has to be pulled up to 3.3V. Otherwise, the ESP12F will not run.

## C - Guide Config and Connection
### 1 - Firmware
//TODO: Update earlier in future
### 2 - Cloud Server AWS
- First time, i run my server NodeJS in local but it must have same Wifi Connection (I dont want that). So, i use AWS EC2 to deploy this local server to cloud server which can overcome the problem of not working everywhere. (Dasboard Website/App).
- In this section, i just do add this parameter to file `.env`
```
AWS_ACCESS_KEY_ID=XXXXXXXXXXXXXBCDEFGHIK.
AWS_SECRET_ACCESS_KEY=XXXXXXXXXXXXXBCDEFGHIK132323232132132.
AWS_REGION=your-region.
EXPRESSPORT=your-port.
```
## D - Demo
<p align="center">
  <img src="https://github.com/vinhdevED/SoundDeviceEmbedded/blob/main/assets/End%20Device.jpg" alt="Hardware Connection"/>
</p>
