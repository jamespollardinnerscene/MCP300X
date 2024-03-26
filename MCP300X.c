#include "MCP300X.h"

static char const *TAG = "MCP300X";

MCP300X_t MCP300X =
{
	.config =
	{
		.clock_speed_hz = MCP300X_INTERFACE_CONFIG_CLOCK_SPEED_HZ_DEFAULT,
		.mode = MCP300X_INTERFACE_CONFIG_MODE_DEFAULT,
		.queue_size = MCP300X_INTERFACE_CONFIG_QUEUE_SIZE_DEFAULT,
	},
	.Vref = MCP300X_INTERFACE_VREF_DEFAULT,
};


bool MCP300X_readSingleEnded(uint8_t chan, uint16_t *val)
{
	bool success = true;

    esp_err_t ret = ESP_OK;

	// NOTE: future code should take model type (2-ch / 4-ch / 8-ch) and store in struct then check when calling this function
    if (chan >= 4)
	{
		ESP_LOGE(TAG, "Invalid channel");

        return false;
	}

	uint8_t data[3] = { 0, 0, 0 };

	data[0] = 0x01;                     // Start bit
    data[1] = 0x80 | (chan << 4);       // Single-Ended = 0x80 / Differential = 0x00
    data[2] = 0;

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));

    t.length = 3 * 8; // Transaction length is in bits.
    t.tx_buffer = data;
    t.rx_buffer = data;

	// Perform the SPI transaction
	ret = spi_device_transmit(MCP300X.handle, &t);
	if (ret != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to transmit to MCP300X: %s", esp_err_to_name(ret));

        success = false;
    }
    else
    {
        uint32_t result = ((data[1] & 0x03) << 8) | data[2];

        *val = (uint16_t)result;
    }

	return success;
}
bool MCP300X_readSingleEndedVoltage(uint8_t chan, double *V)
{
	bool success = true;

	uint16_t val = 0;

	if (!MCP300X_readSingleEnded(chan, &val))
	{
		success = false;
	}
	else
	{
		*V = MCP300X.Vref * ((double)val / (double)MCP300X_VAL_MAX);
	}

	return success;
}

bool MCP300X_deinit()
{
	bool success = true;

    esp_err_t ret = ESP_OK;

	ret = gpio_reset_pin(MCP300X.config.spics_io_num);
	if (ret != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to reset MCP300X Load pin: %s", esp_err_to_name(ret));

		success = false;
	}

	ret = spi_bus_remove_device(MCP300X.handle);
	if (ret != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to remove MCP300X from SPI bus: %s", esp_err_to_name(ret));

		success = false;
	}

	return success;
}
bool MCP300X_init(spi_host_device_t host, int pinCS, double Vref)
{
	bool success = true;

	esp_err_t ret = ESP_OK;

	MCP300X.host = host;

	MCP300X.config.spics_io_num = pinCS;

	MCP300X.Vref = Vref;

	ret = spi_bus_add_device(MCP300X.host, &MCP300X.config, &MCP300X.handle);
	if (ret != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to add MCP300X to SPI bus: %s", esp_err_to_name(ret));

		success = false;
	}

	return success;
}