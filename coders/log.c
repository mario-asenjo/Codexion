/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masenjo <masenjo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 00:00:00 by masenjo           #+#    #+#             */
/*   Updated: 2026/07/02 00:00:00 by masenjo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	cx_log_state(t_sim *sim, int coder_id, const char *message)
{
	long	timestamp;

	if (sim == NULL || message == NULL)
		return ;
	pthread_mutex_lock(&sim->state_lock);
	if (sim->stop && strcmp(message, "burned out") != 0)
	{
		pthread_mutex_unlock(&sim->state_lock);
		return ;
	}
	pthread_mutex_lock(&sim->log_lock);
	timestamp = cx_now_ms() - sim->start_ms;
	printf("%ld %d %s\n", timestamp, coder_id, message);
	pthread_mutex_unlock(&sim->log_lock);
	pthread_mutex_unlock(&sim->state_lock);
}
