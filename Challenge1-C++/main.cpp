#include <iostream>
#include <cstdlib>
#include <thread>
#include <wiringPi.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <fstream>
#include <json-c/json.h>
#include <bcm2835.h>
 
using namespace std;

/* GLobal Variables Begin */

int shutter_period_time = 0;
unsigned int millisecond = 1000;
long int *ts_buffer;
int count = 0;
uint8_t it_occured = 0;
uint8_t i2c_flag = 0;
char dataTx[6] = "hello";

int LED1_Pin = 7;
int LED2_Pin = 0;
int BUTTON_Pin = 2;

struct timeval tv;

/* GLobal Variables End */

/*********************************************************************
 * @fn      		  - GPIO_Init
 *
 * @brief             - This function initialize Led1 and led2.
 *
 * @param[in]         - none
 *
 * @return            - none
 *
 * @Note              - none
 */

void GPIO_Init(){
   wiringPiSetup();
   pinMode(LED1_Pin, OUTPUT);
   pinMode(LED2_Pin, OUTPUT);
   pinMode(BUTTON_Pin, INPUT);
   pullUpDnControl(BUTTON_Pin,PUD_UP);
}

/*********************************************************************
 * @fn      		  - Read_JSON
 *
 * @brief             - This function read shutter_period_time value
 *                      which in config.json file.
 *
 * @param[in]         - none
 *
 * @return            - json_object_get_int(stp)
 *
 * @Note              - none
 */

int Read_JSON(){
   FILE *fp;
	char buffer[50];
   struct json_object *parsed_json;
	struct json_object *stp;
   
   fp = fopen("config.json","r");
	fread(buffer, 50, 1, fp);
	fclose(fp);

	parsed_json = json_tokener_parse(buffer);
	json_object_object_get_ex(parsed_json, "shutter_period_time", &stp);

   return json_object_get_int(stp);
}

/*********************************************************************
 * @fn      		  - thread_shutter_pulse
 *
 * @brief             - This function is called when thread t1 is
 *                      activated.
 *
 * @param[in]         - uint8_t shutter_period
 *
 * @return            - none
 *
 * @Note              - none
 */

void thread_shutter_pulse(uint8_t shutter_period) {
   while(1){
      digitalWrite(LED1_Pin,HIGH);
      digitalWrite(LED2_Pin,HIGH);
      delay(shutter_period * millisecond);
      digitalWrite(LED1_Pin,LOW);
      digitalWrite(LED2_Pin,LOW);
      delay(shutter_period * millisecond);
   }
   
}

/*********************************************************************
 * @fn      		  - thread_shutter_record
 *
 * @brief             - This function saves the values in the ts_buffer
 *                      array to the shutter.csv file every 1 second 
 *                      if a value is entered in ts_buffer.
 *
 * @param[in]         - uint8_t shutter_period
 *
 * @return            - none
 *
 * @Note              - none
 */

void thread_shutter_record(uint8_t shutter_period) {
   while(1){
      if(it_occured == 1){
         ofstream shutter_File("shutter.csv",ios_base::app);
         if (shutter_File.is_open()){
            for(int i = 0;i<count;i++){
               shutter_File << ts_buffer[i] << endl;
            }
            shutter_File.close();
            count=0;
            it_occured=0;
            i2c_flag = 1;
            memset(ts_buffer,'\0',30);
         }
      }
      delay(shutter_period * millisecond);
   }
}

/*********************************************************************
 * @fn      		  - thread_i2c
 *
 * @brief             - This function sends "hello" message from 
 *                      i2c line to STM32F407VG card if i2c_flag is set
 *                      and wakes Receive interrupt on STM32F407VG 
 *                      card. Then it takes 4 bytes of data from 
 *                      the STM32F407VG card and collects this data and 
 *                      prints the counter number.
 *
 * @param[in]         - none
 *
 * @return            - none
 *
 * @Note              - none
 */

void thread_i2c(void) {
   if (!bcm2835_init())
   {
      printf("bcm2835_init failed.\n");
   }

   if (!bcm2835_i2c_begin())
   {
      printf("bcm2835_i2c_begin failed.\n");
   }
   bcm2835_i2c_setSlaveAddress(0x00);
   bcm2835_i2c_setClockDivider(BCM2835_I2C_CLOCK_DIVIDER_150);
   bcm2835_i2c_set_baudrate(400000);
   char testrx[4];
   while(1){
      if(i2c_flag == 1){
         bcm2835_i2c_write(dataTx,6);
         bcm2835_i2c_read((char*)testrx,4);
         uint32_t sum = testrx[0]+testrx[1]+testrx[2]+testrx[3];
         printf("Rx Data: %d\n",sum);
         memset(&testrx,'\0',4);
         i2c_flag = 0;
      }

   }
}

/*********************************************************************
 * @fn      		  - button_callback
 *
 * @brief             - This function is called every time a button
 *                      is pressed.
 *
 * @param[in]         - none
 *
 * @return            - none
 *
 * @Note              - This interrupt button works when it is in
 *                      the rising edge state.
 */

void button_callback(void){
   gettimeofday(&tv,NULL);
   *(ts_buffer+count) = (tv.tv_sec);
   cout << "Buton CallBack "<< count << ":" << *(ts_buffer+count) << endl;
   count++;
   it_occured=1;
}

/* Main Function */

int main () {
   GPIO_Init();
   ts_buffer = (long int*)malloc(30);
   shutter_period_time = Read_JSON();
   if(wiringPiISR(BUTTON_Pin, INT_EDGE_RISING, button_callback)<0){
      cout << "error" << endl;
   }
   thread t1(thread_shutter_pulse,shutter_period_time);
   thread t2(thread_shutter_record,shutter_period_time);
   thread t3(thread_i2c);
   t1.join();
   t2.join();
   t3.join();
}
