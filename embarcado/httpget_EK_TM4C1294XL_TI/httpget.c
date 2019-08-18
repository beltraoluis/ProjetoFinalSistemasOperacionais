/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== httpget.c ========
 *  HTTP Client GET example application
 */
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

/* XDCtools Header files */
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>

/* TI-RTOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/GPIO.h>
#include <ti/net/http/httpcli.h>

/* Example/Board Header file */
#include "Board.h"

#include <sys/socket.h>

#define HOSTNAME          "10.3.20.167:5000"
#define REQUEST_URI       "/commands"
#define USER_AGENT        "HTTPCli (ARM; TI-RTOS)"
#define HTTPTASKSTACKSIZE 4096

bool IS_CONNECTED = FALSE;
bool INIT = TRUE;
char vector[60];
UInt32 sleepTickCount;

void startTaskl(int priority, void *task(UArg,UArg));

/*
 *  ======== printError ========
 */
void printError(char *errString, int code)
{
    System_printf("Error! code = %d, desc = %s\n", code, errString);
    BIOS_exit(code);
}

/*
 *  ======== httpTask ========
 *  Makes a HTTP GET request
 */
Void httpTask(UArg arg0, UArg arg1)
{
    bool moreFlag = false;
    char data[128];
    int ret;
    int len;
    struct sockaddr_in addr;

    HTTPCli_Struct cli;
    HTTPCli_Field fields[3] = {
        { HTTPStd_FIELD_NAME_HOST, HOSTNAME },
        { HTTPStd_FIELD_NAME_USER_AGENT, USER_AGENT },
        { NULL, NULL }
    };

    System_printf("Sending a HTTP GET request to '%s'\n", HOSTNAME);

    HTTPCli_construct(&cli);

    HTTPCli_setRequestFields(&cli, fields);

    ret = HTTPCli_initSockAddr((struct sockaddr *)&addr, HOSTNAME, 0);
    if (ret < 0) {
        printError("httpTask: address resolution failed", ret);
    }

    ret = HTTPCli_connect(&cli, (struct sockaddr *)&addr, 0, NULL);
    if (ret < 0) {
        printError("httpTask: connect failed", ret);
    }

    ret = HTTPCli_sendRequest(&cli, HTTPStd_GET, REQUEST_URI, false);
    if (ret < 0) {
        printError("httpTask: send failed", ret);
    }

    ret = HTTPCli_getResponseStatus(&cli);
    if (ret != HTTPStd_OK) {
        printError("httpTask: cannot get status", ret);
    }

    System_printf("HTTP Response Status Code: %d\n", ret);

    ret = HTTPCli_getResponseField(&cli, data, sizeof(data), &moreFlag);
    if (ret != HTTPCli_FIELD_ID_END) {
        printError("httpTask: response field processing failed", ret);
    }

    len = 0;
    do {
        ret = HTTPCli_readResponseBody(&cli, data, sizeof(data), &moreFlag);
        if (ret < 0) {
            printError("httpTask: response body processing failed", ret);
        }

        len += ret;
    } while (moreFlag);
    int j =0;
    int k = 0;
    while(j<len){
        if(data[j] == '1'){
            vector[k] = '1';
            k++;
        }
        if(data[j] == '2'){
            vector[k] = '2';
            k++;
        }
        if(data[j] == '3'){
            vector[k] = '3';
            k++;
        }
        if(data[j] == ';'){
            vector[k] = '\0';
            break;
        }
        j++;
    }

    System_printf("body: %s\n", data);
    System_printf("body: %s\n", vector);
    System_printf("Recieved %d bytes of payload\n", len);
    System_flush();

    HTTPCli_disconnect(&cli);
    HTTPCli_destruct(&cli);
}

Void sensorTask(UArg arg0, UArg arg1){
    uint32_t i = 0;
    uint32_t j = 0;
    System_printf("start sensor\n");
    System_flush();
    GPIO_write(Board_L0, 1);
    for(i=0;i<121;i++){}
    GPIO_write(Board_L0, 0);
    i = 0;
    while(GPIO_read(Board_L1) == 0 && j < 69021){
        j++;
        if(j == 69021) i = j;
    }
    while(i < 69021){
        if(GPIO_read(Board_L1) == 0 && i < 3451) {
            System_printf("exit system\n");
            System_flush();
            BIOS_exit(0);
        }
        i++;
    }
    System_printf("exit sensor\n");
    System_flush();
}

Void executorTask(UArg arg0, UArg arg1){
    int len = strlen(vector);
    uint32_t i = 0;
    uint32_t time = 2976191;
    int j = 0;
    char value = '.';
    for(j = 0; j < len; j++){
        startTaskl(1, &sensorTask);
        value = vector[j];
        System_printf("Running %c\n", value);
        System_flush();
        //frente
        if(value == '1'){
            GPIO_write(Board_F2, 1);
            GPIO_write(Board_E0, 1);
            GPIO_write(Board_E1, 0);
            GPIO_write(Board_F3, 1);
            GPIO_write(Board_E2, 1);
            GPIO_write(Board_E3, 0);
            for(i = 0; i < time; i++){}
        }
        // esquerda
        if(value == '2'){
            GPIO_write(Board_F2, 0);
            GPIO_write(Board_E0, 0);
            GPIO_write(Board_E1, 0);
            GPIO_write(Board_F3, 1);
            GPIO_write(Board_E2, 1);
            GPIO_write(Board_E3, 0);
            for(i = 0; i < time; i++){}
        }
        // direita
        if(value == '3'){
            GPIO_write(Board_F2, 1);
            GPIO_write(Board_E0, 1);
            GPIO_write(Board_E1, 0);
            GPIO_write(Board_F3, 0);
            GPIO_write(Board_E2, 0);
            GPIO_write(Board_E3, 0);
            for(i = 0; i < time; i++){}
        }
        GPIO_write(Board_F2, 0);
        GPIO_write(Board_E0, 0);
        GPIO_write(Board_E1, 0);
        GPIO_write(Board_F3, 0);
        GPIO_write(Board_E2, 0);
        GPIO_write(Board_E3, 0);
        for(i = 0; i < time; i++){}
    }
    System_printf("Done running\n", value);
    System_flush();
}

Void mainTask(UArg arg0, UArg arg1){
    uint32_t i = 0;
    while(true){
       if(GPIO_read(Board_BUTTON0) == 0){
           while(GPIO_read(Board_BUTTON0) == 0){}
           startTaskl(1, &httpTask);
       }
       if(GPIO_read(Board_BUTTON1) == 0){
                  while(GPIO_read(Board_BUTTON1) == 0){}
                  startTaskl(1, &executorTask);
              }
       Task_sleep(sleepTickCount);
    }
}

void makeRequest(){
    Task_Struct httpStruct;
    Char httpStack[256];
    Task_Params taskParams;
    Task_Params_init(&taskParams);
    taskParams.stackSize = 256;
    taskParams.stack = &httpStack;
    taskParams.priority = 1;
    Task_construct(&httpStruct, (Task_FuncPtr)httpTask, &taskParams, NULL);
}

void startTaskl(int priority, void *task(UArg,UArg)){
    task(1,1);
}

/*
 *  ======== netIPAddrHook ========
 *  This function is called when IP Addr is added/deleted
 */
void netIPAddrHook(unsigned int IPAddr, unsigned int IfIdx, unsigned int fAdd)
{
    static Task_Handle taskHandle;
    Task_Params taskParams;
    Error_Block eb;

    /* Create a HTTP task when the IP address is added */
    if (fAdd && !taskHandle) {
        Error_init(&eb);

        Task_Params_init(&taskParams);
        taskParams.stackSize = HTTPTASKSTACKSIZE;
        taskParams.priority = 1;
        taskHandle = Task_create((Task_FuncPtr)mainTask, &taskParams, &eb);
        if (taskHandle == NULL) {
            printError("netIPAddrHook: Failed to create HTTP Task\n", -1);
        }
    }
    IS_CONNECTED = TRUE;
}

void startTask1(int priority, Task_FuncPtr task ){
    Error_Block eb;
    Error_init(&eb);
    Char taskStack[256];
    Task_Params taskParams;
    Task_Params_init(&taskParams);
    taskParams.stackSize = 256;
    taskParams.stack = &taskStack;
    taskParams.priority = priority;
    Task_create(task, &taskParams, &eb);
}

/*
 *  ======== main ========
 */

#define TASKSTACKSIZE   512
Task_Struct connectionStruct, executorStruct;
Char connectionStack[TASKSTACKSIZE];
Char executorStack[TASKSTACKSIZE];

int main(void)
{

    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    Board_initEMAC();

    /* Turn on user LED */
    GPIO_write(Board_LED0, Board_LED_ON);

    System_printf("Starting the HTTP GET example\nSystem provider is set to "
            "SysMin. Halt the target to view any SysMin contents in ROV.\n");
    /* SysMin will only print to the console when you call flush or exit */
    System_flush();
    sleepTickCount = 10000 / Clock_tickPeriod;
    /* Start BIOS */
    BIOS_start();
    return (0);
}
