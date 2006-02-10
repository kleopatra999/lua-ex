#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#include <unistd.h>
#include <limits.h>
#include <sys/types.h>

#include "posix_spawn.h"

#define nelemof(A) (sizeof A / sizeof *A)

int posix_spawn_file_actions_init(posix_spawn_file_actions_t *act)
{
	act->dups[0] = act->dups[1] = act->dups[2] = -1;
	return 0;
}

int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t *act, int d, int n)
{
	if (d < 0 || OPEN_MAX < d || n < 0 || OPEN_MAX < n) {
		errno = EBADF;
		return -1;
	}
	if (2 < d) {
		errno = EINVAL;
		return -1;
	}
	act->dups[d] = n;
	return 0;
}

int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *act)
{
	return 0;
}

int posix_spawnp(pid_t *restrict ppid,
		         const char *restrict path,
				 const posix_spawn_file_actions_t *act,
				 const posix_spawnattr_t *restrict attrp,
				 char *const argv[restrict],
				 char *const envp[restrict])
{
	if (!ppid || !path || !argv || !envp)
		return EINVAL;
	if (attrp)
		return EINVAL;
	/* check act actions? */
	switch (*ppid = fork()) {
	case -1: return -1;
	default: return 0;
	case 0:
		if (act) {
			size_t i;
			for (i = 0; i < nelemof(act->dups); i++)
				if (act->dups[i] > -1)
					dup2(i, act->dups[i]);
		}
		environ = (char **)envp;
		execvp(path, argv);
		_exit(EXIT_FAILURE);
		/*NOTREACHED*/
	}
}
