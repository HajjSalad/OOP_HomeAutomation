//*****************************************************************************
//
// main.c - Entry point and main flow of the program
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "control_task.h"
#include "demo.h"
#include "wrapper.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

//*****************************************************************************

// mutex to protect concurrent access of UART from multiple tasks.

SemaphoreHandle_t UARTSemaphore;     // Semaphore for UART synchronization
SemaphoreHandle_t LogSemaphore;      // Semaphore for Log synchronization

//*****************************************************************************

// This hook is called by FreeRTOS when an stack overflow error is detected.

void vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName) {
    while(1) {}
}

//*****************************************************************************

// Configure the UART and its pins.

void ConfigureUART(void) {

    // Enable the GPIO Peripheral used by UART0.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Configure GPIO Pins for UART0
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Use the internal 16MHz oscillator as the UART0 clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    // Initialize the UART0 for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);

}

//*****************************************************************************

// Initialize FreeRTOS and start the initial set of tasks.

int main(void) {

    // Set the clocking to run at 50 MHz from the PLL.
    //
     ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);

    // Initialize the UART and configure it for 115,200, 8-N-1 operation.
    //
    ConfigureUART();

    // Send notification to LogTask to wake up and start logs
    //xTaskNotifyGive(xLogTaskHandle);

    // Initialize the demo
    demo_init();

    // Get a message from the C++ file
    const char* message = demo_get_message();

    // Print the message using UARTprintf
    //UARTprintf("Message from C++: %s\n", message);

    // Print demo introduction.
    //
    //UARTprintf("\nWelcome to Home Automation Demo!\n\n");

    // Create a mutex to guard the UART and Log.
    //
    UARTSemaphore = xSemaphoreCreateMutex();
    LogSemaphore = xSemaphoreCreateMutex();

    // Create the Lighting task.
    //
    if (ControlTaskInit() != 0) {
        while(1) {}
    }

    // Start the scheduler.  This should not return.
    //
    vTaskStartScheduler();

    // In case the scheduler returns for some reason, print an error and loop forever
    //
    while(1) {}

}

//*****************************************************************************
