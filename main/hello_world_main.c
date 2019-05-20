/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/semphr.h"

#define RGB_G 2
#define RGB_R 0
#define RGB_B 4

#define BTN_Y 27
#define BTN_G 13

#define BTN_CLICKED 1

enum TypeClick
{
    Simple = 1,
    Double,
    Long,
    VeryLong
};


static xQueueHandle gpio_evt_queue = NULL;

static gpio_num_t RGBOrders[] = {RGB_G, RGB_R, RGB_B};

static TaskHandle_t rgbHandler = NULL;
static uint32_t speed = 200;

static void IRAM_ATTR   gpio_isr_handler(void* arg) //Recepteur du trigger
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL); //Reception l'event et sera receptionné dans une task
}

static void taskRGB(void *pvParameter) //Tache pour alterné un RGB
{
    xSemaphoreGive(*(SemaphoreHandle_t *)pvParameter); //Mutex pour pouvoir copier la structure de la heap
    while(1) {
        for (int x = 0;x < 3;x++){
            gpio_set_level(RGBOrders[x], 1);
            vTaskDelay(pdMS_TO_TICKS(speed));
            gpio_set_level(RGBOrders[x], 0);            
        }
    }
}

static void startRGB() //Lancer + paramétré une tache RGB pour le vert
{
    BaseType_t xResult;
    SemaphoreHandle_t mutex = NULL;
    ESP_LOGI("main","Creating task for RGB...");
    mutex = xSemaphoreCreateMutex();
    if (!mutex || xSemaphoreTake(mutex, (TickType_t) 0 ) != pdTRUE){
        ESP_LOGE("main", "Impossible to create semaphore mutex");
        ESP_ERROR_CHECK(0);
    }
    
    xResult = xTaskCreate(taskRGB, "RGB Chenillard", 2048, (void *)&mutex, tskIDLE_PRIORITY, &rgbHandler);
    ESP_LOGI("main","Saving the task handler...");
    if (xSemaphoreTake(mutex, (TickType_t)10) != pdTRUE || xResult != pdPASS){ //Permet de faire une 'pause' le temps que qu'on copie les params
        ESP_LOGE("main", "Impossible to start task RGB");
        ESP_ERROR_CHECK(0);
    }
    vSemaphoreDelete(mutex);
}

static void btnClicked(uint32_t io_num, enum TypeClick type)
{
    uint32_t    offset = 0;
    uint32_t    tmp;
    ESP_LOGI("main", "GPIO[%d] click type %d", io_num, type);
    switch (type)
    {
        case Simple:
            offset = 5;
            break;
        case Double:
            tmp = RGBOrders[0];
            RGBOrders[0] = RGBOrders[2];
            RGBOrders[2] = tmp;
            return;
        case Long:
            offset = 10;
            break;
        case VeryLong:
            offset = 20;
            break;
    }
    if (io_num == BTN_Y)
        offset = -offset;
    speed += offset;
    if (speed < 0)
        speed = 0;
    ESP_LOGI("main", "Current speed %d", speed);
}

static void taskBtnHandler(void* arg) //Tache de traitement des trigger
{
    uint32_t    io_num;
    uint32_t    clicked = 0;
    TickType_t last_click = xTaskGetTickCount();
    while(1) {
        uint32_t current;
        if(xQueueReceive(gpio_evt_queue, &io_num, 200 / portTICK_PERIOD_MS)) {
            TickType_t tick = xTaskGetTickCount();
            current = (tick - last_click)  * portTICK_PERIOD_MS;
            uint32_t state = gpio_get_level((gpio_num_t)io_num);
            ESP_LOGI("main", "GPIO[%d] state: %d", io_num, state);
            if (clicked > 0 && clicked != io_num)
                continue;
            if (state != BTN_CLICKED){
                clicked = 0;
                continue;
            }
            if (last_click > 0 && current <= 200){
                ESP_LOGI("main", "GPIO[%d] double click", io_num);
                btnClicked(io_num, Double);
            } else {
                ESP_LOGI("main", "GPIO[%d] simple click", io_num);
                btnClicked(io_num, Simple);
            }
            clicked = io_num;
            last_click = tick;
        }else if (clicked > 0 && (current = (xTaskGetTickCount() - last_click)  * portTICK_PERIOD_MS) >= 1000){
            ESP_LOGI("main", "GPIO[%d] long click", clicked);
            if (current > 2000)
                btnClicked(clicked, VeryLong);
            else
                btnClicked(clicked, Long);
        }
    }
}

void app_main()
{
    BaseType_t xResult;
    TaskHandle_t xHandle = NULL;
	gpio_config_t	gpio_conf;

    ESP_LOGI("main","Starting program...");

	ESP_LOGI("main","GPIO Configuration...");
	gpio_conf.intr_type = GPIO_INTR_ANYEDGE;
	gpio_conf.pin_bit_mask = (1ULL<<RGB_G) | (1ULL<<RGB_R) | (1ULL<<RGB_B);
	gpio_conf.mode = GPIO_MODE_OUTPUT;
    gpio_config(&gpio_conf);

    gpio_conf.intr_type = GPIO_INTR_ANYEDGE;
	gpio_conf.pin_bit_mask = (1ULL<<BTN_Y) | (1ULL<<BTN_G) ;
	gpio_conf.mode = GPIO_MODE_INPUT;
    gpio_conf.pull_up_en = 1;
    gpio_config(&gpio_conf);

	//RGB Tasks
    startRGB();

    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    gpio_install_isr_service(0);
    ESP_LOGI("main","Creating the handler of the green bouton...");
    gpio_isr_handler_add(BTN_G, gpio_isr_handler, (void*) BTN_G);
    ESP_LOGI("main","Creating the handler of the yellow bouton...");
    gpio_isr_handler_add(BTN_Y, gpio_isr_handler, (void*) BTN_Y);

    //Background callback
    ESP_LOGI("main","Creating task for callback if the buttons...");
    xResult = xTaskCreate(taskBtnHandler, "callback btns", 2048, NULL, tskIDLE_PRIORITY, &xHandle);
    ESP_LOGI("main","Saving the task handler...");
    if (xResult != pdPASS){
        ESP_LOGE("main", "Impossible to add the task the queue");
        ESP_ERROR_CHECK(0);
    }


	// //Program end
    // ESP_LOGI("main", "Program in pause...");
    // while(1){ //Permet de mettre le programme en attente et pas redémarrer
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }
}
