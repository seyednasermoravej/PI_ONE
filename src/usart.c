#include "../inc/usart.h"


#ifdef MICRO
static const struct device *const usartDevice = DEVICE_DT_GET(DT_ALIAS(deviceusart));



static int usartInit(const struct device *const usart, void * cb)
{
    if (!device_is_ready(usart)) {
		printk("UART device not found!");
		return 0;
	}

	int ret = uart_irq_callback_user_data_set(usart, cb, NULL);
	uart_irq_rx_enable(usart);
	return ret;
}



static void cbDevice(const struct device *dev, void *user_data)
{
	uint8_t c;
	static uint8_t rx_buf_pos = 0;
	static char rx_buf[MSG_SIZE];
	if ((!uart_irq_update(usartDevice)) || (!uart_irq_rx_ready(usartDevice))) {
		return;
	}

	/* read until FIFO empty */
	while (uart_fifo_read(usartDevice, &c, 1) == 1) {
		if ((c == '\n' || c == '\r') && rx_buf_pos > 0) {
			/* terminate string */
			rx_buf[rx_buf_pos] = '\0';

			/* if queue is full, message is silently dropped */
			k_msgq_put(&deviceMsg, &rx_buf, K_NO_WAIT);

			/* reset the buffer (it was copied to the msgq) */
			rx_buf_pos = 0;
		} else if (rx_buf_pos < (sizeof(rx_buf) - 1)) {
			rx_buf[rx_buf_pos++] = c;
		}
		/* else: characters beyond buffer size are dropped */
	}

}


void deviceTask()
{
	uint8_t tx_buf[40];
	usartInit(usartDevice, cbDevice);
	while (k_msgq_get(&deviceMsg, &tx_buf, K_FOREVER) == 0)
	{
		int err = uart_tx(usartDevice, tx_buf, sizeof(tx_buf), SYS_FOREVER_US);
		if(err)
		{
			return;
		}
		k_msleep(1000);

	}
}


K_THREAD_DEFINE(usartDeviceThread, USART_DEVICE_STACK_SIZE, deviceTask, NULL, NULL, NULL, USART_DEVICE_PRIORITY, 0, 0);

#endif