#include "codexion.h"

long	cx_now_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long)tv.tv_sec * 1000L + (long)tv.tv_usec / 1000L);
}

void	cx_sleep_ms(long duration_ms)
{
	long	start;
	long	elapsed;
	long	remaining;

	if (duration_ms <= 0)
		return ;
	start = cx_now_ms();
	elapsed = 0;
	while (elapsed < duration_ms)
	{
		remaining = duration_ms - elapsed;
		if (remaining > 10)
			remaining = 10;
		usleep((unsigned int)(remaining * 1000L));
		elapsed = cx_now_ms() - start;
	}
}
