#include "codexion.h"

static void	cx_destroy_dongle_locks(t_sim *sim, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&sim->dongles[i].lock);
		i++;
	}
}

static void	cx_init_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->cfg.number_of_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].compiles_done = 0;
		sim->coders[i].last_compile_start_ms = 0;
		sim->coders[i].sim = sim;
		i++;
	}
}

static int	cx_init_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->cfg.number_of_coders)
	{
		sim->dongles[i].id = i;
		sim->dongles[i].owner_id = 0;
		sim->dongles[i].available_at_ms = 0;
		if (pthread_mutex_init(&sim->dongles[i].lock, NULL) != 0)
		{
			cx_destroy_dongle_locks(sim, i);
			return (0);
		}
		i++;
	}
	return (1);
}

static void	cx_free_owned_memory(t_sim *sim)
{
	free(sim->coders);
	free(sim->dongles);
	sim->coders = NULL;
	sim->dongles = NULL;
}

static int	cx_alloc_sim_arrays(t_sim *sim)
{
	sim->coders = malloc(sizeof(t_coder)
			* (size_t)sim->cfg.number_of_coders);
	if (sim->coders == NULL)
		return (0);
	sim->dongles = malloc(sizeof(t_dongle)
			* (size_t)sim->cfg.number_of_coders);
	if (sim->dongles == NULL)
	{
		cx_free_owned_memory(sim);
		return (0);
	}
	memset(sim->coders, 0, sizeof(t_coder)
		* (size_t)sim->cfg.number_of_coders);
	memset(sim->dongles, 0, sizeof(t_dongle)
		* (size_t)sim->cfg.number_of_coders);
	return (1);
}

static int	cx_init_sync(t_sim *sim)
{
	if (pthread_mutex_init(&sim->state_lock, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&sim->log_lock, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->state_lock);
		return (0);
	}
	if (pthread_cond_init(&sim->state_changed, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->log_lock);
		pthread_mutex_destroy(&sim->state_lock);
		return (0);
	}
	return (1);
}

static void	cx_destroy_sync(t_sim *sim)
{
	pthread_cond_destroy(&sim->state_changed);
	pthread_mutex_destroy(&sim->log_lock);
	pthread_mutex_destroy(&sim->state_lock);
}

int	cx_sim_init(t_sim *sim, t_config *cfg)
{
	if (sim == NULL || cfg == NULL)
		return (0);
	memset(sim, 0, sizeof(*sim));
	sim->cfg = *cfg;
	sim->start_ms = cx_now_ms();
	sim->request_seq = 0;
	sim->stop = 0;
	sim->burned_coder_id = 0;
	sim->completed_coders = 0;
	if (!cx_alloc_sim_arrays(sim))
		return (0);
	cx_init_coders(sim);
	if (!cx_init_dongles(sim))
	{
		cx_free_owned_memory(sim);
		return (0);
	}
	if (!cx_heap_init(&sim->wait_heap, sim->cfg.number_of_coders))
	{
		cx_destroy_dongle_locks(sim, sim->cfg.number_of_coders);
		cx_free_owned_memory(sim);
		return (0);
	}
	if (!cx_init_sync(sim))
	{
		cx_heap_destroy(&sim->wait_heap);
		cx_destroy_dongle_locks(sim, sim->cfg.number_of_coders);
		cx_free_owned_memory(sim);
		return (0);
	}
	return (1);
}

void	cx_sim_destroy(t_sim *sim)
{
	if (sim == NULL)
		return ;
	cx_destroy_sync(sim);
	cx_heap_destroy(&sim->wait_heap);
	cx_destroy_dongle_locks(sim, sim->cfg.number_of_coders);
	cx_free_owned_memory(sim);
	memset(sim, 0, sizeof(*sim));
}
