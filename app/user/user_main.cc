/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2014/12/1, v1.0 create this file.
*******************************************************************************/
extern "C" {
#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

// #include "udhcp/dhcpd.h"

#include "driver/uart.h"
//#include "driver/gpio.h"
#include "driver/ardio.h"

#include "user_config.h"

#include "monitor.h"
}
#include "cppsupport.h"

#include "driver/vfd.h"


//Gets microseconds from RTC (counter continues during deep sleep)
#define microsRTC() (system_get_rtc_time() * (system_rtc_clock_cali_proc() / 4096.0))

//Gets milliseconds from RTC (counter continues during deep sleep
#define millisRTC() (system_get_rtc_time() * (system_rtc_clock_cali_proc() / 4096.0) / 1000)


void ICACHE_FLASH_ATTR blink2(void *pvParameters) {
    const portTickType xDelay = 250 / portTICK_RATE_MS;
    static uint8_t state = 0;
    
    pinMode(2, OUTPUT);

    for (;;) {
        digitalWrite(2, state);
        state ^=1;
        vTaskDelay(xDelay);
    }
}

#if 0
void taskClient(void *pvParameters)
{
    printf("Hello, welcome to client!\r\n");

    while (1) {
        int recbytes;
        int sin_size;
        int str_len;
        int sta_socket;

        struct sockaddr_in local_ip;
        struct sockaddr_in remote_ip;

        sta_socket = socket(PF_INET, SOCK_STREAM, 0);

        if (-1 == sta_socket) {
            close(sta_socket);
            printf("C > socket fail!\n");
            continue;
        }

        printf("C > socket ok!\n");
        bzero(&remote_ip, sizeof(struct sockaddr_in));
        remote_ip.sin_family = AF_INET;
        remote_ip.sin_addr.s_addr = inet_addr(server_ip);
        remote_ip.sin_port = htons(server_port);

        if (0 != connect(sta_socket, (struct sockaddr *)(&remote_ip), sizeof(struct sockaddr))) {
            close(sta_socket);
            printf("C > connect fail!\n");
            vTaskDelay(4000 / portTICK_RATE_MS);
            continue;
        }

        printf("C > connect ok!\n");
        char *pbuf = (char *)zalloc(1024);
        sprintf(pbuf, "%s\n", "client_send info");

        if (write(sta_socket, pbuf, strlen(pbuf) + 1) < 0) {
            printf("C > send fail\n");
        }

        printf("C > send success\n");
        free(pbuf);

        char *recv_buf = (char *)zalloc(128);
        while ((recbytes = read(sta_socket , recv_buf, 128)) > 0) {
        	recv_buf[recbytes] = 0;
            printf("C > read data success %d!\nC > %s\n", recbytes, recv_buf);
        }
        free(recv_buf);

        if (recbytes <= 0) {
		    close(sta_socket);
            printf("C > read data fail!\n");
        }
    }
}
#endif


VFD vfd;

class ScopeTimer {
    uint32_t start;
public:
    ICACHE_FLASH_ATTR ScopeTimer() : start(system_get_time()) { }
    ICACHE_FLASH_ATTR ~ScopeTimer() {
        printf("%dµS\n", system_get_time() - start);
    }
};

extern "C" int ICACHE_FLASH_ATTR vfdReset(int argc __attribute__((unused)), portCHAR **argv __attribute__((unused))) 
{
    ScopeTimer s;
    printf("VFD init... ");
    vfd.initialize();
    return 0;
}

extern "C" int ICACHE_FLASH_ATTR vfdTest(int argc __attribute__((unused)), portCHAR **argv __attribute__((unused))) 
{
    ScopeTimer s;
    printf("VFD test... ");
    vfd.test();
    return 0;
}

// -----------------------------------------------------------------------------------

static xQueueHandle tcpRxQueue;

extern "C" void ICACHE_FLASH_ATTR GetTcpRxQueue(xQueueHandle *qh) {
  *qh = tcpRxQueue;
}

int client_sock;

void ICACHE_FLASH_ATTR sock_write_handler(char c) 
{
    write(client_sock, (void *)&c, 1);
}

void ICACHE_FLASH_ATTR taskServer(void *pvParameters)
{
    tcpRxQueue = xQueueCreate(128, sizeof(uint8_t));

    while (1) {
        uint8_t i;
        struct sockaddr_in server_addr, client_addr;
        int server_sock; //, client_sock;
        socklen_t sin_size;
        bzero(&server_addr, sizeof(struct sockaddr_in));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(80);
        int recbytes;

        do {
            if (-1 == (server_sock = socket(AF_INET, SOCK_STREAM, 0))) {
                printf("S > socket error\n");
                break;
            }

            printf("S > create socket: %d\n", server_sock);

            if (-1 == bind(server_sock, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr))) {
                printf("S > bind fail\n");
                break;
            }

            printf("S > bind port: %d\n", ntohs(server_addr.sin_port));

            if (-1 == listen(server_sock, 5)) {
                printf("S > listen fail\n");
                break;
            }

            printf("S > listen ok\n");

            sin_size = sizeof(client_addr);

            for (;;) {
                printf("S > wait client\n");

                if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &sin_size)) < 0) {
                    printf("S > accept fail\n");
                    continue;
                }

                printf("S > Client from %s %d\n", inet_ntoa(client_addr.sin_addr), htons(client_addr.sin_port));

                os_install_putc1(sock_write_handler);
                monitorTaskStart();

                    char *recv_buf = (char *)zalloc(128);
                    while ((recbytes = read(client_sock , recv_buf, 128)) > 0) {
                    	recv_buf[recbytes] = 0;
                        //printf("S > read data success %d!\nS > %s\n", recbytes, recv_buf);

                        for (i = 0; i < recbytes; i++) {
                            xQueueSend(tcpRxQueue, (void *)&recv_buf[i], (portTickType)portMAX_DELAY);
                        }
                    }
                    free(recv_buf);

                monitorTaskStop();
                UART_SetPrintPort((UART_Port)0);

                if (recbytes <= 0) {
                    printf("S > read data fail!\n");
                    close(client_sock);
                }
            }
        } while(0);
    }
}


void ICACHE_FLASH_ATTR hexdump(uint8_t *mem, uint32_t len, uint8_t cols) {
    printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (size_t)mem, len, len);
    for(uint32_t i = 0; i < len; i++) {
        if(i % cols == 0) {
            printf("\n[0x%08X] 0x%08X: ", (size_t)mem, i);
        }
        printf("%02X ", *mem);
        mem++;
    }
    printf("\n");
}

extern "C" void ets_update_cpu_frequency(int freqmhz);

extern "C" {
void ICACHE_FLASH_ATTR user_init(void)
{
    cppInitialize();

    REG_SET_BIT(0x3ff00014, BIT(0));
    ets_update_cpu_frequency(160);

    // system debug output
    //void system_set_os_print (uint8 onoff);

//void system_print_meminfo (void)

// uint8 system_get_cpu_freq(void)

/*
If system_get_rtc_time returns 10 (it means 10 RTC cycles), 
and system_rtc_clock_cali_proc returns 5.75 (means 5.75us per RTC cycle), 
then the real time is 10 x 5.75 = 57.5 us.
*/




    uart_init_new();
    //printf("SDK version:%s\n", system_get_sdk_version());

    wifi_set_opmode(STATIONAP_MODE);
    {
        struct station_config *config = (struct station_config *)zalloc(sizeof(struct station_config));
        sprintf((char *)config->ssid, "pit");
        sprintf((char *)config->password, "0000000000000000");

        /* need to sure that you are in station mode first,
         * otherwise it will be failed. */
        wifi_station_set_config(config);
        free(config);
    }


    xTaskCreate(blink2, (const signed char*)"blnk", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
//  xTaskCreate(taskClient, "cli", 256, NULL, 2, NULL);
    xTaskCreate(taskServer, (const signed char*)"srv", 256, NULL, 2, NULL);

//    vfd.initialize();
//    vfd.test();
}

} // extern "C"

