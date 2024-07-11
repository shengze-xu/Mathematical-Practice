#include <stdio.h>

int main(){
    float a[300],b[300]={0};
    FILE *fpRead = fopen("mouse6.txt","r");  
    for(int i=0;i<55;i++){
        fscanf(fpRead,"%f %f",&a[i],&b[i]);
        printf("{%f,%f},",a[i],b[i]);
    }
    return 0;
}


