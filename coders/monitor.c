/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masenjo <masenjo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 00:00:00 by masenjo           #+#    #+#             */
/*   Updated: 2026/07/02 00:00:00 by masenjo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	cx_burned_coder(t_sim *sim, long now)
{
	int		i;
	t_coder	*coder;

	i = 0;
	while (i < sim->cfg.number_of_coders)
	{
		coder = &sim->coders[i];
		if (coder->compiles_done < sim->cfg.number_of_compiles_required
			&& now - coder->last_compile_start_ms >= sim->cfg.time_to_burnout)
			return (coder->id);
		i++;
	}
	return (0);
}

static int	cx_monitor_check(t_sim *sim)
{
	long	now;
	int		burned;

	pthread_mutex_lock(&sim->state_lock);
	if (sim->stop)
		return (pthread_mutex_unlock(&sim->state_lock), 0);
	now = cx_now_ms() - sim->start_ms;
	burned = cx_burned_coder(sim, now);
	if (burned != 0)
	{
		sim->burned_coder_id = burned;
		sim->stop = 1;
		pthread_cond_broadcast(&sim->state_changed);
	}
	pthread_mutex_unlock(&sim->state_lock);
	if (burned != 0)
		cx_log_state(sim, burned, "burned out");
	return (burned == 0);
}

void	*cx_monitor_routine(void *arg)
{
	t_sim	*sim;

	sim = (t_sim *)arg;
	while (cx_monitor_check(sim))
		cx_sleep_ms(1);
	return (NULL);
}
