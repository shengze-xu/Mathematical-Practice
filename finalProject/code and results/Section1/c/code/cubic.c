#include  <stdio.h>

#define  MAXNUM  1000  
typedef struct SPLINE{
    float x[MAXNUM+1];    
    float y[MAXNUM+1];   
    float m[MAXNUM+1]; 
    unsigned int number;  
    float begin_k1; 
    float end_k1;     
    float begin_k2; 
    float end_k2;   
}SPLINE,*pSPLINE;  

int cubicspline_cond1(pSPLINE pline){
    
    float h[MAXNUM];
    float a[MAXNUM+1];
    float b[MAXNUM+1];
    float A[MAXNUM+1];
    float B[MAXNUM+1];
    float m[MAXNUM+1];

    int i=0;
    for(i=0;i<=pline->number-2;i++){
        h[i]=pline->x[i+1]-pline->x[i];
        //printf("h[%d]=%f\n",i,h[i]);
    }

    for(i=0;i<=pline->number-1;i++){
        if(i==0){
            a[i]=0;
            b[i]=2*pline->begin_k1;
        }
        else if(i==pline->number-1){
            a[i]=1;
            b[i]=2*pline->end_k1;
        }
        else{
            a[i]=h[i-1]/(h[i-1]+h[i]);
            b[i]=3*((1-a[i])/h[i-1]*(pline->y[i]-pline->y[i-1])+a[i]/h[i]*(pline->y[i+1]-pline->y[i]));
        }
        //printf("a[%d]=%f,b[%d]=%f\n",i,a[i],i,b[i]);
    }
    for(i=0;i<=pline->number-1;i++){
        if(i==0){
            A[i]=-a[i]/2;
            B[i]=b[i]/2;
        }
        else if(i==pline->number-1){
            B[i]=(b[i]-(1-a[i])*B[i-1])/(2+(1-a[i])*A[i-1]);
        }
        else{
            A[i]=-a[i]/(2+(1-a[i])*A[0]);
            B[i]=(b[i]-(1-a[i])*B[i-1])/(2+(1-a[i])*A[i-1]);
        }
        //printf("A[%d]=%f,B[%d]=%f\n",i,A[i],i,B[i]);
    }
    
    for(i=pline->number-1;i>=0;i--){
        if(i==pline->number-1){
            m[i]=B[i];
        }
        else{
            m[i]=A[i]*m[i+1]+B[i];
        }
    }

    for(i=0;i<=pline->number-1;i++){
        //printf("%.3f ",m[i]);
        printf("m[%d]=%f\n",i,m[i]);
    }
}

int cubicspline_cond2(pSPLINE pline){

    float h[MAXNUM];
    float a[MAXNUM+1];
    float b[MAXNUM+1];
    float A[MAXNUM+1];
    float B[MAXNUM+1];
    float m[MAXNUM+1];

    int i=0;
    for(i=0;i<=pline->number-2;i++){
          h[i]=pline->x[i+1]-pline->x[i];
          //printf("h[%d]=%f\n",i,h[i]);
    }
    for(i=0;i<=pline->number-1;i++){
        if(i==0){
            a[i]=1;
            b[i]=3/h[i]*(pline->y[i+1]-pline->y[i])-h[i]/2*pline->begin_k2;
        }
        else if(i==pline->number-1){
            a[i]=0;
            b[i]=3/h[i-1]*(pline->y[i]-pline->y[i-1])+h[i-1]/2*pline->end_k2;
        }
        else{
            a[i]=h[i-1]/(h[i-1]+h[i]);
            b[i]=3*((1-a[i])/h[i-1]*(pline->y[i]-pline->y[i-1])+a[i]/h[i]*(pline->y[i+1]-pline->y[i]));
        }
        //printf("a[%d]=%f,b[%d]=%f\n",i,a[i],i,b[i]);
    }

    for(i=0;i<=pline->number-1;i++){
        if(i==0){
            A[i]=-a[i]/2;
            B[i]=b[i]/2;
        }
        else if(i==pline->number-1){
            B[i]=(b[i]-(1-a[i])*B[i-1])/(2+(1-a[i])*A[i-1]);
        }
        else{
            A[i]=-a[i]/(2+(1-a[i])*A[0]);
            B[i]=(b[i]-(1-a[i])*B[i-1])/(2+(1-a[i])*A[i-1]);
        }
        //printf("A[%d]=%f,B[%d]=%f\n",i,A[i],i,B[i]);
    }
    
    for(i=pline->number-1;i>=0;i--){
        if(i==pline->number-1){
            m[i]=B[i];
        }
        else{
            m[i]=A[i]*m[i+1]+B[i];
        }
    }
    for(i=0;i<=pline->number-1;i++){
        printf("m[%d]=%f\n",i,m[i]);
    }
}

SPLINE line1;
pSPLINE pline1=&line1;

int main(){
    int splinenumber=13;
    char name[20];
    int i,j;
    FILE *fp;
    SPLINE line[splinenumber];
    pSPLINE pline[splinenumber];
    for(i=0;i<splinenumber;i++){
        pline[i]=&line[i];
    }

    line[0].number=101;
    line[1].number=92;
    line[2].number=53;
    line[3].number=15;
    line[4].number=9;
    line[5].number=10;
    line[6].number=13;
    line[7].number=21;
    line[8].number=12;
    line[9].number=10;
    line[10].number=12;
    line[11].number=13;
    line[12].number=31;

    for(i=0;i<splinenumber;i++){
        sprintf(name,"panda%d.txt",i+1);
        fp=fopen(name,"r");
            for(j=0;j<line[i].number;j++){
            fscanf(fp,"%f %f",&line[i].x[j],&line[i].y[j]);
            //printf("%f %f\n",line[i].x[j],line[i].y[j]);
        }
    }

    int cond=2;
    for(i=0;i<splinenumber;i++){
        //line[i].begin_k1 = 0;  
        //line[i].end_k1 = 0;  
        line[i].begin_k2 = 0;  
        line[i].end_k2 = 0;
        printf("The following data is for the curve-%d:\n",i+1);
        if(cond==1){
            cubicspline_cond1(pline[i]);
        }
        else{
            cubicspline_cond2(pline[i]);
        }
    }
    printf("The above result is all curve parameters.\n");
    return 0;
}
