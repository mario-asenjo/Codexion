#include "codexion.h"

int	cx_log_init(t_log *log, long start_ms)
{
	if (log == NULL)
		return (0);
	log->start_ms = start_ms;
	if (pthread_mutex_init(&log->lock, NULL) != 0)
		return (0);
	return (1);
}

void	cx_log_destroy(t_log *log)
{
	if (log != NULL)
		pthread_mutex_destroy(&log->lock);
}

void	cx_log_state(t_log *log, int coder_id, const char *message)
{
	long	timestamp;

	if (log == NULL || message == NULL)
		return ;
	pthread_mutex_lock(&log->lock);
	timestamp = cx_now_ms() - log->start_ms;
	printf("%ld %d %s\n", timestamp, coder_id, message);
	pthread_mutex_unlock(&log->lock);
}
