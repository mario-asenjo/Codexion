#include "codexion.h"

int	main(int argc, char **argv)
{
	t_config	cfg;

	memset(&cfg, 0, sizeof(cfg));
	if (!cx_parse_config(argc, argv, &cfg))
	{
		cx_print_usage();
		return (1);
	}
	return (0);
}
