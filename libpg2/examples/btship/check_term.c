#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define SERVER_IP "127.0.0.1"
#define GAME_NAME "battle_john_jmartins"

int launch_battle(const char *player) {
	int pid;
	if ((pid = fork()) == 0)  { // child
		execlp("btship2", "btship2", player, SERVER_IP, GAME_NAME, NULL);
		return -1;
	}
	return pid;
}


int main() {
	
	while(1) {
		int status1, status2;
		
		int pid1 = launch_battle("jmartins");
		int pid2 = launch_battle("john");
		
		waitpid(pid1, &status1, 0);
		waitpid(pid2, &status2, 0);
		
		if (!WIFEXITED(status1)) {
			printf("bad termination for jmartins\n");
			break;
		}
		else if (!WIFEXITED(status2)) {
			printf("bad termination for john\n");
			break;
		}
		sleep(1);
	}
}
