#include "exec.h"

#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../builtin/echo.h"

static int exec_builtin(char **words)
{
    char *cmd = words[0];
    if (!strcmp(cmd, "true"))
        return 0;
    else if (!strcmp(cmd, "false"))
        return 1;
    else if (!strcmp(cmd, "echo"))
        return echo_b(words + 1);
    return -1;
}

/* Description:
 *  	execute les commandes avec les args donnes
 * Arguments:
 *  	words: la ligne de commande a executer
 * Retour:
 *  	0 si succes, 1 sinon
 * Verbose:
 *  	execute la commande en words[0] via un appelle si builtin, via fork->
 *  	execvp sinon.
 */
int exec_cmd(char **words)
{
    if (!words || !words[0])
        return 0;
    int r = exec_builtin(words);
    if (r == -1)
    {
        int child = fork();
        if (!child)
        {
            execvp(words[0], words);
            _exit(127);
        }
        int wstat;
        waitpid(child, &wstat, 0);
        r = WEXITSTATUS(wstat);
    }
    return r;
}
