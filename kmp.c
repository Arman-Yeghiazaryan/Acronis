#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void pref_func(char* str, int* a, int size){
	int j = 0, i = 1;
	a[0] = 0;
	while(i < size){
		if (str[i] == str[j]){
			a[i] = j +1;
			i++;
			j++;
		}
		else{
			if (j == 0){
				a[i] = 0;
				i++;
			}
			else{
				j = a[j - 1];
			}
		}
	}
}

int main(int argc, char** argv){
	int k = 0, l = 0;
	char a;
	int len = strlen(argv[1]);
	int* pi = calloc(len, sizeof(int));
	pref_func(argv[1], pi, len);
	while((a = getchar()) != EOF){
		/*if (a == ':')
			break;*/
		if (a == argv[1][l]){
			printf("%c", a);
			k++;
			l++;
			if (l == len){
				printf("*");
				l = 0;
			}
		}
		else if (l == 0){
			printf("%c", a);
			k++;
			}
			else{
				l = pi[l - 1];
			}
	}
	return 0;
}