# Architecture

## Candidate main design
Use a central simulation object passed to every thread. No mutable globals. Coders request permission to compile through a scheduler heap protected by `state_lock`. The arbiter grants two dongles atomically only when:
- the request is at the top of the heap;
- both adjacent dongles are available;
- both dongles have passed cooldown;
- the simulation has not stopped.

This avoids the classic deadlock pattern where every coder holds one dongle while waiting for another.

## Recommended structures

```c
typedef struct s_config
{
    int     number_of_coders;
    long    time_to_burnout;
    long    time_to_compile;
    long    time_to_debug;
    long    time_to_refactor;
    int     number_of_compiles_required;
    long    dongle_cooldown;
    int     scheduler;
}   t_config;
```

```c
typedef struct s_dongle
{
    int             id;
    int             owner_id;
    long            available_at_ms;
    pthread_mutex_t lock;
}   t_dongle;
```

```c
typedef struct s_coder
{
    int             id;
    int             compiles_done;
    long            last_compile_start_ms;
    pthread_t       thread;
    struct s_sim    *sim;
}   t_coder;
```

```c
typedef struct s_request
{
    int     coder_id;
    long    seq;
    long    deadline_ms;
}   t_request;
```

```c
typedef struct s_heap
{
    t_request   *items;
    int         size;
    int         capacity;
}   t_heap;
```

```c
typedef struct s_sim
{
    t_config        cfg;
    t_coder         *coders;
    t_dongle        *dongles;
    t_heap          wait_heap;
    pthread_mutex_t state_lock;
    pthread_mutex_t log_lock;
    pthread_cond_t  state_changed;
    pthread_t       monitor_thread;
    long            start_ms;
    long            request_seq;
    int             stop;
    int             burned_coder_id;
    int             completed_coders;
}   t_sim;
```

## Lock ordering
1. `state_lock`
2. `dongle.lock`, only if retained after central arbitration
3. `log_lock`

Never acquire `state_lock` after `log_lock`.

## Scheduler
The heap stores pending compile requests. The comparator is the only place that should know about FIFO or EDF policy.

Recommended EDF comparator:
1. earlier deadline wins;
2. tie-break function decides equal deadlines;
3. sequence can be used for stability.

Recode target: modify only the EDF tie-break function to prefer higher `coder_id` on equal deadline.

## Timing
Use `gettimeofday` and convert to milliseconds. Use relative timestamps from `sim.start_ms`. The monitor should compute the nearest deadline and wait only until that point or a short bounded interval. Cooldown expiration must also wake waiters via timed waits or short precise sleeps.

## Alternatives discarded
- Each coder locks one dongle and then the other: can deadlock without strict global order and does not integrate scheduler priority cleanly.
- Parity strategy: helps classic philosophers but does not fully solve FIFO/EDF fairness with cooldown.
- One global mutex with no heap: simpler but violates scheduler priority requirements.
- Aggressive busy-wait: can hurt timing, CPU use, and evaluation stability.
