/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_wait.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masenjo <masenjo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 00:00:00 by masenjo           #+#    #+#             */
/*   Updated: 2026/08/17 21:20:00 by masenjo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	cx_queue_request(t_coder *coder, t_request *request)
{
	t_sim	*sim;

	sim = coder->sim;
	request->coder_id = coder->id;
	request->seq = sim->request_seq++;
	request->deadline_ms = coder->last_compile_start_ms
		+ sim->cfg.time_to_burnout;
	return (cx_heap_push(&sim->wait_heap, &sim->cfg, *request));
}

static int	cx_wait_grant(t_coder *coder)
{
	t_sim	*sim;
	int		granted;

	sim = coder->sim;
	while (!sim->stop && !coder->granted)
	{
		cx_dispatch_grants(sim);
		if (!coder->granted)
		{
			pthread_mutex_unlock(&sim->state_lock);
			cx_sleep_ms(1);
			pthread_mutex_lock(&sim->state_lock);
		}
	}
	granted = coder->granted;
	return (granted && !sim->stop);
}

int	cx_coder_wait_turn(t_coder *coder, t_request *request)
{
	t_sim	*sim;
	int		granted;

	sim = coder->sim;
	pthread_mutex_lock(&sim->state_lock);
	if (!cx_queue_request(coder, request))
		return (pthread_mutex_unlock(&sim->state_lock), 0);
	granted = cx_wait_grant(coder);
	pthread_mutex_unlock(&sim->state_lock);
	if (!granted)
		return (0);
	cx_log_state(sim, coder->id, "has taken a dongle");
	cx_log_state(sim, coder->id, "has taken a dongle");
	return (1);
}
