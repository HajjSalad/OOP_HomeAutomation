//*****************************************************************************

// control_task.c - Control all devices and sensors based on input.

//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "drivers/rgb.h"
#include "utils/uartstdio.h"
#include "control_task.h"
#include "wrapper.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

//*****************************************************************************

// The stack size for the Control task.

#define     CONTROLTASKSTACKSIZE        128            // Stack size in words

TaskHandle_t xControlTaskHandle = NULL;                // ControlTask Notification

extern SemaphoreHandle_t UARTSemaphore;
extern SemaphoreHandle_t LogSemaphore;

// [G, R, B] range is 0 to 0xFFFF per color.
static uint32_t g_pui32Colors[3] = { 0x0000, 0x0000, 0x0000 };

//*****************************************************************************

// Check Heap
void checkHeapUsage() {
    UARTprintf("Free Heap Size: %u bytes\n", xPortGetFreeHeapSize());
    UARTprintf("Minimum Ever Free Heap Size: %u\n", xPortGetMinimumEverFreeHeapSize());
}

// Print Header
void printHeader(uint8_t myRoom1Flag, uint8_t myRoom2Flag, uint8_t myRoom3Flag,
                 uint8_t myRoom4Flag) {
    UARTprintf("\nWelcome to Home Automation Demo!\n\n");
    UARTprintf("LivingRoom-1 created - More description later.\n");
    UARTprintf("BedRoom-1 created - More description later.\n");
    UARTprintf("LivingRoom-2 created - More description later.\n");
    UARTprintf("BedRoom-2 created - More description later.\n");
}

// The Control Task

static void ControlTask(void *pvParameters) {
    // Predefined motion data
    uint8_t LRmotion1[] = {0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1,
                        0, 0, 0, 1, 1, 1, 0, 0, 0};             // LivingRoom-1
    uint8_t BRmotion1[] = {1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0,
                        1, 1, 1, 0, 0, 0, 1, 1, 1};             // BedRoom-1
    uint8_t LRmotion2[] = {1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0,
                        0, 0, 0, 1, 1, 1, 1, 1, 1};             // LivingRoom-2
    uint8_t BRmotion2[] = {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0,
                        1, 1, 1, 0, 0, 0, 1, 1, 1};             // BedRoom-2
   // Predefined temp data
   uint8_t LRtemp1[] = {22, 22, 23, 27, 29, 26, 23, 22, 20,
                       19, 17, 16, 21, 24, 22, 26, 27, 28};     // LivingRoom-1
   uint8_t BRtemp1[] = {19, 17, 16, 22, 20, 23, 26, 27, 29,
                       22, 24, 21, 27, 28, 26, 23, 22, 20};     // BedRoom-1
   uint8_t LRtemp2[] = {19, 16, 17, 22, 23, 20, 27, 26, 29,
                       22, 21, 24, 27, 28, 26, 22, 23, 20};     // LivingRoom-2
   uint8_t BRtemp2[] = {27, 29, 26, 24, 23, 22, 18, 16, 15,
                       22, 23, 21, 19, 17, 16, 21, 23, 22};     // BedRoom-2

    uint8_t myRoom1Flag, myRoom2Flag, myRoom3Flag, myRoom4Flag = 0;
    // Create a LivingRoom-1 with RoomNum = 101
    void* myRoom1 = createLivingRoom(101);
    if(!myRoom1) {
        UARTprintf("LivingRoom creation failed.\n");
        return;
    } else {
        myRoom1Flag = 1;
    }
    // Start with all devices off
    turnOffLight(myRoom1);
    turnOffAC(myRoom1);
    turnOffHeater(myRoom1);

    // Create a BedRoom-1 with RoomNum = 102
    void* myRoom2 = createBedRoom(102);
    if(!myRoom2) {
        UARTprintf("BedRoom creation failed.\n");
        return;
    } else {
        myRoom2Flag = 1;
    }
    // Start with all devices off
    turnOffLight(myRoom2);
    turnOffAC(myRoom2);
    turnOffHeater(myRoom2);

    // Create LivingRoom-2 with RoomNum = 201
    void* myRoom3 = createLivingRoom(201);
    if(!myRoom3) {
        UARTprintf("LivingRoom-2 creation failed.\n");
        return;
    } else {
        myRoom3Flag = 1;
    }
    // Start with all devices off
    turnOffLight(myRoom3);
    turnOffAC(myRoom3);
    turnOffHeater(myRoom3);

    // Create BedRoom-2 with RoomNum = 202
    void* myRoom4 = createBedRoom(202);
    if(!myRoom4) {
        UARTprintf("BedRoom-2 creation failed.\n");
        return;
    } else {
        myRoom4Flag = 1;
    }
    // Start with all devices off
    turnOffLight(myRoom4);
    turnOffAC(myRoom4);
    turnOffHeater(myRoom4);
    printHeader(myRoom1Flag, myRoom2Flag, myRoom3Flag, myRoom4Flag);

    size_t i = 0;
    while (1) {
        UARTprintf("\033[2J\033[H");
        // LivingRoom-1
        setMotionDetectorValue(myRoom1, LRmotion1[i]);       // Pass the sensor value
        if (getMotionDetectorValue(myRoom1)) {
            if (!isLightOn(myRoom1)) {
                turnOnLight(myRoom1);
            }
        } else {
            if (isLightOn(myRoom1)) {
                turnOffLight(myRoom1);
            }
        }
        setTempSensorValue(myRoom1, LRtemp1[i]);            // Pass the sensor value
        if (getTempSensorValue(myRoom1) >= 25) {
            turnOnAC(myRoom1);
            turnOffHeater(myRoom1);
        } else if (getTempSensorValue(myRoom1) < 20) {
            turnOnHeater(myRoom1);
            turnOffAC(myRoom1);
        } else {
            turnOffAC(myRoom1);
            turnOffHeater(myRoom1);
        }

        // BedRoom-1
        setMotionDetectorValue(myRoom2, BRmotion1[i]);   // Pass the sensor value
        if (getMotionDetectorValue(myRoom2)) {
            if (!isLightOn(myRoom2)) {
                turnOnLight(myRoom2);
            }
        } else {
            if (isLightOn(myRoom2)) {
                turnOffLight(myRoom2);
            }
        }
        setTempSensorValue(myRoom2, BRtemp1[i]);            // Pass the sensor value
        if (getTempSensorValue(myRoom2) >= 25) {
            turnOnAC(myRoom2);
            turnOffHeater(myRoom2);
        } else if (getTempSensorValue(myRoom2) < 20) {
            turnOnHeater(myRoom2);
            turnOffAC(myRoom2);
        } else {
            turnOffAC(myRoom2);
            turnOffHeater(myRoom2);
        }
        // LivingRoom-2
        setMotionDetectorValue(myRoom3, LRmotion2[i]);       // Pass the sensor value
        if (getMotionDetectorValue(myRoom3)) {
            if (!isLightOn(myRoom3)) {
                turnOnLight(myRoom3);
            }
        } else {
            if (isLightOn(myRoom3)) {
                turnOffLight(myRoom3);
            }
        }
        setTempSensorValue(myRoom3, LRtemp2[i]);            // Pass the sensor value
        if (getTempSensorValue(myRoom3) >= 25) {
            turnOnAC(myRoom3);
            turnOffHeater(myRoom3);
        } else if (getTempSensorValue(myRoom3) < 20) {
            turnOnHeater(myRoom3);
            turnOffAC(myRoom3);
        } else {
            turnOffAC(myRoom3);
            turnOffHeater(myRoom3);
        }
        // BedRoom-2
        setMotionDetectorValue(myRoom4, BRmotion2[i]);   // Pass the sensor value
        if (getMotionDetectorValue(myRoom4)) {
            if (!isLightOn(myRoom4)) {
                turnOnLight(myRoom4);
            }
        } else {
            if (isLightOn(myRoom4)) {
                turnOffLight(myRoom4);
            }
        }
        setTempSensorValue(myRoom4, BRtemp2[i]);            // Pass the sensor value
        if (getTempSensorValue(myRoom4) >= 25) {
            turnOnAC(myRoom4);
            turnOffHeater(myRoom4);
        } else if (getTempSensorValue(myRoom4) < 20) {
            turnOnHeater(myRoom4);
            turnOffAC(myRoom4);
        } else {
            turnOffAC(myRoom4);
            turnOffHeater(myRoom4);
        }

        UARTprintf("\n\tWelcome to Home Automation Demo!\n\n");
        // Print Display Layout
        if (myRoom1Flag || myRoom3Flag) {
            UARTprintf(" Living Rooms: ");
            if (myRoom1Flag) UARTprintf("LivingRoom-1 created successfully.\n");
            if (myRoom3Flag) UARTprintf("\t       LivingRoom-2 created successfully.\n");
            describeRoom(myRoom1);
        }

        if (myRoom2Flag || myRoom4Flag) {
            UARTprintf("\n Bedrooms: ");
            if (myRoom2Flag) UARTprintf("BedRoom-1 created successfully.\n");
            if (myRoom4Flag) UARTprintf("\t   BedRoom-2 created successfully.\n");
            describeRoom(myRoom2);
        }

        UARTprintf("\n Legend: Light turn on if motion detected\n");
        UARTprintf("         Heater turn on if temp < 20\n");
        UARTprintf("         AC turns on if temp > 25\n");
        UARTprintf("         If temp 20-25, AC and Heater off\n");

        UARTprintf(" ---------------------------------------------------------\n");
        UARTprintf(" |                                                       |\n");
        UARTprintf(" |     LivingRoom-1           |      Bedroom-1           |\n");
        UARTprintf(" |                            |                          |\n");
        UARTprintf("     Temp sensor: %uC\t\t Temp sensor: %uC\n",
                   getTempSensorValue(myRoom1), getTempSensorValue(myRoom2));
        UARTprintf("   Motion sensor: %s\t Motion sensor: %s\n ",
                   LRmotion1[i] ? "Detected" : "No motion", BRmotion1[i] ? "Detected" : "No motion");
        UARTprintf("|                                                       |\n");
        UARTprintf("         %s\t\t     %s    \n", isLightOn(myRoom1)?"Light On":"Light Off",
                isLightOn(myRoom2)?"Light On":"Light Off");
        UARTprintf("          %s                       %s      \n", isACOn(myRoom1)?"AC On":"AC Off",
                isACOn(myRoom2)?"AC On":"AC Off");
        UARTprintf("         %s\t             %s   \n", isHeaterOn(myRoom1)?"Heater On":"Heater Off",
                isHeaterOn(myRoom2)?"Heater On":"Heater Off");
        UARTprintf(" |                                                       |\n");
        UARTprintf(" ---------------------------------------------------------\n");
        UARTprintf(" |                                                       |\n");
        UARTprintf(" |     LivingRoom-2           |      Bedroom-2           |\n");
        UARTprintf(" |                            |                          |\n");
        UARTprintf("     Temp sensor: %uC\t\t Temp sensor: %uC\n",
                   getTempSensorValue(myRoom3), getTempSensorValue(myRoom4));
        UARTprintf("   Motion sensor: %s\t Motion sensor: %s\n ",
                   LRmotion2[i] ? "Detected" : "No motion", BRmotion2[i] ? "Detected" : "No motion");
        UARTprintf("|                                                       |\n");
        UARTprintf("         %s\t\t     %s    \n", isLightOn(myRoom3)?"Light On":"Light Off",
                isLightOn(myRoom4)?"Light On":"Light Off");
        UARTprintf("          %s                       %s      \n", isACOn(myRoom3)?"AC On":"AC Off",
                isACOn(myRoom4)?"AC On":"AC Off");
        UARTprintf("         %s\t             %s   \n", isHeaterOn(myRoom3)?"Heater On":"Heater Off",
                isHeaterOn(myRoom4)?"Heater On":"Heater Off");
        UARTprintf(" |                                                       |\n");
        UARTprintf(" ---------------------------------------------------------\n");

        // Delay between iterations
        vTaskDelay(pdMS_TO_TICKS(3000));
        i++;
    }

    // cleanup
    destroyRoom(myRoom1);
    destroyRoom(myRoom2);
    destroyRoom(myRoom3);
    destroyRoom(myRoom4);
}


//*****************************************************************************

// Initialize the Control task.

uint32_t ControlTaskInit(void) {

    static uint8_t ui8ColorsIndx;

    // Initialize the GPIOs and Timers that drive the LEDs.
    //
    RGBInit(1);
    RGBIntensitySet(0.3f);

    // Turn on Green LED
    // Indicates beginning of the program - Control task highest priority
    //
    ui8ColorsIndx = 1;
    g_pui32Colors[ui8ColorsIndx] = 0x8000;
    RGBColorSet(g_pui32Colors);


    // Unlock the GPIO LOCK register for Right button to work.
    //
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0xFF;

    // Initialize the buttons
    //
    //ButtonsInit();

    // Create the Control task.
    //
    if(xTaskCreate(ControlTask, (const portCHAR *)"Control",
                   CONTROLTASKSTACKSIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_CONTROL_TASK, &xControlTaskHandle) != pdTRUE) {
        return(1);
    }

    return(0);
}
