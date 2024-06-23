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

## C - Guide Config and Connection
### 1 - Firmware

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
