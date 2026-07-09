/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_release.c                                    :+:      :+:    :+:   */
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

void	cx_coder_release(t_coder *coder)
{
	t_sim	*sim;
	long	next;

	sim = coder->sim;
	pthread_mutex_lock(&sim->state_lock);
	next = cx_now_ms() - sim->start_ms + sim->cfg.dongle_cooldown;
	sim->dongles[cx_left_dongle(coder)].owner_id = 0;
	sim->dongles[cx_right_dongle(coder)].owner_id = 0;
	sim->dongles[cx_left_dongle(coder)].available_at_ms = next;
	sim->dongles[cx_right_dongle(coder)].available_at_ms = next;
	pthread_cond_broadcast(&sim->state_changed);
	pthread_mutex_unlock(&sim->state_lock);
}
