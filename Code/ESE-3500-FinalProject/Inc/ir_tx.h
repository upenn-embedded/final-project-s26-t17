#ifndef IR_TX_H_
#define IR_TX_H_

#include <stdint.h>

void ir_tx_init(void);
void ir_tx_start_carrier(void);
void ir_tx_stop_carrier(void);
void ir_tx_send_burst_us(uint32_t burst_us);

#endif /* IR_TX_H_ */
