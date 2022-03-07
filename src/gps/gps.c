#include <driver/uart.h>
#include <driver/gpio.h>

// GPS Pins
#define GPS_RXPIN GPIO_NUM_36
#define GPS_TXPIN GPIO_NUM_32  // unused
#define GPS_RTSPIN GPIO_NUM_28  // unused
#define GPS_CTSPIN GPIO_NUM_29  // unused
#define GPS_BAUD 9600

#define GPS_UART_CONTROLLER UART_NUM_1

int init() {
    const uart_port_t uart_num = GPS_UART_CONTROLLER;
    uart_config_t uart_config = {
        .baud_rate = GPS_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 122
    };

    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));

    // Set UART pins(TX: IO4, RX: IO5, RTS: IO18, CTS: IO19)
    ESP_ERROR_CHECK(uart_set_pin(GPS_UART_CONTROLLER, GPS_TXPIN, GPS_RXPIN, GPS_RTSPIN, GPS_CTSPIN));

    // Setup UART buffered IO with event queue
    const int uart_buffer_size = (1024 * 2);

    QueueHandle_t uart_queue;
    // Install UART driver using an event queue here
    ESP_ERROR_CHECK(uart_driver_install(GPS_UART_CONTROLLER, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));

    return 0;
}

uint8_t* getMessage() {
    // Read data from UART
    uint8_t data[128];
    int length = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(GPS_UART_CONTROLLER, (size_t*)&length));
    length = uart_read_bytes(GPS_UART_CONTROLLER, data, length, 100);

    return data;
}