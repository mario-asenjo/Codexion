#include "codexion.h"

int	main(int argc, char **argv)
{
	t_config	cfg;
	t_sim		sim;

	memset(&cfg, 0, sizeof(cfg));
	if (!cx_parse_config(argc, argv, &cfg))
	{
		cx_print_usage();
		return (1);
	}
	if (!cx_sim_init(&sim, &cfg))
	{
		fprintf(stderr, "codexion: initialization failed\n");
		return (1);
	}
	cx_sim_destroy(&sim);
	return (0);
}
