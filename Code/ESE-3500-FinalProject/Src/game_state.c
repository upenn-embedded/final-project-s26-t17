#include "game_state.h"

#define GAME_STATE_TAG_THRESHOLD   1U

void game_state_init(game_state_t *state)
{
    state->mode = GAME_STATE_READY;
    state->tag_score = 0U;
    state->tag_latched = 0U;
}

void game_state_update(game_state_t *state, uint8_t ir_detected, uint8_t reset_requested)
{
    if (reset_requested != 0U)
    {
        state->mode = GAME_STATE_READY;
        state->tag_score = 0U;
        state->tag_latched = 0U;
        return;
    }

    if (state->mode == GAME_STATE_TAGGED)
    {
        state->tag_latched = 1U;
        return;
    }

    if (ir_detected != 0U)
    {
        if (state->tag_score < GAME_STATE_TAG_THRESHOLD)
        {
            state->tag_score++;
        }
    }
    else if (state->tag_score > 0U)
    {
        state->tag_score--;
    }

    if (state->tag_score >= GAME_STATE_TAG_THRESHOLD)
    {
        state->mode = GAME_STATE_TAGGED;
        state->tag_latched = 1U;
    }
}

uint8_t game_state_is_tagged(const game_state_t *state)
{
    return (state->mode == GAME_STATE_TAGGED) ? 1U : 0U;
}
