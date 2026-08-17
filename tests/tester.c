#define _POSIX_C_SOURCE 200809L

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ARGS 16
#define ACTION_LEN 64
#define TOLERANCE_MS 3

typedef struct s_run
{
	int		exit_code;
	int		timed_out;
	char	*output;
} t_run;

typedef struct s_log
{
	long	ts;
	int		coder;
	char	action[ACTION_LEN];
} t_log;

typedef struct s_logs
{
	t_log	*items;
	int		count;
} t_logs;

static int g_pass = 0;
static int g_fail = 0;
static const char *g_binary = "./codexion";

static void result(int ok, const char *message)
{
	if (ok)
	{
		printf("  [PASS] %s\n", message);
		g_pass++;
	}
	else
	{
		printf("  [FAIL] %s\n", message);
		g_fail++;
	}
}

static void section(const char *title)
{
	printf("\n=====================================================\n");
	printf("  %s\n", title);
	printf("=====================================================\n");
}

static void subsection(const char *title)
{
	printf("\n--- %s ---\n", title);
}

static void sleep_ms(long ms)
{
	struct timespec req;

	req.tv_sec = ms / 1000;
	req.tv_nsec = (ms % 1000) * 1000000L;
	while (nanosleep(&req, &req) != 0 && errno == EINTR)
		;
}

static char *dup_text(const char *s)
{
	char *copy;

	copy = malloc(strlen(s) + 1);
	if (copy != NULL)
		strcpy(copy, s);
	return (copy);
}

static int make_argv(const char *args, char **argv, char **storage)
{
	char *token;
	int count;

	*storage = dup_text(args);
	if (*storage == NULL)
		return (-1);
	argv[0] = (char *)g_binary;
	count = 1;
	token = strtok(*storage, " ");
	while (token != NULL && count < MAX_ARGS - 1)
	{
		argv[count++] = token;
		token = strtok(NULL, " ");
	}
	argv[count] = NULL;
	return (count);
}

static char *read_fd_all(int fd)
{
	struct stat st;
	char *buf;
	ssize_t got;
	ssize_t total;

	if (fstat(fd, &st) != 0)
		return (NULL);
	buf = malloc((size_t)st.st_size + 1);
	if (buf == NULL)
		return (NULL);
	if (lseek(fd, 0, SEEK_SET) < 0)
		return (free(buf), NULL);
	total = 0;
	while (total < st.st_size)
	{
		got = read(fd, buf + total, (size_t)(st.st_size - total));
		if (got <= 0)
			break ;
		total += got;
	}
	buf[total] = '\0';
	return (buf);
}

static int wait_with_timeout(pid_t pid, int timeout_ms, int *status)
{
	int elapsed;
	pid_t ret;

	elapsed = 0;
	while (elapsed <= timeout_ms)
	{
		ret = waitpid(pid, status, WNOHANG);
		if (ret == pid)
			return (1);
		if (ret < 0)
			return (0);
		sleep_ms(10);
		elapsed += 10;
	}
	kill(pid, SIGKILL);
	waitpid(pid, status, 0);
	return (0);
}

static int run_program(const char *args, int timeout_ms, t_run *run)
{
	char tmpl[] = "/tmp/codexion_tester_XXXXXX";
	char *argv[MAX_ARGS];
	char *storage;
	int fd;
	int status;
	pid_t pid;

	memset(run, 0, sizeof(*run));
	if (make_argv(args, argv, &storage) < 0)
		return (0);
	fd = mkstemp(tmpl);
	if (fd < 0)
		return (free(storage), 0);
	unlink(tmpl);
	pid = fork();
	if (pid == 0)
	{
		dup2(fd, STDOUT_FILENO);
		dup2(fd, STDERR_FILENO);
		execv(g_binary, argv);
		_exit(127);
	}
	if (pid < 0)
		return (close(fd), free(storage), 0);
	run->timed_out = !wait_with_timeout(pid, timeout_ms, &status);
	if (WIFEXITED(status))
		run->exit_code = WEXITSTATUS(status);
	else
		run->exit_code = 128;
	run->output = read_fd_all(fd);
	close(fd);
	free(storage);
	return (run->output != NULL);
}

static void free_run(t_run *run)
{
	free(run->output);
	run->output = NULL;
}

static int known_action(const char *s)
{
	return (strcmp(s, "has taken a dongle") == 0
		|| strcmp(s, "is compiling") == 0
		|| strcmp(s, "is debugging") == 0
		|| strcmp(s, "is refactoring") == 0
		|| strcmp(s, "burned out") == 0);
}

static int count_lines(const char *s)
{
	int n;

	n = 0;
	while (*s != '\0')
	{
		if (*s == '\n')
			n++;
		s++;
	}
	return (n + 1);
}

static int parse_logs(const char *output, int coders, t_logs *logs)
{
	char *copy;
	char *line;
	char *save;
	long ts;
	int id;
	int used;
	char action[ACTION_LEN];

	logs->items = calloc((size_t)count_lines(output), sizeof(t_log));
	copy = dup_text(output);
	if (logs->items == NULL || copy == NULL)
		return (free(logs->items), free(copy), 0);
	logs->count = 0;
	line = strtok_r(copy, "\n", &save);
	while (line != NULL)
	{
		used = 0;
		if (sscanf(line, "%ld %d %63[^\n]%n", &ts, &id, action, &used) != 3
			|| used != (int)strlen(line) || id < 1 || id > coders
			|| ts < 0 || !known_action(action))
			return (free(copy), free(logs->items), logs->items = NULL, 0);
		logs->items[logs->count].ts = ts;
		logs->items[logs->count].coder = id;
		strcpy(logs->items[logs->count].action, action);
		logs->count++;
		line = strtok_r(NULL, "\n", &save);
	}
	free(copy);
	return (logs->count > 0);
}

static void free_logs(t_logs *logs)
{
	free(logs->items);
	logs->items = NULL;
	logs->count = 0;
}

static int logs_monotonic(t_logs *logs)
{
	int i;

	i = 1;
	while (i < logs->count)
	{
		if (logs->items[i].ts < logs->items[i - 1].ts)
			return (0);
		i++;
	}
	return (1);
}

static int count_action(t_logs *logs, const char *action)
{
	int i;
	int count;

	count = 0;
	i = 0;
	while (i < logs->count)
	{
		if (strcmp(logs->items[i].action, action) == 0)
			count++;
		i++;
	}
	return (count);
}

static int validate_two_takes(t_logs *logs, int coders)
{
	int *takes;
	int i;
	int id;

	takes = calloc((size_t)coders, sizeof(int));
	if (takes == NULL)
		return (0);
	i = 0;
	while (i < logs->count)
	{
		id = logs->items[i].coder - 1;
		if (strcmp(logs->items[i].action, "has taken a dongle") == 0)
			takes[id]++;
		else if (strcmp(logs->items[i].action, "is compiling") == 0)
		{
			if (takes[id] != 2)
				return (free(takes), 0);
			takes[id] = 0;
		}
		i++;
	}
	free(takes);
	return (1);
}

static int all_compiled(t_logs *logs, int coders, int required, int *missing)
{
	int *counts;
	int i;

	counts = calloc((size_t)coders, sizeof(int));
	if (counts == NULL)
		return (0);
	i = 0;
	while (i < logs->count)
	{
		if (strcmp(logs->items[i].action, "is compiling") == 0)
			counts[logs->items[i].coder - 1]++;
		i++;
	}
	*missing = 0;
	i = 0;
	while (i < coders)
	{
		if (counts[i] < required)
			(*missing)++;
		i++;
	}
	free(counts);
	return (*missing == 0);
}

static int validate_phase_timing(t_logs *logs, int coders, long compile_ms,
	long debug_ms)
{
	long *compile_at;
	long *debug_at;
	int i;
	int id;
	int ok;

	compile_at = malloc(sizeof(long) * (size_t)coders);
	debug_at = malloc(sizeof(long) * (size_t)coders);
	if (compile_at == NULL || debug_at == NULL)
		return (free(compile_at), free(debug_at), 0);
	for (i = 0; i < coders; i++)
		compile_at[i] = debug_at[i] = -1;
	ok = 1;
	for (i = 0; i < logs->count && ok; i++)
	{
		id = logs->items[i].coder - 1;
		if (strcmp(logs->items[i].action, "is compiling") == 0)
			compile_at[id] = logs->items[i].ts;
		else if (strcmp(logs->items[i].action, "is debugging") == 0)
		{
			if (compile_at[id] >= 0 && logs->items[i].ts - compile_at[id]
				< compile_ms - TOLERANCE_MS)
				ok = 0;
			debug_at[id] = logs->items[i].ts;
		}
		else if (strcmp(logs->items[i].action, "is refactoring") == 0
			&& debug_at[id] >= 0 && logs->items[i].ts - debug_at[id]
			< debug_ms - TOLERANCE_MS)
			ok = 0;
	}
	free(compile_at);
	free(debug_at);
	return (ok);
}

static int burnout_is_last(t_logs *logs)
{
	if (logs->count == 0)
		return (0);
	return (strcmp(logs->items[logs->count - 1].action, "burned out") == 0);
}

static int validate_burnout_deadline(t_logs *logs, int coders, long burnout)
{
	long *last;
	long expected;
	int i;
	int id;
	int ok;

	last = calloc((size_t)coders, sizeof(long));
	if (last == NULL)
		return (0);
	ok = 0;
	for (i = 0; i < logs->count; i++)
	{
		id = logs->items[i].coder - 1;
		if (strcmp(logs->items[i].action, "is compiling") == 0)
			last[id] = logs->items[i].ts;
		else if (strcmp(logs->items[i].action, "burned out") == 0)
		{
			expected = last[id] + burnout;
			ok = logs->items[i].ts >= expected - TOLERANCE_MS
				&& logs->items[i].ts <= expected + 10 + TOLERANCE_MS;
			break ;
		}
	}
	free(last);
	return (ok);
}

static int validate_cooldown_n2(t_logs *logs, long compile_ms, long cooldown)
{
	long previous;
	int i;

	previous = -1;
	for (i = 0; i < logs->count; i++)
	{
		if (strcmp(logs->items[i].action, "is compiling") != 0)
			continue ;
		if (previous >= 0 && logs->items[i].ts - previous
			< compile_ms + cooldown - TOLERANCE_MS)
			return (0);
		previous = logs->items[i].ts;
	}
	return (previous >= 0);
}

static int valid_case(const char *name, const char *args, int coders,
	int required, int timeout_ms, int phase_timing)
{
	t_run run;
	t_logs logs;
	int ok;
	int missing;

	subsection(name);
	if (!run_program(args, timeout_ms, &run))
		return (result(0, "No se pudo ejecutar el binario"), 0);
	result(!run.timed_out && run.exit_code == 0, "Termina correctamente");
	ok = parse_logs(run.output, coders, &logs);
	result(ok, "Formato de log correcto");
	if (!ok)
		return (free_run(&run), 0);
	result(logs_monotonic(&logs), "Timestamps no decrecientes");
	result(count_action(&logs, "burned out") == 0, "Nadie se agoto");
	ok = all_compiled(&logs, coders, required, &missing);
	result(ok, "Todos alcanzaron las compilaciones requeridas");
	if (!ok)
		printf("  i Coders incompletos: %d (consecuencia, no FAIL extra)\n", missing);
	result(validate_two_takes(&logs, coders),
		"Cada compile tiene exactamente dos adquisiciones previas");
	if (phase_timing)
		result(validate_phase_timing(&logs, coders, 80, 120),
			"Timing compile/debug/refactor respetado");
	free_logs(&logs);
	free_run(&run);
	return (ok);
}

static void invalid_tests(void)
{
	const char *args[] = {"", "5 1000", "5 1000 80 80 80 1 0 fifo extra",
		"0 1000 80 80 80 1 0 fifo", "x 1000 80 80 80 1 0 fifo",
		"5 1000 80 80 80 1 0 nope"};
	const char *names[] = {"sin argumentos", "faltan argumentos",
		"argumento extra", "0 coders", "valor no numerico",
		"scheduler invalido"};
	int i;
	t_run run;

	section("0. VALIDACION DE ARGUMENTOS");
	for (i = 0; i < 6; i++)
	{
		if (!run_program(args[i], 1000, &run))
			result(0, names[i]);
		else
		{
			result(run.exit_code != 0 && !run.timed_out, names[i]);
			free_run(&run);
		}
	}
}

static void easy_tests(void)
{
	int i;
	int before;

	section("1. EASY");
	valid_case("5 coders holgados - fifo",
		"5 3000 80 120 120 3 40 fifo", 5, 3, 8000, 1);
	valid_case("5 coders holgados - edf",
		"5 3000 80 120 120 3 40 edf", 5, 3, 8000, 0);
	subsection("Stress EDF repetido - 100 coders");
	before = g_fail;
	for (i = 0; i < 3; i++)
		valid_case("100 coders / deadline 5000",
			"100 5000 60 60 60 1 0 edf", 100, 1, 8000, 0);
	if (g_fail == before)
		printf("  i Las 3 corridas de 100 coders fueron verdes.\n");
	subsection("Stress EDF repetido - 200 coders");
	for (i = 0; i < 2; i++)
		valid_case("200 coders / deadline 5000",
			"200 5000 60 60 60 1 0 edf", 200, 1, 8000, 0);
}

static void burnout_case(const char *name, const char *args, int coders,
	long burnout)
{
	t_run run;
	t_logs logs;
	int ok;

	subsection(name);
	if (!run_program(args, 3000, &run))
		return (result(0, "No se pudo ejecutar el binario"));
	ok = parse_logs(run.output, coders, &logs);
	result(ok, "Formato de log correcto");
	if (ok)
	{
		result(count_action(&logs, "burned out") == 1,
			"Se detecto exactamente un burnout");
		result(burnout_is_last(&logs), "burned out es la ultima linea");
		result(validate_burnout_deadline(&logs, coders, burnout),
			"Burnout dentro de la ventana temporal esperada");
		free_logs(&logs);
	}
	free_run(&run);
}

static void less_easy_tests(void)
{
	section("2. LESS EASY");
	burnout_case("Burnout forzado", "2 120 200 60 60 1 0 fifo", 2, 120);
	burnout_case("N=1", "1 120 60 60 60 1 0 edf", 1, 120);
}

static void medium_tests(void)
{
	t_run run;
	t_logs logs;
	int ok;

	section("3. MEDIUM");
	subsection("Cooldown determinista N=2");
	if (!run_program("2 4000 80 60 60 3 150 fifo", 8000, &run))
		result(0, "No se pudo ejecutar cooldown");
	else
	{
		ok = parse_logs(run.output, 2, &logs);
		result(ok, "Formato de log correcto");
		if (ok)
		{
			result(count_action(&logs, "burned out") == 0, "Sin burnout");
			result(validate_cooldown_n2(&logs, 80, 150),
				"Cooldown respetado externamente");
			free_logs(&logs);
		}
		free_run(&run);
	}
	valid_case("EDF bajo contencion",
		"25 2500 80 80 80 3 50 edf", 25, 3, 10000, 0);
	valid_case("FIFO bajo contencion",
		"25 3500 80 80 80 3 50 fifo", 25, 3, 12000, 0);
	printf("  i No se compara 'burnouts EDF <= FIFO': esa no es una propiedad\n");
	printf("    exigida por la hoja. Cada scheduler se valida por separado.\n");
}

static void recode_probe(void)
{
	t_run run;
	t_logs logs;
	int first;
	int i;

	section("4. RECODE - EDF TIE BREAKER");
	printf("  i La evaluacion pide preferir coder_id mayor en igualdad de deadline.\n");
	printf("  i El probe es informativo: el SO puede ejecutar un hilo antes de que\n");
	printf("    todos los contendientes hayan llegado a la cola.\n");
	if (!run_program("8 3000 80 80 80 1 0 edf", 5000, &run))
		return ;
	if (parse_logs(run.output, 8, &logs))
	{
		first = 0;
		for (i = 0; i < logs.count; i++)
		{
			if (strcmp(logs.items[i].action, "is compiling") == 0)
			{
				first = logs.items[i].coder;
				break ;
			}
		}
		printf("  i Primer coder observado compilando: %d\n", first);
		free_logs(&logs);
	}
	free_run(&run);
}

static int command_exists(const char *name)
{
	char cmd[256];

	snprintf(cmd, sizeof(cmd), "command -v %s >/dev/null 2>&1", name);
	return (system(cmd) == 0);
}

static void external_tools(void)
{
	section("5. HERRAMIENTAS EXTERNAS");
	if (command_exists("norminette"))
		result(system("norminette coders/*.c coders/*.h >/dev/null") == 0,
			"Norminette");
	else
		printf("  [SKIP] norminette no encontrado\n");
	if (command_exists("valgrind"))
	{
		result(system("valgrind --quiet --error-exitcode=42 --leak-check=full "
			"./codexion 5 3000 100 100 100 2 50 edf >/dev/null") == 0,
			"Valgrind Memcheck");
		result(system("valgrind --quiet --tool=helgrind --error-exitcode=43 "
			"./codexion 4 3000 100 100 100 2 50 edf >/dev/null") == 0,
			"Valgrind Helgrind");
	}
	else
		printf("  [SKIP] valgrind no encontrado\n");
}

int main(int argc, char **argv)
{
	int tools;

	tools = 0;
	if (argc >= 2 && strcmp(argv[1], "--help") == 0)
	{
		printf("Uso: %s [binario] [--tools]\n", argv[0]);
		return (0);
	}
	if (argc >= 2 && strcmp(argv[1], "--tools") != 0)
		g_binary = argv[1];
	if ((argc >= 2 && strcmp(argv[1], "--tools") == 0)
		|| (argc >= 3 && strcmp(argv[2], "--tools") == 0))
		tools = 1;
	printf("Binario: %s\n", g_binary);
	invalid_tests();
	easy_tests();
	less_easy_tests();
	medium_tests();
	recode_probe();
	if (tools)
		external_tools();
	section("RESUMEN");
	printf("  PASS: %d    FAIL: %d\n", g_pass, g_fail);
	printf("  Nota: los FAIL derivados de una misma terminacion no se multiplican.\n");
	printf("  Inspeccion estatica sigue siendo necesaria para globals, funciones\n");
	printf("  prohibidas, mutex por dongle y existencia del hilo monitor.\n");
	return (g_fail != 0);
}
