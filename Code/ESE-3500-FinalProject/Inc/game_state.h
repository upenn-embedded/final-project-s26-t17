#ifndef GAME_STATE_H_
#define GAME_STATE_H_

#include <stdint.h>

#define CONTROL_FLAG_RESET_REQUESTED   (1U << 0)
#define CONTROL_FLAG_CHASER_IS_CAR_A   (1U << 1)

typedef enum
{
    GAME_STATE_READY = 0,
    GAME_STATE_TAGGED
} game_state_mode_t;

typedef struct
{
    game_state_mode_t mode;
    uint8_t tag_score;
    uint8_t tag_latched;
} game_state_t;

void game_state_init(game_state_t *state);
void game_state_update(game_state_t *state, uint8_t ir_detected, uint8_t reset_requested);
uint8_t game_state_is_tagged(const game_state_t *state);

#endif /* GAME_STATE_H_ */
