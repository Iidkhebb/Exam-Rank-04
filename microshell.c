#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int	print_error(char *str, char *arg)
{
	while (*str)
		write(2, str++, 1);
	if (arg)
		while(*arg)
			write(2, arg++, 1);
	write(2, "\n", 1);
	return (1);
}

int ft_execute(char *av[], int i, int sv, char *env[])
{
	av[i] = NULL;
	dup2(sv, 0);
	close(sv);
	execve(av[0], av, env);
	return (print_error("error: cannot execute ", av[0]));
}

int	main(int ac, char *av[], char *env[])
{
	int	i;
	int fd[2];
	int sv;
	(void)ac;

	i = 0;
	sv = dup(0);
	while (av[i] && av[i + 1]) //check if the end is reached
	{
		av = &av[i + 1];	//the new av start after the ; or |
		i = 0;
		while (av[i] && strcmp(av[i], ";") && strcmp(av[i], "|"))
			i++;
		if (!strcmp(av[0], "cd")) {
			if (i != 2)
				print_error("error: cd: bad arguments", NULL);
			else if (chdir(av[1]) != 0)
				print_error("error: cd: cannot change directory to ", av[1]);
		}
		else if (i != 0 && (av[i] == NULL || strcmp(av[i], ";") == 0)) {
			if (fork() == 0) {
				if (ft_execute(av, i, sv, env))
					return (1);
			}
			else {
				close(sv);
				while(waitpid(-1, NULL, 0) != -1);
				sv = dup(1);
			}
		}
		else if(i != 0 && strcmp(av[i], "|") == 0) {
			pipe(fd);
			if (fork() == 0) {
				dup2(fd[1], 1);
				close(fd[0]);
				close(fd[1]);
				if (ft_execute(av, i, sv, env))
					return (1);
			}
			else {
				close(fd[1]);
				close(sv);
				sv = fd[0];
			}
		}
	}
	close(sv);
	return (0);
}