/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_wait.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masenjo <masenjo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 00:00:00 by masenjo           #+#    #+#             */
/*   Updated: 2026/07/02 00:00:00 by masenjo          ###   ########.fr       */
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

static int	cx_dongles_available(t_coder *coder)
{
	t_sim	*sim;
	long	now;
	int		left;
	int		right;

	sim = coder->sim;
	now = cx_now_ms() - sim->start_ms;
	left = cx_left_dongle(coder);
	right = cx_right_dongle(coder);
	return (sim->dongles[left].owner_id == 0
		&& sim->dongles[right].owner_id == 0
		&& sim->dongles[left].available_at_ms <= now
		&& sim->dongles[right].available_at_ms <= now);
}

static int	cx_is_my_turn(t_coder *coder, t_request *request)
{
	t_request	top;

	if (!cx_heap_peek(&coder->sim->wait_heap, &top))
		return (0);
	return (top.coder_id == request->coder_id && cx_dongles_available(coder));
}

int	cx_coder_wait_turn(t_coder *coder, t_request *request)
{
	t_sim	*sim;

	sim = coder->sim;
	pthread_mutex_lock(&sim->state_lock);
	request->coder_id = coder->id;
	request->seq = sim->request_seq++;
	request->deadline_ms = coder->last_compile_start_ms
		+ sim->cfg.time_to_burnout;
	if (!cx_heap_push(&sim->wait_heap, &sim->cfg, *request))
		return (pthread_mutex_unlock(&sim->state_lock), 0);
	while (!sim->stop && !cx_is_my_turn(coder, request))
	{
		pthread_mutex_unlock(&sim->state_lock);
		cx_sleep_ms(1);
		pthread_mutex_lock(&sim->state_lock);
	}
	if (sim->stop)
		return (pthread_mutex_unlock(&sim->state_lock), 0);
	cx_heap_pop(&sim->wait_heap, &sim->cfg, request);
	sim->dongles[cx_left_dongle(coder)].owner_id = coder->id;
	sim->dongles[cx_right_dongle(coder)].owner_id = coder->id;
	return (pthread_mutex_unlock(&sim->state_lock), 1);
}
