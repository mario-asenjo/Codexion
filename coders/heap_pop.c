/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_pop.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masenjo <masenjo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 00:00:00 by masenjo           #+#    #+#             */
/*   Updated: 2026/07/02 00:00:00 by masenjo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	cx_heap_pop(t_heap *heap, t_config *cfg, t_request *out)
{
	if (heap == NULL || heap->items == NULL || heap->size <= 0)
		return (0);
	if (out != NULL)
		*out = heap->items[0];
	heap->size--;
	if (heap->size > 0)
	{
		heap->items[0] = heap->items[heap->size];
		cx_heap_sift_down(heap, cfg, 0);
	}
	return (1);
}

int	cx_heap_peek(t_heap *heap, t_request *out)
{
	if (heap == NULL || heap->items == NULL || heap->size <= 0
		|| out == NULL)
		return (0);
	*out = heap->items[0];
	return (1);
}
