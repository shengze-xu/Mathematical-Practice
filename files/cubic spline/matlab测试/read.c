#include <stdio.h>

int main(){
    float a[200],b[200]={0};
    int n=100;
    FILE *fpRead = fopen("panda1.txt","r");  
    int i;
    for(i=0;i<n;i++){
        fscanf(fpRead,"%f %f",&a[i],&b[i]);
    }
    for(i=0;i<n;i++){
        printf("{%f,%f},",a[i],b[i]);
    }
    return 0;
}


