# Finite State Machine (FSM) Module

## Overview

The **Finite State Machine (FSM)** is a high-performance, table-driven state machine module designed for deterministic, low-latency execution in embedded systems and control logic applications. Traditional nested switch-case implementations can become brittle and complex as state spaces expand; the FSM algorithm overcomes this limitation by dynamically compiling a declarative transition table into an optimized $O(1)$ indexed jump matrix at boot time. Written in pure C with zero external runtime dependencies, it provides robust support for guard conditions, transition actions, and user context attachments, making it ideal for real-time embedded systems and event-driven architectures.

### Key Features

* **$O(1)$ State Dispatch Complexity:** Transforms a declarative transition array into a direct indexed memory matrix at initialization, ensuring constant-time event dispatching with zero runtime loops.


* **Guard Conditions & Actions:** Supports optional function pointers for transition guards (to validate criteria before switching states) and transition actions (to execute code during state changes).


* **Quality & Verification:** Verified using Ceedling/Unity complemented by Doxygen-compatible inline documentation.



---

## Controller Performance & Simulation

The following response graph demonstrates the deterministic structure and execution flow of the FSM module, highlighting rapid, condition-safe event handling and predictable state transitions:

---

## Project Structure

```text
fsm/
├── docs/                   # Doxygen configuration
├── fsm.h                   # Public API
├── fsm.c                   # State machine algorithm implementation
├── test/                   # Unit testing environment
│   ├── support/            # Auxiliary headers and Ceedling mocks
│   ├── project.yml         # Ceedling project configuration
│   └── test_fsm.c          # Unit test suite for state machine logic
└── README.md               # Main project documentation


```

---

## Integration and Getting Started

### Integration Scheme

Integration consists of defining your states and events, instantiating a transition table along with a compiled matrix buffer, initializing via `fsm_init()`, and executing `fsm_dispatch()` whenever an application event occurs.

---

### Integration Steps

1. Copy `fsm.h` and `fsm.c` into your project's source tree.


2. Include `"fsm.h"` in your application module.


3. Declare your states, events, transition table, and matrix cell buffer.
4. Call `fsm_init()` passing your context reference, states, events, and table configuration.


5. Periodically or event-driven invoke `fsm_dispatch(&fsm, event)` whenever a new event takes place.



---

### Code Examples

#### Example Usage

The following example demonstrates how to initialize the FSM and dispatch events in a real-time event loop.

```c
/**
 * @file main.c
 * @brief Minimal usage example for the FSM module.
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdbool.h>
#include "fsm.h"

// Define states
typedef enum {
    STATE_IDLE = 0,
    STATE_RUNNING,
    STATE_MAX
} app_states_t;

// Define events
typedef enum {
    EVENT_START = 0,
    EVENT_STOP,
    EVENT_MAX
} app_events_t;

// Optional action handler
static void on_start_action(fsm_t *fsm, event_t event) {
    printf("Action: Transitioning to RUNNING state.\n");
}

int main(void) {
    fsm_t my_fsm;
    fsm_cell_t matrix[STATE_MAX * EVENT_MAX];

    // Define declarative transition table
    const fsm_transition_t table[] = {
        { STATE_IDLE, EVENT_START, STATE_RUNNING, NULL, on_start_action },
        { STATE_RUNNING, EVENT_STOP, STATE_IDLE, NULL, NULL }
    };

    // 1. Initialize the FSM instance and compile the transition table
    if (fsm_init(&my_fsm, STATE_IDLE, matrix, STATE_MAX, EVENT_MAX, table, sizeof(table)/sizeof(table[0]), NULL) != 0) {
        printf("FSM Initialization failed!\n");
        return 1;
    }

    printf("FSM initialized. Current state: IDLE\n");

    // 2. Dispatch an event to trigger a state transition
    printf("Dispatching EVENT_START...\n");
    if (fsm_dispatch(&my_fsm, EVENT_START) == 0) {
        printf("Event handled successfully. Current state: RUNNING\n");
    }

    // 3. Dispatch stop event
    printf("Dispatching EVENT_STOP...\n");
    if (fsm_dispatch(&my_fsm, EVENT_STOP) == 0) {
        printf("Event handled successfully. Current state: IDLE\n");
    }

    return 0;
}

```

---

## Running Unit Tests

The development environment uses **Ceedling** (along with **Unity** and **CMock**) for hardware-decoupled unit testing.

### Prerequisites

* **Ruby** (for executing the Ceedling engine)
* **Ceedling** (unit testing framework for C)
* **gcovr** (optional, for coverage reporting)

### Execution Commands

1. Navigate to the test directory:

```bash
cd test

```

2. Run the full unit test suite:

```bash
ceedling test:all

```

3. Generate code coverage report:

```bash
ceedling gcov:all

```

The resulting HTML report will be available at:
`test/build/artifacts/gcov/gcovr/index.html`

---

## Generating Documentation

The public API is fully documented using **Doxygen**-compatible comment blocks.

### Option 1: Via Command Line

1. Navigate to the `docs/` directory:

```bash
cd docs

```

2. Build the documentation using the `doxyfile` configuration file:

```bash
doxygen doxyfile

```

3. Open the generated file in your web browser:

```bash
xdg-open html/index.html

```

### Option 2: Via Graphical User Interface

1. Launch Doxywizard from the terminal:

```bash
doxywizard

```

2. Select **File > Open** and open the `doxyfile` inside the `docs/` directory.
3. Select the **Run** tab -> Click **Run doxygen**.
4. Open `html/index.html` in your browser.

---

## License

This project is distributed under the **MIT** License.