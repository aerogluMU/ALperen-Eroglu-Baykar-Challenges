import _thread
import time
import RPi.GPIO as GPIO
import json
from smbus import SMBus

def GPIO_Init():
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(4,GPIO.OUT)
    GPIO.setup(17,GPIO.OUT)
    
    GPIO.setup(27,GPIO.IN,pull_up_down=GPIO.PUD_UP)
    GPIO.add_event_detect(27,GPIO.FALLING,callback=BUTTON_CALLBACK,bouncetime=100)

def Read_JSON():
    with open('config.json', 'r') as myfile:
        data=myfile.read()

    config = json.loads(data)
    shutter_period_time = (config['shutter_period_time'])
    return shutter_period_time

def BUTTON_CALLBACK(channel):
    global it_occured
    global ts_buffer
    global count
    ts = time.time()
    ts_buffer[count]=ts
    count+=1
    it_occured = 1
    print("Buton CallBack: ",ts)

def thread_shutter_pulse(shutter_period):
    while(1):
            GPIO.output(4,True)
            GPIO.output(17,True)
            time.sleep(shutter_period)
            GPIO.output(4,False)
            GPIO.output(17,False)
            time.sleep(shutter_period)

def thread_shutter_record(shutter_period):
    global it_occured
    while(1):
        if(it_occured == 1):
            global ts_buffer
            global count
            with open('shutter.csv', 'a', encoding='UTF8') as f:
                for ts in ts_buffer:
                    if ts is not None:
                        f.write(f"{ts}\n")
            ts_buffer=[None] * 100
            count=0
            it_occured=0
        time.sleep(shutter_period)

def thread_i2c_read(shutter_period):
    addr = 0x61
    bus = SMBus(1)
    time.sleep(1)
    while(1):
        data = bus.read_byte_data(addr,3)
        print("I2C: ",data)
        time.sleep(shutter_period)

ts_buffer= [None] * 100
count=0
it_occured = 0

GPIO_Init()
shutter_period_time = Read_JSON()

_thread.start_new_thread(thread_shutter_pulse,(shutter_period_time,))
_thread.start_new_thread(thread_shutter_record,(shutter_period_time,))
_thread.start_new_thread(thread_i2c_read,(shutter_period_time,))