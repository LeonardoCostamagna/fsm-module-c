/**
 * @file test_fsm.c
 * @brief Unit tests for the Finite State Machine (FSM) Module using Ceedling and Unity.
 */

#include "unity.h"
#include "fsm.h"

/* Definición de estados y eventos de prueba */
typedef enum {
    STATE_INIT = 0,
    STATE_IDLE,
    STATE_RUNNING,
    STATE_ERROR,
    STATE_MAX_STATES
} test_state_t;

typedef enum {
    EVENT_START = 0,
    EVENT_STOP,
    EVENT_FAIL,
    EVENT_RESET,
    EVENT_MAX_EVENTS
} test_event_t;

static fsm_t fsm;
static fsm_cell_t matrix[STATE_MAX_STATES * EVENT_MAX_EVENTS];

/* Variables auxiliares para verificar guards y acciones */
static bool guard_allow = true;
static int action_called_count = 0;
static void *last_user_data_seen = NULL;

static bool sample_guard(fsm_t *fsm_inst, event_t event) {
    (void)event;
    last_user_data_seen = fsm_inst->user_data;
    return guard_allow;
}

static void sample_action(fsm_t *fsm_inst, event_t event) {
    (void)event;
    action_called_count++;
    last_user_data_seen = fsm_inst->user_data;
}

void setUp(void) {
    guard_allow = true;
    action_called_count = 0;
    last_user_data_seen = NULL;
}

void tearDown(void) {
}

/* ============================================================================
   1. DEFENSIVE PROGRAMMING & BOUNDARY TESTS (NULL Pointers & Invalid Inputs)
   ============================================================================ */

void test_fsm_null_context_and_invalid_inputs(void) {
    const fsm_transition_t table[] = {
        { STATE_INIT, EVENT_START, STATE_IDLE, NULL, NULL }
    };

    /* Verificaciones defensivas de fsm_init */
    TEST_ASSERT_EQUAL_INT32(-1, fsm_init(NULL, STATE_INIT, matrix, STATE_MAX_STATES, EVENT_MAX_EVENTS, table, 1, NULL));
    TEST_ASSERT_EQUAL_INT32(-1, fsm_init(&fsm, STATE_INIT, NULL, STATE_MAX_STATES, EVENT_MAX_EVENTS, table, 1, NULL));

    /* Inicialización correcta para pruebas de dispatch */
    TEST_ASSERT_EQUAL_INT32(0, fsm_init(&fsm, STATE_INIT, matrix, STATE_MAX_STATES, EVENT_MAX_EVENTS, table, 1, NULL));

    /* Verificaciones defensivas de fsm_dispatch */
    TEST_ASSERT_EQUAL_INT32(-1, fsm_dispatch(NULL, EVENT_START));
    
    fsm_t bad_fsm = {0}; /* matrix es NULL */
    TEST_ASSERT_EQUAL_INT32(-1, fsm_dispatch(&bad_fsm, EVENT_START));

    /* Eventos o estados fuera de rango (Out of bounds) */
    TEST_ASSERT_EQUAL_INT32(-2, fsm_dispatch(&fsm, EVENT_MAX_EVENTS));
    
    fsm.current_state = STATE_MAX_STATES;
    TEST_ASSERT_EQUAL_INT32(-2, fsm_dispatch(&fsm, EVENT_START));
}

/* ============================================================================
   2. INITIALIZATION & TABLE COMPILATION
   ============================================================================ */

void test_fsm_initialization_and_compilation(void) {
    int dummy_user_data = 42;
    const fsm_transition_t table[] = {
        { STATE_INIT, EVENT_START, STATE_IDLE, NULL, NULL },
        { STATE_IDLE, EVENT_STOP, STATE_INIT, NULL, NULL }
    };

    int32_t result = fsm_init(&fsm, STATE_INIT, matrix, STATE_MAX_STATES, EVENT_MAX_EVENTS, table, 2, &dummy_user_data);

    TEST_ASSERT_EQUAL_INT32(0, result);
    TEST_ASSERT_EQUAL_UINT8(STATE_INIT, fsm.current_state);
    TEST_ASSERT_EQUAL_PTR(matrix, fsm.matrix);
    TEST_ASSERT_EQUAL_INT(STATE_MAX_STATES, fsm.num_states);
    TEST_ASSERT_EQUAL_INT(EVENT_MAX_EVENTS, fsm.num_events);
    TEST_ASSERT_EQUAL_PTR(&dummy_user_data, fsm.user_data);
}

/* ============================================================================
   3. UNHANDLED TRANSITIONS & DISPATCH BEHAVIOR
   ============================================================================ */

void test_fsm_unhandled_transition(void) {
    const fsm_transition_t table[] = {
        { STATE_INIT, EVENT_START, STATE_IDLE, NULL, NULL }
    };

    fsm_init(&fsm, STATE_INIT, matrix, STATE_MAX_STATES, EVENT_MAX_EVENTS, table, 1, NULL);

    /* Disparar un evento sin transición definida debe retornar -3 y mantener el estado */
    int32_t result = fsm_dispatch(&fsm, EVENT_STOP);
    TEST_ASSERT_EQUAL_INT32(-3, result);
    TEST_ASSERT_EQUAL_UINT8(STATE_INIT, fsm.current_state);
}

/* ============================================================================
   4. GUARDS AND ACTIONS EXECUTION
   ============================================================================ */

void test_fsm_guard_rejection_and_acceptance(void) {
    int dummy_context = 99;
    const fsm_transition_t table[] = {
        { STATE_INIT, EVENT_START, STATE_RUNNING, sample_guard, sample_action }
    };

    fsm_init(&fsm, STATE_INIT, matrix, STATE_MAX_STATES, EVENT_MAX_EVENTS, table, 1, &dummy_context);

    /* 1. Guard en falso -> Transición rechazada (-4), acción no ejecutada, estado inalterado */
    guard_allow = false;
    int32_t result = fsm_dispatch(&fsm, EVENT_START);

    TEST_ASSERT_EQUAL_INT32(-4, result);
    TEST_ASSERT_EQUAL_UINT8(STATE_INIT, fsm.current_state);
    TEST_ASSERT_EQUAL_INT(0, action_called_count);
    TEST_ASSERT_EQUAL_PTR(&dummy_context, last_user_data_seen);

    /* 2. Guard en verdadero -> Transición permitida (0), acción ejecutada, estado actualizado */
    guard_allow = true;
    result = fsm_dispatch(&fsm, EVENT_START);

    TEST_ASSERT_EQUAL_INT32(0, result);
    TEST_ASSERT_EQUAL_UINT8(STATE_RUNNING, fsm.current_state);
    TEST_ASSERT_EQUAL_INT(1, action_called_count);
    TEST_ASSERT_EQUAL_PTR(&dummy_context, last_user_data_seen);
}