#include "usart.h"



static const struct device *const usartShell = DEVICE_DT_GET(DT_CHOSEN(zephyr_shell_uart));
static const struct device *const usartDevice = DEVICE_DT_GET(DT_ALIAS(deviceusart));

// const k_tid_t usartDeviceThread;
// const k_tid_t usartShellThread;



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


static void cbShell(const struct device *dev, void *user_data)
{
	uint8_t c;
	uint8_t totRX[MSG_SIZE];
	uint8_t temp[] = "this is echo: ";
	static uint8_t rx_buf_pos = 0;
	static char rx_buf[MSG_SIZE];

	if ((!uart_irq_update(usartShell)) || (!uart_irq_rx_ready(usartShell))) {
		return;
	}

	/* read until FIFO empty */
	while (uart_fifo_read(usartShell, &c, 1) == 1) {
		if ((c == '\n' || c == '\r') && rx_buf_pos > 0) {
			/* terminate string */
			rx_buf[rx_buf_pos] = '\0';

			/* if queue is full, message is silently dropped */
			k_msgq_put(&shellMsg, &rx_buf, K_NO_WAIT);

			/* reset the buffer (it was copied to the msgq) */
			rx_buf_pos = 0;
		} else if (rx_buf_pos < (sizeof(rx_buf) - 1)) {
			rx_buf[rx_buf_pos++] = c;
		}
		/* else: characters beyond buffer size are dropped */
	}
	memset(totRX, 0, MSG_SIZE);
	memcpy(totRX, temp, 14);

	memcpy(&totRX[13], rx_buf, MSG_SIZE - 14);
	k_msgq_put(&shellMsg, &rx_buf, K_NO_WAIT);
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

void shellTask()
{
	uint8_t tx_buf[40];
	uint8_t tx_buf2[] = "besme aalah";
	usartInit(usartShell, cbShell);
	while (1)
	// while (k_msgq_get(&shellMsg, &tx_buf, K_FOREVER) == 0)
	{
		int err = uart_tx(usartShell, tx_buf2, sizeof(tx_buf2), SYS_FOREVER_US);
		// int err = uart_tx(usartShell, tx_buf, sizeof(tx_buf), SYS_FOREVER_US);
		if(err)
		{
			return;
		}
		k_msleep(1000);
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
K_THREAD_DEFINE(usartShellThread, USART_SHELL_STACK_SIZE, shellTask, NULL, NULL, NULL, USART_SHELL_PRIORITY, 0, 0);

