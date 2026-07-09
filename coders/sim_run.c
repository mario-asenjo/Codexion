/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_run.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masenjo <masenjo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 00:00:00 by masenjo           #+#    #+#             */
/*   Updated: 2026/07/02 00:00:00 by masenjo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	cx_stop_created(t_sim *sim, int count)
{
	int	i;

	pthread_mutex_lock(&sim->state_lock);
	sim->stop = 1;
	pthread_cond_broadcast(&sim->state_changed);
	pthread_mutex_unlock(&sim->state_lock);
	i = 0;
	while (i < count)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
}

int	cx_sim_run(t_sim *sim)
{
	int	i;

	if (sim->cfg.number_of_coders == 1)
		return (1);
	i = 0;
	while (i < sim->cfg.number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL,
				cx_coder_routine, &sim->coders[i]) != 0)
		{
			cx_stop_created(sim, i);
			return (0);
		}
		i++;
	}
	i = 0;
	while (i < sim->cfg.number_of_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
	return (1);
}
