#include <stdio.h>

int main(int argc, char *argv[], char *envp[]){
	for(int i=0; i< argc; i++){
	printf("%s\n", argv[i]);
	}

	int nenv= 0;
	char*cur_ev = envp[nenv];
	while (cur_ev != NULL) {
	printf ("%s\n", cur_ev);
	nenv++;
	cur_ev = envp[nenv];
	}

	return 0; 
}

