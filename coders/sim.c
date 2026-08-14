/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masenjo <masenjo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 00:00:00 by masenjo           #+#    #+#             */
/*   Updated: 2026/08/14 16:30:00 by masenjo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	cx_init_sync(t_sim *sim)
{
	if (pthread_mutex_init(&sim->state_lock, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&sim->log_lock, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->state_lock);
		return (0);
	}
	return (1);
}

static void	cx_destroy_sync(t_sim *sim)
{
	pthread_mutex_destroy(&sim->log_lock);
	pthread_mutex_destroy(&sim->state_lock);
}

static int	cx_sim_init_storage(t_sim *sim)
{
	if (!cx_alloc_sim_arrays(sim))
		return (0);
	cx_init_coders(sim);
	if (!cx_init_dongles(sim))
	{
		cx_free_owned_memory(sim);
		return (0);
	}
	return (1);
}

int	cx_sim_init(t_sim *sim, t_config *cfg)
{
	if (sim == NULL || cfg == NULL)
		return (0);
	memset(sim, 0, sizeof(*sim));
	sim->cfg = *cfg;
	sim->start_ms = cx_now_ms();
	if (!cx_sim_init_storage(sim))
		return (0);
	if (!cx_init_sync(sim))
	{
		cx_destroy_dongles(sim, sim->cfg.number_of_coders);
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
	cx_destroy_dongles(sim, sim->cfg.number_of_coders);
	cx_free_owned_memory(sim);
	memset(sim, 0, sizeof(*sim));
}
