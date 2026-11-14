/**
  * @file    fsm.h
  * @author  Leonardo Costamagna
  * @brief   Header file containing API declarations, data types,
  *          and context structures for the Finite State Machine module.
  */

#ifndef FSM_H
#define FSM_H

/* Ensures C linkage compatibility when compiled with C++, preventing name mangling. */
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
  * @defgroup FSM_Driver Finite State Machine Module
  * @{
  */

/**
  * @defgroup FSM_Types Data Types & Forward Declarations
  * @brief Core types, pointers, and structures for state machine execution.
  * @{
  */

/** Forward declaration of the FSM instance structure */
typedef struct fsm_t fsm_t;

/** @brief Type definition for state identifiers */
typedef uint8_t state_t;

/** @brief Type definition for event identifiers */
typedef uint8_t event_t;

/**
  * @brief Function pointer type for transition guard conditions.
  * @param fsm   Pointer to the FSM instance.
  * @param event Event triggering the evaluation.
  * @return true to allow transition, false to reject it.
  */
typedef bool (*fsm_guard_t)(fsm_t *fsm, event_t event);

/**
  * @brief Function pointer type for transition actions.
  * @param fsm   Pointer to the FSM instance.
  * @param event Event triggering the action.
  */
typedef void (*fsm_action_t)(fsm_t *fsm, event_t event);

/**
  * @brief Declarative transition mapping structure used for table definition.
  */
typedef struct
{
  state_t        curr_state;  /**< Current state */
  event_t        event;       /**< Triggering event */
  state_t        next_state;  /**< Destination state */
  fsm_guard_t    guard;       /**< Optional guard condition function pointer */
  fsm_action_t   action;      /**< Optional transition action function pointer */
} fsm_transition_t;

/**
  * @brief Internal matrix cell structure optimized for O(1) runtime lookups.
  */
typedef struct
{
  state_t        next_state;  /**< Destination state */
  fsm_guard_t    guard;       /**< Guard condition function pointer */
  fsm_action_t   action;      /**< Transition action function pointer */
  bool           is_valid;    /**< Flag indicating if the transition is valid/defined */
} fsm_cell_t;

/**
  * @brief Main Finite State Machine instance context structure.
  */
struct fsm_t
{
  state_t          current_state; /**< Active state of the machine */
  fsm_cell_t      *matrix;        /**< Pointer to the compiled O(1) indexed transition matrix */
  size_t           num_states;    /**< Total number of states managed by the FSM */
  size_t           num_events;    /**< Total number of events managed by the FSM */
  void            *user_data;     /**< Pointer to application-specific user context data */
};

/**
  * @} FSM_Types
  */

/**
  * @defgroup FSM_Core_API High-Level Application Interface
  * @brief Primary API function declarations for state machine initialization and execution.
  * @{
  */

/**
  * @brief  Initializes the FSM instance and compiles the declarative table into an O(1) matrix.
  * @param  fsm            Pointer to the FSM instance.
  * @param  initial_state  Initial starting state.
  * @param  matrix         Pointer to allocated memory for the transition matrix.
  * @param  num_states     Total number of states in the FSM.
  * @param  num_events     Total number of events in the FSM.
  * @param  table          Pointer to the declarative transition array.
  * @param  table_size     Number of elements in the transition array.
  * @param  user_data      Pointer to application-specific context data (can be NULL).
  * @return Execution status (0: Success, <0: Invalid arguments or failure).
  */
int32_t fsm_init(fsm_t *fsm,
                 state_t initial_state,
                 fsm_cell_t *matrix,
                 size_t num_states,
                 size_t num_events,
                 const fsm_transition_t *table,
                 size_t table_size,
                 void *user_data);

/**
  * @brief  Dispatches an event to the FSM, executing guards, actions, and state transitions in O(1) time.
  * @param  fsm    Pointer to the FSM instance.
  * @param  event  Event to dispatch.
  * @return Execution status (0: Success / Processed, <0: Invalid parameters or unhandled event).
  */
int32_t fsm_dispatch(fsm_t *fsm, event_t event);

/**
  * @} FSM_Core_API
  */

/**
  * @} FSM_Driver
  */

#ifdef __cplusplus
}
#endif

#endif /* FSM_H */