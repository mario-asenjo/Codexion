/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masenjo <masenjo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/14 16:30:00 by masenjo           #+#    #+#             */
/*   Updated: 2026/08/17 21:20:00 by masenjo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	cx_destroy_dongles(t_sim *sim, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&sim->dongles[i].lock);
		i++;
	}
}

static int	cx_init_one_dongle(t_sim *sim, int i)
{
	t_dongle	*dongle;

	dongle = &sim->dongles[i];
	dongle->id = i;
	dongle->owner_id = 0;
	dongle->available_at_ms = 0;
	if (pthread_mutex_init(&dongle->lock, NULL) != 0)
		return (0);
	return (1);
}

int	cx_init_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->cfg.number_of_coders)
	{
		if (!cx_init_one_dongle(sim, i))
		{
			cx_destroy_dongles(sim, i);
			return (0);
		}
		i++;
	}
	return (1);
}
