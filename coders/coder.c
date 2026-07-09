/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masenjo <masenjo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 00:00:00 by masenjo           #+#    #+#             */
/*   Updated: 2026/07/02 00:00:00 by masenjo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	cx_should_stop(t_sim *sim)
{
	int	stop;

	pthread_mutex_lock(&sim->state_lock);
	stop = sim->stop;
	pthread_mutex_unlock(&sim->state_lock);
	return (stop);
}

static int	cx_mark_compile_done(t_coder *coder)
{
	t_sim	*sim;
	int		stop;

	sim = coder->sim;
	pthread_mutex_lock(&sim->state_lock);
	coder->compiles_done++;
	stop = 0;
	if (coder->compiles_done == sim->cfg.number_of_compiles_required)
	{
		sim->completed_coders++;
		if (sim->completed_coders == sim->cfg.number_of_coders)
		{
			sim->stop = 1;
			stop = 1;
			pthread_cond_broadcast(&sim->state_changed);
		}
	}
	pthread_mutex_unlock(&sim->state_lock);
	return (stop);
}

static void	cx_compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->sim->state_lock);
	coder->last_compile_start_ms = cx_now_ms() - coder->sim->start_ms;
	pthread_mutex_unlock(&coder->sim->state_lock);
	cx_log_state(coder->sim, coder->id, "has taken a dongle");
	cx_log_state(coder->sim, coder->id, "has taken a dongle");
	cx_log_state(coder->sim, coder->id, "is compiling");
	cx_sleep_ms(coder->sim->cfg.time_to_compile);
}

static void	cx_debug_and_refactor(t_coder *coder)
{
	cx_log_state(coder->sim, coder->id, "is debugging");
	cx_sleep_ms(coder->sim->cfg.time_to_debug);
	cx_log_state(coder->sim, coder->id, "is refactoring");
	cx_sleep_ms(coder->sim->cfg.time_to_refactor);
}

void	*cx_coder_routine(void *arg)
{
	t_coder		*coder;
	t_request	request;

	coder = (t_coder *)arg;
	while (!cx_should_stop(coder->sim))
	{
		if (!cx_coder_wait_turn(coder, &request))
			return (NULL);
		cx_compile(coder);
		cx_coder_release(coder);
		if (cx_should_stop(coder->sim))
			return (NULL);
		if (cx_mark_compile_done(coder))
			return (NULL);
		cx_debug_and_refactor(coder);
	}
	return (NULL);
}
