#include "string.h"
#include "cli.h"

char inputString[CLI_BUF_SIZE] = {0};   // buffer for incoming messages
uint32_t inputIndex = 0;                // current index pointing to a letter in the message
bool inputComplete = false;             // flag for reaching the end of the message
TaskHandle_t h_cliTask = NULL;          // Task handle for the CLI task (FreeRTOS)

// init all UART hardware and interrupts on the UART line, start the handling task
void initCLI(uint32_t baudrate){   
    Serial.begin(baudrate);
    Serial.setTimeout(10);
    delay(10);
    attachInterrupt(digitalPinToInterrupt(RX_PIN), serialISR, FALLING);
    xTaskCreate(processInput, "CLI event task", 4096, NULL, 1, &h_cliTask);
}

// interrupt service routine for incoming messages
void serialISR(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(h_cliTask, 
                               &xHigherPriorityTaskWoken);
}

// processing task (waits for input)
void processInput(void* param) {
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

                if(strcmp(command, "button") == 0){
                    Serial.println("BUTTON service routine");
                }
                else if(strcmp(command, "switch") == 0){
                    char* arg = strtok(NULL, " ");
                    if(arg != NULL){
                        if(strcmp(arg, "on") == 0){
                           Serial.println("SWITCH ON service routine"); 
                        }
                        else if(strcmp(arg, "off") == 0){
                            Serial.println("SWITCH ON service routine");
                        }
                        else{
                            Serial.print("Unknown argument <");
                            Serial.print(arg);
                            Serial.println(">. Use <on> or <off>.");
                        }
                    }
                    else{
                        Serial.println("Missing argument. Use <on> or <off>.");
                    }
                }
                else{
                    Serial.print("Unknown command <");
                    Serial.print(command);
                    Serial.print(">\r\n");
                }
            }
            inputComplete = false;
            printCLIHead();
        }
    }
}

void printBootMessage(void){
    Serial.println("\n***********\n\rCLI ready.\n\r***********\n\r");
}

void printCLIHead(void){
    Serial.print("mini-CLI0.1 $ ");
}