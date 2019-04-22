#include <stdio.h>

int exgcd(int r0,int r1,int *s,int *t)
{
    int s1,t1,s0,t0,c;
    s0=1; t0=0;
    s1=0; t1=1;
    
    *s = 0; *t = 1;
    
    int r = r0 % r1;
    int q = (r0 - r) / r1;
    
    while(r)
    {
        *s = s0 - q*s1;
        *t = t0 - q*t1;
        s0=s1;
        t0=t1;
        s1 = *s;
        t1 = *t;
        
        r0 = r1;
        r1 = r;
        r = r0 % r1;
        
        q = (r0 - r) / r1;
    }
    
    return r1;
}

int main(){
    int r0,r1,s,t,c;
    
    r0 = 973;
    r1 = 301;
    
    c = exgcd(r0, r1, &s, &t);
    
    printf("%d %d %d\n", c, s, t);
}