#include <stdint.h>

#include "ir_rx.h"
#include "ir_tx.h"
#include "joystick.h"
#include "motor.h"
#include "nrf24.h"
#include "st7735.h"

typedef struct
{
    int16_t motor_a_cmd;
    int16_t motor_b_cmd;
    uint8_t sensor_flag;
    uint8_t sender_id;
} radio_packet_t;

#define SENDER_ID_HUNTER_CONTROLLER  1U
#define SENDER_ID_RUNNER_CONTROLLER  2U
#define SENDER_ID_HUNTER_CAR         3U
#define SENDER_ID_RUNNER_CAR         4U

#define SENSOR_FLAG_NONE             0U
#define SENSOR_FLAG_GAME_OVER        1U

static const uint8_t HUNTER_CTRL_ADDR[5]   = { 'H', 'C', 'A', 'R', '2' };
static const uint8_t RUNNER_CTRL_ADDR[5]   = { 'R', 'C', 'A', 'R', '1' };
static const uint8_t HUNTER_STATUS_ADDR[5] = { 'H', 'S', 'T', '2', '0' };
static const uint8_t RUNNER_STATUS_ADDR[5] = { 'R', 'S', 'T', '1', '0' };

static void delay_cycles(volatile uint32_t count)
{
    while (count--)
    {
    }
}

static void radio_init_common(void)
{
    nrf24_init();
    nrf24_set_channel(76U);
    nrf24_set_payload_size((uint8_t)sizeof(radio_packet_t));
    nrf24_set_retries(0x0FU, 0x0FU);
    nrf24_flush_rx();
    nrf24_flush_tx();
}

static void radio_enter_rx(const uint8_t *addr)
{
    nrf24_stop_rx();
    nrf24_open_rx_pipe(1U, addr);
    nrf24_flush_rx();
    nrf24_start_rx();
}

static uint8_t radio_try_read(radio_packet_t *pkt)
{
    uint8_t len = 0U;
    uint8_t pipe = 0U;

    if (nrf24_available() <= 0)
    {
        return 0U;
    }

    if (nrf24_read((uint8_t *)pkt, &len, &pipe) != NRF24_OK)
    {
        return 0U;
    }

    return (uint8_t)(len == sizeof(radio_packet_t));
}

static int radio_send_packet(const uint8_t *addr, const radio_packet_t *pkt)
{
    nrf24_stop_rx();
    nrf24_open_tx_pipe(addr);
    return nrf24_send((const uint8_t *)pkt, (uint8_t)sizeof(radio_packet_t));
}

static void send_runner_status_to_both(uint8_t sensor_flag)
{
    radio_packet_t status_pkt;

    status_pkt.motor_a_cmd = 0;
    status_pkt.motor_b_cmd = 0;
    status_pkt.sensor_flag = sensor_flag;
    status_pkt.sender_id = SENDER_ID_RUNNER_CAR;

    (void)radio_send_packet(HUNTER_STATUS_ADDR, &status_pkt);
    delay_cycles(20000U);
    (void)radio_send_packet(RUNNER_STATUS_ADDR, &status_pkt);
}

static void draw_controller_screen(uint8_t role_is_hunter,
                                   uint8_t tx_ok,
                                   uint8_t game_over,
                                   uint16_t *last_bg,
                                   uint16_t *last_square)
{
    uint16_t bg_color;
    uint16_t square_color;

    if (game_over != 0U)
    {
        bg_color = ST7735_MAGENTA;
        square_color = ST7735_MAGENTA;
    }
    else
    {
        bg_color = (tx_ok != 0U) ? ST7735_GREEN : ST7735_RED;
        square_color = (role_is_hunter != 0U) ? ST7735_YELLOW : ST7735_BLUE;
    }

    if (*last_bg != bg_color)
    {
        st7735_fill_screen(bg_color);
        *last_bg = bg_color;
        *last_square = 0xFFFFU;
    }

    if (*last_square != square_color)
    {
        st7735_fill_rect(12U, 12U, 28U, 28U, square_color);
        *last_square = square_color;
    }
}

/* Flash this main to Car2 (hunter car). */
//int main(void)
//{
//    radio_packet_t pkt;
//
//    motor_init();
//    radio_init_common();
//    ir_tx_init();
//    radio_enter_rx(HUNTER_CTRL_ADDR);
//
//    while (1)
//    {
//        if (radio_try_read(&pkt) != 0U)
//        {
//            if (pkt.sender_id == SENDER_ID_HUNTER_CONTROLLER)
//            {
//                motor_set_command(MOTOR_A, pkt.motor_a_cmd);
//                motor_set_command(MOTOR_B, pkt.motor_b_cmd);
//            }
//        }
//
//        ir_tx_send_burst_us(600U);
//        delay_cycles(40000U);
//    }
//}

/* Flash this main to Car1 (runner car). */
#if 1
int main(void)
{
    radio_packet_t pkt;
    uint8_t game_over = 0U;
    uint8_t ir_detected = 0U;
    uint8_t got_runner_cmd = 0U;
    uint8_t game_over_status_sent = 0U;

    motor_init();
    radio_init_common();
    ir_rx_init();
    radio_enter_rx(RUNNER_CTRL_ADDR);

    while (1)
    {
        got_runner_cmd = 0U;

        if (radio_try_read(&pkt) != 0U)
        {
            if (pkt.sender_id == SENDER_ID_RUNNER_CONTROLLER)
            {
                got_runner_cmd = 1U;

                if (game_over == 0U)
                {
                    motor_set_command(MOTOR_A, pkt.motor_a_cmd);
                    motor_set_command(MOTOR_B, pkt.motor_b_cmd);
                }
                else
                {
                    motor_stop_all();
                }
            }
        }

        if ((got_runner_cmd != 0U) && (game_over == 0U))
        {
            ir_detected = ir_rx_recent_activity(25000U);
            if (ir_detected != 0U)
            {
                game_over = 1U;

                motor_stop_all();
            }
        }

        if ((got_runner_cmd != 0U) && (game_over != 0U) && (game_over_status_sent == 0U))
        {
            delay_cycles(80000U);
            send_runner_status_to_both(SENSOR_FLAG_GAME_OVER);
            delay_cycles(80000U);
            send_runner_status_to_both(SENSOR_FLAG_GAME_OVER);
            radio_enter_rx(RUNNER_CTRL_ADDR);
            game_over_status_sent = 1U;
        }
        else if (game_over == 0U)
        {
            game_over_status_sent = 0U;
        }
    }
}
#endif

/* Flash this main to the hunter controller (controls Car2). */
#if 0
int main(void)
{
    joystick_data_t js;
    radio_packet_t ctrl_pkt;
    radio_packet_t status_pkt;
    uint8_t game_over = 0U;
    uint8_t tx_ok = 0U;
    uint16_t last_bg = 0xFFFFU;
    uint16_t last_square = 0xFFFFU;

    joystick_init();
    radio_init_common();
    st7735_init();
    draw_controller_screen(1U, 0U, 0U, &last_bg, &last_square);

    while (1)
    {
        joystick_read(&js);

        ctrl_pkt.motor_a_cmd = (game_over != 0U) ? 0 : js.motor_a_cmd;
        ctrl_pkt.motor_b_cmd = (game_over != 0U) ? 0 : js.motor_b_cmd;
        ctrl_pkt.sensor_flag = SENSOR_FLAG_NONE;
        ctrl_pkt.sender_id = SENDER_ID_HUNTER_CONTROLLER;

        tx_ok = (uint8_t)(radio_send_packet(HUNTER_CTRL_ADDR, &ctrl_pkt) == NRF24_OK);

        radio_enter_rx(HUNTER_STATUS_ADDR);
        delay_cycles(40000U);

        if (radio_try_read(&status_pkt) != 0U)
        {
            if ((status_pkt.sender_id == SENDER_ID_RUNNER_CAR) &&
                (status_pkt.sensor_flag == SENSOR_FLAG_GAME_OVER))
            {
                game_over = 1U;
            }
        }

        draw_controller_screen(1U, tx_ok, game_over, &last_bg, &last_square);
    }
}
#endif

/* Flash this main to the runner controller (controls Car1). */
#if 0
int main(void)
{
    joystick_data_t js;
    radio_packet_t ctrl_pkt;
    radio_packet_t status_pkt;
    uint8_t game_over = 0U;
    uint8_t tx_ok = 0U;
    uint8_t runner_reply_seen = 0U;
    uint16_t last_bg = 0xFFFFU;
    uint16_t last_square = 0xFFFFU;

    joystick_init();
    radio_init_common();
    st7735_init();
    draw_controller_screen(0U, 0U, 0U, &last_bg, &last_square);

    while (1)
    {
        joystick_read(&js);

        ctrl_pkt.motor_a_cmd = (game_over != 0U) ? 0 : js.motor_a_cmd;
        ctrl_pkt.motor_b_cmd = (game_over != 0U) ? 0 : js.motor_b_cmd;
        ctrl_pkt.sensor_flag = SENSOR_FLAG_NONE;
        ctrl_pkt.sender_id = SENDER_ID_RUNNER_CONTROLLER;

        tx_ok = (uint8_t)(radio_send_packet(RUNNER_CTRL_ADDR, &ctrl_pkt) == NRF24_OK);

        radio_enter_rx(RUNNER_STATUS_ADDR);
        delay_cycles(40000U);

        if (radio_try_read(&status_pkt) != 0U)
        {
            if ((status_pkt.sender_id == SENDER_ID_RUNNER_CAR) &&
                (status_pkt.sensor_flag == SENSOR_FLAG_GAME_OVER))
            {
                game_over = 1U;
            }
        }

        draw_controller_screen(0U, tx_ok, game_over, &last_bg, &last_square);
    }
}
#endif
