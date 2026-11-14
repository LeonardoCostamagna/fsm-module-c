/**
  * @file    fsm.c
  * @author  Leonardo Costamagna
  * @brief   Finite State Machine module implementation.
  */

#include "fsm.h"
#include <string.h>

/* Core FSM Operations */

int32_t fsm_init(fsm_t *fsm,
                 state_t initial_state,
                 fsm_cell_t *matrix,
                 size_t num_states,
                 size_t num_events,
                 const fsm_transition_t *table,
                 size_t table_size,
                 void *user_data)
{
  size_t i;
  size_t index;
  const fsm_transition_t *t;

  if ((fsm == NULL) || (matrix == NULL))
  {
    return -1;
  }

  fsm->current_state = initial_state;
  fsm->matrix        = matrix;
  fsm->num_states    = num_states;
  fsm->num_events    = num_events;
  fsm->user_data     = user_data;

  /* Clear the entire transition matrix to invalid states by default */
  memset(fsm->matrix, 0, num_states * num_events * sizeof(fsm_cell_t));

  /* Boot-time compilation loop: Transforms the declarative transition table into an O(1) jump matrix */
  for (i = 0U; i < table_size; i++)
  {
    t = &table[i];

    if ((t->curr_state < num_states) && (t->event < num_events))
    {
      index = ((size_t)t->curr_state * num_events) + (size_t)t->event;

      fsm->matrix[index].next_state = t->next_state;
      fsm->matrix[index].guard      = t->guard;
      fsm->matrix[index].action     = t->action;
      fsm->matrix[index].is_valid   = true;
    }
  }

  return 0;
}

int32_t fsm_dispatch(fsm_t *fsm, event_t event)
{
  size_t index;
  const fsm_cell_t *cell;

  if ((fsm == NULL) || (fsm->matrix == NULL))
  {
    return -1;
  }

  if ((event >= fsm->num_events) || (fsm->current_state >= fsm->num_states))
  {
    return -2;
  }

  /* Direct cell calculation: Pure O(1) execution time with zero runtime loops */
  index = ((size_t)fsm->current_state * fsm->num_events) + (size_t)event;
  cell  = &fsm->matrix[index];

  /* Safely ignore if the transition is not defined in the transition table */
  if (!cell->is_valid)
  {
    return -3;
  }

  /* Evaluate guard condition if one is registered */
  if ((cell->guard != NULL) && !cell->guard(fsm, event))
  {
    return -4; /* Guard condition rejected the transition */
  }

  /* Execute transition action if one is registered */
  if (cell->action != NULL)
  {
    cell->action(fsm, event);
  }

  /* Advance to the next state strictly defined in the table */
  fsm->current_state = cell->next_state;

  return 0;
}