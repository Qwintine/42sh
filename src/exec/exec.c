#include "exec.h"

static int exec_builtin(char **words)
{
    char *cmd = words[0];
    if (!strcmp(cmd, "true"))
        return 0;
    else if (!strcmp(cmd, "false"))
        return 1;
    else if (!strcmp(cmd, "echo"))
        return echo_b(words);
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
    int r = exec_builtin(words);
    if (r != -1)
    {
        int child = fork();
        if (!child)
        {
            execvp(words[0], words);
            return 1;
        }
        int wstat;
        waitpid(child, &wstat, 0);
        r = WEXITSTATUS(wstat);
    }
    return r;
}
