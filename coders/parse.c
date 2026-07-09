/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masenjo <masenjo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 00:00:00 by masenjo           #+#    #+#             */
/*   Updated: 2026/07/02 00:00:00 by masenjo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	cx_is_digit(char c)
{
	return (c >= '0' && c <= '9');
}

static int	cx_parse_long(const char *str, long min, long max, long *out)
{
	long	value;
	int		i;
	int		digit;

	if (str == NULL || str[0] == '\0')
		return (0);
	value = 0;
	i = 0;
	while (str[i] != '\0')
	{
		if (!cx_is_digit(str[i]))
			return (0);
		digit = str[i] - '0';
		if (value > (max - digit) / 10)
			return (0);
		value = value * 10 + digit;
		i++;
	}
	if (value < min || value > max)
		return (0);
	*out = value;
	return (1);
}

static int	cx_parse_int_arg(const char *str, int min, int max, int *out)
{
	long	value;

	if (!cx_parse_long(str, min, max, &value))
		return (0);
	*out = (int)value;
	return (1);
}

static int	cx_parse_scheduler(const char *str, int *scheduler)
{
	if (strcmp(str, "fifo") == 0)
	{
		*scheduler = CX_SCHED_FIFO;
		return (1);
	}
	if (strcmp(str, "edf") == 0)
	{
		*scheduler = CX_SCHED_EDF;
		return (1);
	}
	return (0);
}

int	cx_parse_config(int argc, char **argv, t_config *cfg)
{
	if (argc != 9 || cfg == NULL)
		return (0);
	if (!cx_parse_int_arg(argv[1], 1, CX_MAX_CODERS,
			&cfg->number_of_coders))
		return (0);
	if (!cx_parse_long(argv[2], 1, CX_MAX_TIME_MS, &cfg->time_to_burnout))
		return (0);
	if (!cx_parse_long(argv[3], 0, CX_MAX_TIME_MS, &cfg->time_to_compile))
		return (0);
	if (!cx_parse_long(argv[4], 0, CX_MAX_TIME_MS, &cfg->time_to_debug))
		return (0);
	if (!cx_parse_long(argv[5], 0, CX_MAX_TIME_MS, &cfg->time_to_refactor))
		return (0);
	if (!cx_parse_int_arg(argv[6], 1, CX_MAX_COMPILES,
			&cfg->number_of_compiles_required))
		return (0);
	if (!cx_parse_long(argv[7], 0, CX_MAX_TIME_MS, &cfg->dongle_cooldown))
		return (0);
	if (!cx_parse_scheduler(argv[8], &cfg->scheduler))
		return (0);
	return (1);
}
