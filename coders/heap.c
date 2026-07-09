/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masenjo <masenjo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 00:00:00 by masenjo           #+#    #+#             */
/*   Updated: 2026/07/02 00:00:00 by masenjo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	cx_heap_init(t_heap *heap, int capacity)
{
	if (heap == NULL || capacity <= 0)
		return (0);
	heap->items = malloc(sizeof(t_request) * (size_t)capacity);
	if (heap->items == NULL)
		return (0);
	heap->size = 0;
	heap->capacity = capacity;
	return (1);
}

void	cx_heap_destroy(t_heap *heap)
{
	if (heap == NULL)
		return ;
	free(heap->items);
	heap->items = NULL;
	heap->size = 0;
	heap->capacity = 0;
}

static void	cx_heap_sift_up(t_heap *heap, t_config *cfg, int index)
{
	int	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (!cx_heap_request_before(cfg, heap->items[index],
				heap->items[parent]))
			return ;
		cx_heap_swap(&heap->items[index], &heap->items[parent]);
		index = parent;
	}
}

void	cx_heap_sift_down(t_heap *heap, t_config *cfg, int index)
{
	int	left;
	int	right;
	int	best;

	while (1)
	{
		left = index * 2 + 1;
		right = index * 2 + 2;
		best = index;
		if (left < heap->size && cx_heap_request_before(cfg,
				heap->items[left], heap->items[best]))
			best = left;
		if (right < heap->size && cx_heap_request_before(cfg,
				heap->items[right], heap->items[best]))
			best = right;
		if (best == index)
			return ;
		cx_heap_swap(&heap->items[index], &heap->items[best]);
		index = best;
	}
}

int	cx_heap_push(t_heap *heap, t_config *cfg, t_request request)
{
	if (heap == NULL || heap->items == NULL || heap->size >= heap->capacity)
		return (0);
	heap->items[heap->size] = request;
	cx_heap_sift_up(heap, cfg, heap->size);
	heap->size++;
	return (1);
}
