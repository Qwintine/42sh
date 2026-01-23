#define _POSIX_C_SOURCE 200809L

#include "cd.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int update_pwd(struct dictionnary *vars)
{
    char **pwd = get_var(vars, "PWD");
    if (pwd == NULL || pwd[0] == NULL)
        return 1;
    
    char *saved_pwd = strdup(pwd[0]);
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
    strcpy(oldpwd_var, "OLDPWD=");
    strcat(oldpwd_var, saved_pwd);
    free(saved_pwd);
    
    char *pwd_var = malloc(strlen(new_pwd) + 5);
    strcpy(pwd_var, "PWD=");
    strcat(pwd_var, new_pwd);
    
    add_var(vars, oldpwd_var);
    add_var(vars, pwd_var);
    free(oldpwd_var);
    free(pwd_var);
    
    return 0;
}

int cd_b(char **args, struct dictionnary *vars)
{
    char *path;
    char **home = NULL;
    char **oldpwd = NULL;
    int print_path = 0;
    
    if (args[0] == NULL)
    {
        home = get_var(vars, "HOME");
        if (home == NULL || home[0] == NULL)
        {
            return 1;
        }
        path = home[0];
    }
    else if (strcmp(args[0], "-") == 0)
    {
        oldpwd = get_var(vars, "OLDPWD");
        if (oldpwd == NULL || oldpwd[0] == NULL)
        {
            return 1;
        }
        path = oldpwd[0];
        print_path = 1;
    }
    else
    {
        path = args[0];
    }
    
    if (chdir(path) != 0)
    {
        fprintf(stderr, "cd: wrong path: %s\n", path);
        return 1;
    }
    
    char *path_to_print = NULL;
    if (print_path)
    {
        path_to_print = strdup(path);
    }
    
    int result = update_pwd(vars);
    
    if (path_to_print != NULL)
    {
        printf("%s\n", path_to_print);
        fflush(stdout);
        free(path_to_print);
    }
    
    if (home != NULL)
        free(home);
    if (oldpwd != NULL)
        free(oldpwd);
    
    return result;
}