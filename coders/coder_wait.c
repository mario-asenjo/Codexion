/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_wait.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masenjo <masenjo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 00:00:00 by masenjo           #+#    #+#             */
/*   Updated: 2026/08/14 16:35:00 by masenjo          ###   ########.fr       */
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

static void	cx_lock_pair(t_sim *sim, int left, int right)
{
	if (left < right)
	{
		pthread_mutex_lock(&sim->dongles[left].lock);
		pthread_mutex_lock(&sim->dongles[right].lock);
	}
	else
	{
		pthread_mutex_lock(&sim->dongles[right].lock);
		pthread_mutex_lock(&sim->dongles[left].lock);
	}
}

static void	cx_unlock_pair(t_sim *sim, int left, int right)
{
	pthread_mutex_unlock(&sim->dongles[left].lock);
	pthread_mutex_unlock(&sim->dongles[right].lock);
}

static int	cx_is_top(t_sim *sim, int index, int coder_id)
{
	t_request	top;

	if (!cx_heap_peek(&sim->dongles[index].wait_heap, &top))
		return (0);
	return (top.coder_id == coder_id);
}

static int	cx_pair_available(t_coder *coder, int left, int right)
{
	t_sim	*sim;
	long	now;

	sim = coder->sim;
	now = cx_now_ms() - sim->start_ms;
	return (sim->dongles[left].owner_id == 0
		&& sim->dongles[right].owner_id == 0
		&& sim->dongles[left].available_at_ms <= now
		&& sim->dongles[right].available_at_ms <= now);
}

static int	cx_try_grant(t_coder *coder, t_request *request)
{
	t_sim	*sim;
	int		left;
	int		right;
	int		granted;

	sim = coder->sim;
	left = cx_left_dongle(coder);
	right = cx_right_dongle(coder);
	cx_lock_pair(sim, left, right);
	granted = cx_is_top(sim, left, coder->id)
		&& cx_is_top(sim, right, coder->id)
		&& cx_pair_available(coder, left, right);
	if (granted)
	{
		cx_heap_pop(&sim->dongles[left].wait_heap, &sim->cfg, request);
		cx_heap_pop(&sim->dongles[right].wait_heap, &sim->cfg, request);
		sim->dongles[left].owner_id = coder->id;
		sim->dongles[right].owner_id = coder->id;
	}
	cx_unlock_pair(sim, left, right);
	return (granted);
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
	cx_lock_pair(sim, left, right);
	ok = cx_heap_push(&sim->dongles[left].wait_heap, &sim->cfg, *request);
	if (ok)
		ok = cx_heap_push(&sim->dongles[right].wait_heap, &sim->cfg, *request);
	cx_unlock_pair(sim, left, right);
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
