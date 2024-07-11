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
    for(i=0;i<=pline->number-2;i++){//求h[i]
        h[i]=pline->x[i+1]-pline->x[i];
        //printf("h[%d]=%f\n",i,h[i]);
    }

    for(i=0;i<=pline->number-1;i++){//求a[i]和b[i]
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
    }//求解系数矩阵

    for(i=0;i<=pline->number-1;i++){//求A[i]和B[i]
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
    
    for(i=pline->number-1;i>=0;i--){//求m[i]
        if(i==pline->number-1){
            m[i]=B[i];
        }
        else{
            m[i]=A[i]*m[i+1]+B[i];
        }
    }

    for(i=0;i<=pline->number-1;i++){
        printf("%.3f ",m[i]);
        //printf("m[%d]=%f\n",i,m[i]);
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
    }//求解系数矩阵

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

int cubicspline_cond3(pSPLINE pline){
    return 0;
}

SPLINE line1;
pSPLINE pline1=&line1;

int main(){
    line1.number=26;
    FILE *fp=fopen("pandadata.txt","r");

    for(int i=0;i<line1.number;i++){
        fscanf(fp,"%f %f",&line1.x[i],&line1.y[i]);
    }
    int cond=1;
    line1.begin_k1 = 0;  
    line1.end_k1 = 0;  
    //line1.begin_k2 = 1;  
    //line1.end_k2 = 2;
    if(cond==1){
        cubicspline_cond1(pline1);
    }
    else if(cond==2){
        cubicspline_cond2(pline1);
    }
    else{
        cubicspline_cond3(pline1);
    }
    
    return 0;
}