/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_grant.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masenjo <masenjo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/14 16:40:00 by masenjo           #+#    #+#             */
/*   Updated: 2026/08/17 21:20:00 by masenjo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	cx_pair_indexes(t_sim *sim, int coder_id, int *left, int *right)
{
	*left = coder_id - 1;
	*right = coder_id % sim->cfg.number_of_coders;
}

static int	cx_pair_available(t_sim *sim, int left, int right, long now)
{
	return (sim->dongles[left].owner_id == 0
		&& sim->dongles[right].owner_id == 0
		&& sim->dongles[left].available_at_ms <= now
		&& sim->dongles[right].available_at_ms <= now);
}

static int	cx_grant_request(t_sim *sim, t_request request, long now)
{
	t_coder	*coder;
	int		left;
	int		right;
	int		granted;

	coder = &sim->coders[request.coder_id - 1];
	cx_pair_indexes(sim, request.coder_id, &left, &right);
	cx_lock_dongle_pair(sim, left, right);
	granted = cx_pair_available(sim, left, right, now);
	if (granted)
	{
		sim->dongles[left].owner_id = request.coder_id;
		sim->dongles[right].owner_id = request.coder_id;
		coder->granted = 1;
		coder->last_compile_start_ms = now;
	}
	cx_unlock_dongle_pair(sim, left, right);
	return (granted);
}

static void	cx_restore_deferred(t_sim *sim, t_request *deferred, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		cx_heap_push(&sim->wait_heap, &sim->cfg, deferred[i]);
		i++;
	}
}

void	cx_dispatch_grants(t_sim *sim)
{
	t_request	deferred[CX_MAX_CODERS];
	t_request	request;
	long		now;
	int			count;

	now = cx_now_ms() - sim->start_ms;
	count = 0;
	while (sim->wait_heap.size > 0)
	{
		cx_heap_pop(&sim->wait_heap, &sim->cfg, &request);
		if (!cx_grant_request(sim, request, now))
			deferred[count++] = request;
	}
	cx_restore_deferred(sim, deferred, count);
}
