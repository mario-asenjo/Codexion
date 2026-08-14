/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_lock.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masenjo <masenjo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/14 16:40:00 by masenjo           #+#    #+#             */
/*   Updated: 2026/08/14 16:40:00 by masenjo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	cx_lock_dongle_pair(t_sim *sim, int left, int right)
{
	if (left == right)
		pthread_mutex_lock(&sim->dongles[left].lock);
	else if (left < right)
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

void	cx_unlock_dongle_pair(t_sim *sim, int left, int right)
{
	pthread_mutex_unlock(&sim->dongles[left].lock);
	if (left != right)
		pthread_mutex_unlock(&sim->dongles[right].lock);
}
