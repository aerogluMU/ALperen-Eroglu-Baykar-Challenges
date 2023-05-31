Challenge 1

Create a program that implements a multithreaded architecture in Linux with C++ or C programming.  You should implement this for real as much as you can.  We want to see how far you can take this.

The architecture comprises 4 parts (2 threads , 1 interrupt sequence, and 1 bonus). The purpose of this program is to simulate the shutter system in our device. The shutter system includes the shuttering signal to cameras, receiving the feedback from the camera, and recording the timestamp of each shutter.  You should compile your code in Linux, and then share your code and results.

The first part is a periodic thread. Currently, our system has cameras to which we need to send a shutter pulse signal so they can all shutter at the same time.  In your challenge, you don’t have cameras, so you can send a signal to light up LEDs instead of cameras. You should send a “shutter pulse” signal once a second. The period of the signal (defaulted to 1 second) should be configurable and drawn from a file called config.json file.  If we to wish change the shutter signal period, we should need to change just the “shutter period time” key’s value in config.json.
The second part is an interrupt sequence. The cameras are connected to the Raspberry Pi (RPI) controller through the cameras’ hotshoe, which sends a pulse to the RPI when the camera actually shutters (feedback signal). That is not exactly simultaneous with the shutter signal; the camera has around a 150-200ms delay from the invocation of the shutter signal. Therefore, the system has an interrupt sequence for each camera to record after the shutter actually happens. To simulate that, you can use a push-button circuit and record the timestamp of each push through an interrupt sequence. 
The third part is  another periodic thread.  Save all of the recorded feedback signals' timestamps every second to a file called shutter.csv.  Since we have many cameras that send feedbacks, we avoid writing to the file in the interrupt sequence to avoid any conflicts and control order. This thread takes all recorded timestamps in the memory during the period (1 sec) and saves them in the file and clears the memory.
The bonus part is a continuous thread.  As soon as shutter.csv is written to from the feedback signal, your device (which could be an RPI, Pico, Jetson Nano, etc) should raise an interrupt to an ARM chip (we suggest you use STM32F4 microcontrollers).  The reason we use an ARM chip ourselves is that our device (RPI, Pico, etc.) does not have enough GPIO and comm pins for our activities, so we need extra support.  In any event, you should run a function on the ARM chip (could be as simple as a counter) upon receipt of the interrupt. The communication between the RPI and ARM should be via SPI or I2C.  The function that the ARM runs could be a counter, temperature reader, etc.


Challenge 2

You have to prepare to design a PCB. You should get to know SD card socket pin architecture (9 pins) and SD card reader pin architecture if you haven’t already.  

After taking pictures on our DSLR camera and writing those pictures to an SD card housed inside of it, we want to transfer all those pictures to another device like a PC.  We assert the following architecture in order to effect this.

![](https://ibb.co/hgTrpv2)

To do this, we must, after capturing the images, change the active SD card pin connection from an SD card socket inside the camera to a  (USB)  SD card reader, so that we can transfer all images by means of the SD card reader.  If you don’t switch all SD card pins properly, the DSLR camera will likely  fault  to an  error mode and therefore, shuttering and saving of new images to the SD card  will be impossible.  You will need to ensure that at any time, the SD  card pins are connected to either the SD card socket pins or to the  (USB) SD card reader’s pins.

You should use a switch component that is activated when an SD card mode signal status is delivered from an ARM chip. Depending upon the ARM chip SD card mode, the active SD card will be either the one in the SD card socket on the camera, or the one in the  (USB)  SD card reader.
