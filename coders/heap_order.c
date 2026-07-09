/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_order.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masenjo <masenjo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 00:00:00 by masenjo           #+#    #+#             */
/*   Updated: 2026/07/02 00:00:00 by masenjo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	cx_heap_swap(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

static int	cx_fifo_tie_break(t_request a, t_request b)
{
	if (a.seq != b.seq)
		return (a.seq < b.seq);
	return (a.coder_id < b.coder_id);
}

int	cx_edf_tie_break(t_request a, t_request b)
{
	if (a.coder_id != b.coder_id)
		return (a.coder_id < b.coder_id);
	return (a.seq < b.seq);
}

int	cx_heap_request_before(t_config *cfg, t_request a, t_request b)
{
	if (cfg != NULL && cfg->scheduler == CX_SCHED_EDF)
	{
		if (a.deadline_ms != b.deadline_ms)
			return (a.deadline_ms < b.deadline_ms);
		return (cx_edf_tie_break(a, b));
	}
	return (cx_fifo_tie_break(a, b));
}
