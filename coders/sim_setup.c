/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_setup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masenjo <masenjo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 00:00:00 by masenjo           #+#    #+#             */
/*   Updated: 2026/07/02 00:00:00 by masenjo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	cx_init_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->cfg.number_of_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].compiles_done = 0;
		sim->coders[i].last_compile_start_ms = 0;
		sim->coders[i].sim = sim;
		i++;
	}
}

void	cx_init_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->cfg.number_of_coders)
	{
		sim->dongles[i].id = i;
		sim->dongles[i].owner_id = 0;
		sim->dongles[i].available_at_ms = 0;
		i++;
	}
}

void	cx_free_owned_memory(t_sim *sim)
{
	free(sim->coders);
	free(sim->dongles);
	sim->coders = NULL;
	sim->dongles = NULL;
}

int	cx_alloc_sim_arrays(t_sim *sim)
{
	sim->coders = malloc(sizeof(t_coder) * sim->cfg.number_of_coders);
	if (sim->coders == NULL)
		return (0);
	sim->dongles = malloc(sizeof(t_dongle) * sim->cfg.number_of_coders);
	if (sim->dongles == NULL)
	{
		cx_free_owned_memory(sim);
		return (0);
	}
	memset(sim->coders, 0, sizeof(t_coder) * sim->cfg.number_of_coders);
	memset(sim->dongles, 0, sizeof(t_dongle) * sim->cfg.number_of_coders);
	return (1);
}
