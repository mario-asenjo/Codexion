/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_release.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masenjo <masenjo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 00:00:00 by masenjo           #+#    #+#             */
/*   Updated: 2026/08/17 21:20:00 by masenjo          ###   ########.fr       */
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

void	cx_coder_release(t_coder *coder)
{
	t_sim	*sim;
	long	next;
	int		left;
	int		right;

	sim = coder->sim;
	left = cx_left_dongle(coder);
	right = cx_right_dongle(coder);
	pthread_mutex_lock(&sim->state_lock);
	cx_lock_dongle_pair(sim, left, right);
	next = cx_now_ms() - sim->start_ms + sim->cfg.dongle_cooldown;
	sim->dongles[left].owner_id = 0;
	sim->dongles[left].available_at_ms = next;
	if (left != right)
	{
		sim->dongles[right].owner_id = 0;
		sim->dongles[right].available_at_ms = next;
	}
	coder->granted = 0;
	cx_unlock_dongle_pair(sim, left, right);
	pthread_mutex_unlock(&sim->state_lock);
}
