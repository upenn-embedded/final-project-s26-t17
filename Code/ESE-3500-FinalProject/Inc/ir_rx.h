#ifndef IR_RX_H_
#define IR_RX_H_

#include <stdint.h>

void ir_rx_init(void);
uint8_t ir_rx_carrier_detected(void);
uint8_t ir_rx_recent_activity(uint32_t window_us);
uint8_t ir_rx_event_latched(void);
void ir_rx_clear_event(void);

#endif /* IR_RX_H_ */
