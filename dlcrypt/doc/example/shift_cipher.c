#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* 
    移位密码(凯撒密码)， 一点都不安全，只做学习之用
*/

char *dict = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int main(){
    int         m, len, i, index;
    int         k;
    char        *x, *y, *z, *cur;
    
    k = 100;
    m = 26;
    x = "ATTACK";
    
    len = strlen(x);
    
    y = malloc(len + 1);
    z = malloc(len + 1);
    
    /* encrypt */
    for(i = 0; i < len; i++){
        index = ((x[i] - 65) + k % m) % m;
        y[i] = dict[index];
    }
    y[i] = '\0';
    
    printf("enc: %s\n", y);
    
    /* decrypt */
    for(i = 0; i < len; i++){
        index = (y[i] - 65) - k % m;
        if(index < 0) index += m;
        
        z[i] = dict[index];
    }
    z[i] = '\0';
    
    printf("dec: %s\n", z);
    
    free(y);
    free(z);
    return 1;
}