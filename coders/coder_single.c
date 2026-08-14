/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_single.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masenjo <masenjo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/14 16:40:00 by masenjo           #+#    #+#             */
/*   Updated: 2026/08/14 16:40:00 by masenjo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	cx_single_should_stop(t_sim *sim)
{
	int	stop;

	pthread_mutex_lock(&sim->state_lock);
	stop = sim->stop;
	pthread_mutex_unlock(&sim->state_lock);
	return (stop);
}

void	cx_single_coder(t_coder *coder)
{
	t_sim	*sim;
	int		taken;

	sim = coder->sim;
	taken = 0;
	pthread_mutex_lock(&sim->state_lock);
	pthread_mutex_lock(&sim->dongles[0].lock);
	if (!sim->stop && sim->dongles[0].owner_id == 0)
	{
		sim->dongles[0].owner_id = coder->id;
		taken = 1;
	}
	pthread_mutex_unlock(&sim->dongles[0].lock);
	pthread_mutex_unlock(&sim->state_lock);
	if (taken)
		cx_log_state(sim, coder->id, "has taken a dongle");
	while (!cx_single_should_stop(sim))
		cx_sleep_ms(1);
}
