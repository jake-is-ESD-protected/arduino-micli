#include "string.h"
#include "cli.h"


TaskHandle_t h_cliTask = NULL;  // Task handle for the CLI task (FreeRTOS)

// init all UART hardware and interrupts on the UART line, start the handling task
void initCLI(uint32_t baudrate){   
    Serial.begin(baudrate);
    Serial.setTimeout(10);
    attachInterrupt(digitalPinToInterrupt(RX_PIN), serialISR, FALLING);
    xTaskCreate(processInput, "CLI event task", 4096, NULL, 1, &h_cliTask);
}

// interrupt service routine for incoming messages
void serialISR(void) {
    detachInterrupt(digitalPinToInterrupt(RX_PIN));
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(h_cliTask, 
                               &xHigherPriorityTaskWoken);
}

// helper function
void printBootMessage(void){
    Serial.println("\n***********\n\rCLI ready.\n\r***********\n\r");
}

// helper function
void printCLIHead(void){
    Serial.print("mini-CLI0.1 $ ");
}

// processing task (waits for input)
void processInput(void* param) {
    char inputString[CLI_BUF_SIZE] = {0};   // buffer for incoming messages
    uint32_t inputIndex = 0;                // current index pointing to a letter in the message
    bool inputComplete = false;             // flag for reaching the end of the message
    printBootMessage();
    printCLIHead();
    while(true){
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);    // wait for notification from interrupt
        vTaskDelay(10 / portTICK_PERIOD_MS);        // wait for all messages to arrive
        while (Serial.available()) {                // read letters
            char c = Serial.read();
            if(c == '\r' || c == '\n'){
                inputString[inputIndex] = '\0';
                inputIndex = 0;
                inputComplete = true;
            } 
            else if(inputIndex < CLI_BUF_SIZE - 1){
                inputString[inputIndex++] = c;
            }
            
        }
        if(inputComplete){
            char* command = strtok(inputString, " ");       // look for arguments seperated by whitespaces
            if (command != NULL) {

                // YOUR STUFF GOES HERE:

                // button command
                if(strcmp(command, "button") == 0){
                    Serial.println("BUTTON service routine");
                    // toggle a GPIO or whatever...
                }
                // switch command
                else if(strcmp(command, "switch") == 0){
                    char* arg = strtok(NULL, " ");
                    if(arg != NULL){
                        // switch on command
                        if(strcmp(arg, "on") == 0){
                           Serial.println("SWITCH ON service routine");
                           // toggle a GPIO or whatever...
                        }
                        // switch off command
                        else if(strcmp(arg, "off") == 0){
                            Serial.println("SWITCH OFF service routine");
                            // toggle a GPIO or whatever...
                        }
                        // warn user about not specifying an argument
                        else{
                            Serial.print("Unknown argument <");
                            Serial.print(arg);
                            Serial.println(">. Use <on> or <off>.");
                        }
                    }
                    // warn user about an unknown argument
                    else{
                        Serial.println("Missing argument. Use <on> or <off>.");
                    }
                }
                // warn user about unknown command
                else{
                    Serial.print("Unknown command <");
                    Serial.print(command);
                    Serial.print(">\r\n");
                }
            }
            // reset flag
            inputComplete = false;
            printCLIHead();
            attachInterrupt(digitalPinToInterrupt(RX_PIN), serialISR, FALLING);
        }
    }
}

