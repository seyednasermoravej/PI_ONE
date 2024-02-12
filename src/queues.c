#include "queues.h"


struct k_msgq shellMsg;
struct k_msgq deviceMsg;
/* queue to store up to 10 messages (aligned to 4-byte boundary) */
K_MSGQ_DEFINE(shellMsg, USART_MSG_LEN, 10, 4);
K_MSGQ_DEFINE(deviceMsg, USART_MSG_LEN, 10, 4);