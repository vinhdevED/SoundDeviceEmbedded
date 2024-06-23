# Warning and Measuring Noise Level Effect to Classroom
- Overview about this project, this device measures the noise/sound level thourgh sound sensor (Simple) to give a warning (Text/LED !Not Any Sound) and use Wifi to send data to Cloud Server for Dashboard Management.
- I just focus on Hardware and Firmware in this section description
## A - Introduction Device

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
