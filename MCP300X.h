#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "esp_log.h"
#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"

//#include "Globals.h"

#define MCP300X_INTERFACE_CONFIG_CLOCK_SPEED_HZ_DEFAULT 1000000
#define MCP300X_INTERFACE_CONFIG_MODE_DEFAULT 0
#define MCP300X_INTERFACE_CONFIG_QUEUE_SIZE_DEFAULT 7

#define MCP300X_INTERFACE_VREF_DEFAULT 3.3

enum MCP300X_VAL
{
	MCP300X_VAL_MIN = 0,
	MCP300X_VAL_MAX = 1023,
};

typedef struct MCP300X
{
	spi_host_device_t host;
	spi_device_interface_config_t config;
	spi_device_handle_t handle;
	double Vref;
} MCP300X_t;

extern MCP300X_t MCP300X;

void MCP300X_DUMMYFUNCTION();

bool MCP300X_readSingleEnded(uint8_t chan, uint16_t *val);
bool MCP300X_readSingleEndedVoltage(uint8_t chan, double *V);

bool MCP300X_deinit();
bool MCP300X_init(spi_host_device_t host, int pinCS, double Vref);