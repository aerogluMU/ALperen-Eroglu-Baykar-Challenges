
# Alperen-Eroglu-Baykar-Challenge
## Challenge 1
- By using **Thread**, the LEDs are made to blink at intervals of seconds as the **"shutter_period_time"** value in the config.json file.
- Added button **interrupt** and when the interrupt is triggered, **timestamp** value is taken and this value is saved in ts_buffer array.
- By using **Thread**, the values in ts_buffer are saved to the **shutter.csv** file at intervals of seconds (if a button interrupt occurred) as much as the **"shutter_period_time"** value in the config.json file.

### Pinouts

- Led 1 (RED) = 7;
- Led 2 (Green) = 0;
- Button = 2;

> Mode of Leds --> Out
> Mode of Button --> In
> PUPD of Button --> Pull Up

### Config.json

    {"shutter_period_time":1}

### System Images

![System](https://i.ibb.co/GPPqQVg/1654244250046.jpg)
![Button Interrupt](https://i.ibb.co/jkR0xM9/1654244250016.jpg)
![Saved timestamps in shutter.csv](https://i.ibb.co/PG6VCc1/1654244249968.jpg)
![Video of the system in operation](https://im3.ezgif.com/tmp/ezgif-3-93caae81eb.gif)

### FlowChart
```mermaid
graph TD
    A[System Start] -->B[Initialization]
    B --> C(Read shutter_period_time value in config.json)
    C --> T1[thread_shutter_pulse begin]
    C --> T2[thread_shutter_record begin]
    C --> INT[button_callback begin]

    T1 --> T11(Open LED1 and LED2)
    T11 --> T1D1(Wait seconds at shutter_period_time)
    T1D1 --> T12(Close LED1 and LED2)
    T12 --> T1D2(Wait seconds at shutter_period_time)
    T1D2 --> T11

    T2 --> T2INT{Is it_occured value set?}
    T2INT --> |Yes| T2SAVE(Save values from ts_buffer array to shutter.csv)
    T2SAVE --> T2CLEAR(Clear it_occured flag and ts_buffer array)
    T2CLEAR --> T2D1(Wait seconds at shutter_period_time)
    T2D1 --> T2INT
    T2INT --> |No| T2D2(Wait seconds at shutter_period_time)
    T2D2 --> T2INT

    INT --> INT_ISPRESSED{Is button pressed?}
    INT_ISPRESSED --> |Yes| INT_ISPRESSED_Y(Save current timestamp value to ts_buffer array and print it with cout)
    INT_ISPRESSED_Y --> INT_ISPRESSED_1(set it_occured flag)
    INT_ISPRESSED_1 --> INT_ISPRESSED
    INT_ISPRESSED --> |No| INT_ISPRESSED
    
```

## Challenge 2

- Added 9 pin header for SD card connections from camera.
- Added SD card socket to save data from camera to SD card.
- Added GL827L integrated circuit and USB socket to transfer the data from the camera to the computer.
- 74LVC1G18 demultiplexers have been added to transfer the data from the camera to SD card or USB on demand.

### 74LVC1G18 Work Structure

![74LVC1G18 Function Table](https://i.ibb.co/sRHkNQJ/74-LVC1-G18-FT.png)

> A --> Camera Pins
> S --> MCU Selection
> 1Y --> SD Card
> 2Y --> SD Card Reader (USB)

As we can see from the table, if the selective value from the MCU pin **is equal to 0**, the data from the camera will be transferred to the **SD card**, if it **is equal to 1**, the data from the camera will be transferred to the **USB**.

### Schematic

![Schematic](https://i.ibb.co/S5JK3Ft/BYKR-C2-V00-SCHEMATIC.jpg)

### PCB

![PCB](https://i.ibb.co/1Mdy0rn/BYKR-C2-V00-PCB-page-0001.jpg)

### 3D PCB

![BYKR-C2-V00 TOP](https://i.ibb.co/LdnB4Q1/1.png)

![BYKR-C2-V00 BOTTOM](https://i.ibb.co/cr9c7gD/2.png)
