#define _POSIX_C_SOURCE 200809L

#include "cd.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Description:
 * 	Update PWD and OLDPWD vars after cd
 * Arguments:
 * 	struct dictionnary *vars -> dictionnary of vars
 * Return:
 * 	int -> 0 success, 1 failure
 */
static int update_pwd(struct dictionnary *vars)
{
    char **pwd = get_var(vars, "PWD");
    if (pwd == NULL)
        return 1;
    if (pwd[0] == NULL)
    {
        free(pwd);
        return 1;
    }

    char *saved_pwd = pwd[0];
    free(pwd);
    if (saved_pwd == NULL)
        return 1;

    char new_pwd[1024];
    if (!getcwd(new_pwd, sizeof(new_pwd)))
    {
        free(saved_pwd);
        return 1;
    }

    char *oldpwd_var = malloc(strlen(saved_pwd) + 8);
    if (!oldpwd_var)
    {
        free(saved_pwd);
        return 1;
    }
    strcpy(oldpwd_var, "OLDPWD=");
    strcat(oldpwd_var, saved_pwd);
    free(saved_pwd);

    char *pwd_var = malloc(strlen(new_pwd) + 5);
    if (!pwd_var)
    {
        free(oldpwd_var);
        return 1;
    }
    strcpy(pwd_var, "PWD=");
    strcat(pwd_var, new_pwd);

    add_var(vars, oldpwd_var);
    add_var(vars, pwd_var);
    free(oldpwd_var);
    free(pwd_var);

    return 0;
}

// Structure to hold cd path result
struct cd_result
{
    char *path;
    char **var;
};

/* Description:
 * 	Find the target path for cd command
 * Arguments:
 * 	char **args -> args passed to cd
 * 	struct dictionnary *vars -> dictionnary of vars
 * 	int *print_path -> flag indicate path should be printed
 * Return:
 * 	struct cd_result -> contains path + var to free
 */
static struct cd_result find_path(char **args, struct dictionnary *vars,
                                  int *print_path)
{
    struct cd_result result = { NULL, NULL };

    if (args[0] == NULL)
    {
        result.var = get_var(vars, "HOME");
        if (result.var == NULL || result.var[0] == NULL)
        {
            fprintf(stderr, "42sh: cd: HOME not set\n");
            if (result.var)
                free_ex(result.var);
            return result;
        }
        result.path = result.var[0];
    }
    else if (strcmp(args[0], "-") == 0)
    {
        result.var = get_var(vars, "OLDPWD");
        if (result.var == NULL || result.var[0] == NULL)
        {
            fprintf(stderr, "42sh: cd: OLDPWD not set\n");
            if (result.var)
                free_ex(result.var);
            return result;
        }
        result.path = result.var[0];
        *print_path = 1;
    }
    else
    {
        result.path = strdup(args[0]);
        if (!result.path)
            return result;
    }
    return result;
}

/* Description:
 * 	Execute the cd builtin cmd
 * Arguments:
 * 	char **args -> args passed to cd
 * 	struct dictionnary *vars -> dictionnary of vars
 * Return:
 * 	int -> Exit status of cd cmd
 */
int cd_b(char **args, struct dictionnary *vars)
{
    int print_path = 0;
    struct cd_result res = find_path(args, vars, &print_path);

    if (res.path == NULL)
        return 1;

    if (chdir(res.path) != 0)
    {
        fprintf(stderr, "42sh: cd: wrong path: %s\n", res.path);
        if (res.var)
            free_ex(res.var);
        else
            free(res.path);
        return 1;
    }

    if (print_path)
    {
        printf("%s\n", res.path);
        fflush(stdout);
    }

    int ret = update_pwd(vars);
    if (res.var)
        free_ex(res.var);
    else
        free(res.path);

    return ret;
}
