#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>

# define CX_SCHED_FIFO 1
# define CX_SCHED_EDF 2
# define CX_MAX_CODERS 200
# define CX_MAX_TIME_MS 1000000L
# define CX_MAX_COMPILES 1000000

struct s_sim;

typedef struct s_config
{
	int		number_of_coders;
	long	time_to_burnout;
	long	time_to_compile;
	long	time_to_debug;
	long	time_to_refactor;
	int		number_of_compiles_required;
	long	dongle_cooldown;
	int		scheduler;
} 	t_config;

typedef struct s_dongle
{
	int				id;
	int				owner_id;
	long			available_at_ms;
	pthread_mutex_t	lock;
} 	t_dongle;

typedef struct s_coder
{
	int				id;
	int				compiles_done;
	long			last_compile_start_ms;
	pthread_t		thread;
	struct s_sim	*sim;
} 	t_coder;

typedef struct s_request
{
	int		coder_id;
	long	seq;
	long	deadline_ms;
} 	t_request;

typedef struct s_heap
{
	t_request	*items;
	int			size;
	int			capacity;
} 	t_heap;

typedef struct s_log
{
	pthread_mutex_t	lock;
	long			start_ms;
} 	t_log;

typedef struct s_sim
{
	t_config		cfg;
	t_coder			*coders;
	t_dongle		*dongles;
	t_heap			wait_heap;
	pthread_mutex_t	state_lock;
	pthread_mutex_t	log_lock;
	pthread_cond_t	state_changed;
	pthread_t		monitor_thread;
	long			start_ms;
	long			request_seq;
	int				stop;
	int				burned_coder_id;
	int				completed_coders;
} 	t_sim;

int		cx_parse_config(int argc, char **argv, t_config *cfg);
void	cx_print_usage(void);
long	cx_now_ms(void);
void	cx_sleep_ms(long duration_ms);
int		cx_log_init(t_log *log, long start_ms);
void	cx_log_destroy(t_log *log);
void	cx_log_state(t_log *log, int coder_id, const char *message);
int		cx_heap_init(t_heap *heap, int capacity);
void	cx_heap_destroy(t_heap *heap);
int		cx_heap_push(t_heap *heap, t_config *cfg, t_request request);
int		cx_heap_pop(t_heap *heap, t_config *cfg, t_request *out);
int		cx_heap_peek(t_heap *heap, t_request *out);
int		cx_heap_request_before(t_config *cfg, t_request a, t_request b);
int		cx_edf_tie_break(t_request a, t_request b);
int		cx_sim_init(t_sim *sim, t_config *cfg);
void	cx_sim_destroy(t_sim *sim);

#endif
