/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_wait.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masenjo <masenjo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 00:00:00 by masenjo           #+#    #+#             */
/*   Updated: 2026/08/14 16:40:00 by masenjo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	cx_left_dongle(t_coder *coder)
{
	return (coder->id - 1);
}

static int	cx_right_dongle(t_coder *coder)
{
	return (coder->id % coder->sim->cfg.number_of_coders);
}

static int	cx_queue_pair(t_coder *coder, t_request *request)
{
	t_sim	*sim;
	int		left;
	int		right;
	int		ok;

	sim = coder->sim;
	left = cx_left_dongle(coder);
	right = cx_right_dongle(coder);
	cx_lock_dongle_pair(sim, left, right);
	ok = cx_heap_push(&sim->dongles[left].wait_heap, &sim->cfg, *request);
	if (ok)
		ok = cx_heap_push(&sim->dongles[right].wait_heap, &sim->cfg, *request);
	cx_unlock_dongle_pair(sim, left, right);
	return (ok);
}

static int	cx_queue_request(t_coder *coder, t_request *request)
{
	t_sim	*sim;

	sim = coder->sim;
	request->coder_id = coder->id;
	request->seq = sim->request_seq++;
	request->deadline_ms = coder->last_compile_start_ms
		+ sim->cfg.time_to_burnout;
	return (cx_queue_pair(coder, request));
}

int	cx_coder_wait_turn(t_coder *coder, t_request *request)
{
	t_sim	*sim;

	sim = coder->sim;
	pthread_mutex_lock(&sim->state_lock);
	if (!cx_queue_request(coder, request))
		return (pthread_mutex_unlock(&sim->state_lock), 0);
	while (!sim->stop && !cx_try_grant(coder, request))
	{
		pthread_mutex_unlock(&sim->state_lock);
		cx_sleep_ms(1);
		pthread_mutex_lock(&sim->state_lock);
	}
	if (sim->stop)
		return (pthread_mutex_unlock(&sim->state_lock), 0);
	pthread_mutex_unlock(&sim->state_lock);
	return (1);
}
