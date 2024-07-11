#include  <stdio.h>

////////////////////////////////////////////////////////////////////////////////  
#define  MAXNUM  1000   //定义样条数据区间个数最多为1000个  
typedef struct SPLINE    //定义样条结构体，用于存储一条样条的所有信息  
{ //初始化数据输入  
    float x[MAXNUM+1];    //存储样条上的点的x坐标
    float y[MAXNUM+1];    //存储样条上的点的y坐标 
    unsigned int point_num;   //存储样条上的实际的 点 的个数  
    float begin_k1;     //开始点的一阶导数信息  
    float end_k1;     //终止点的一阶导数信息  
    float begin_k2;    //开始点的二阶导数信息  
    float end_k2;     //终止点的二阶导数信息  
    //计算所得的样条函数S(x)  
    float k1[MAXNUM+1];    //所有点的一阶导数信息  
    float k2[MAXNUM+1];    //所有点的二阶导数信息  
    //func[]存储每段的函数系数  
    float a3[MAXNUM],a1[MAXNUM];      
    float b3[MAXNUM],b1[MAXNUM];  
    //分段函数的形式为 Si(x) = a3[i] * {x(i+1) - x}^3  + a1[i] * {x(i+1) - x} +  
    //        b3[i] * {x - x(i)}^3 + b1[i] * {x - x(i)}  
    //xi为x[i]的值，xi_1为x[i+1]的值        
}SPLINE,*pSPLINE;  

typedef int RESULT;      //返回函数执行的结果状态，下面为具体的返回选项  
#ifndef TRUE  
#define TRUE 1  
#endif  

#ifndef FALSE  
#define FALSE -1  
#endif  

#ifndef NULL  
#define NULL 0  
#endif  
#ifndef ERR  
#define ERR  -2  
#endif  
//////////////////////////////////////////////////////////////////////////////////  
  
RESULT Spline3(pSPLINE pLine)  
{  
    float H[MAXNUM] = {0};     //小区间的步长  
    float Fi[MAXNUM] = {0};     //中间量  
    float U[MAXNUM+1] = {0};    //中间量  
    float A[MAXNUM+1] = {0};    //中间量  
    float D[MAXNUM+1] = {0};    //中间量  
    float M[MAXNUM+1] = {0};    //M矩阵  
    float B[MAXNUM+1] = {0};    //追赶法中间量  
    float Y[MAXNUM+1] = {0};    //追赶法中间变量  
    int i = 0;  
    ////////////////////////////////////////计算中间参数  
    if((pLine->point_num < 3) || (pLine->point_num > MAXNUM + 1))  
    {  
        return ERR;       //输入数据点个数太少或太多  
    }  
    for(i = 0;i <= pLine->point_num - 2;i++)  
    {          //求H[i]  
        H[i] = pLine->x[i+1] - pLine->x[i];  
        Fi[i] = (pLine->y[i+1] - pLine->y[i]) / H[i]; //求F[x(i),x(i+1)]  
    }  
    for(i = 1;i <= pLine->point_num - 2;i++)  
    {          //求U[i]和A[i]和D[i]  
        U[i] = H[i-1] / (H[i-1] + H[i]);  
        A[i] = H[i] / (H[i-1] + H[i]);  
        D[i] = 6 * (Fi[i] - Fi[i-1]) / (H[i-1] + H[i]);  
    }  
        //若边界条件为1号条件，则  
        U[i] = 1;  
        A[0] = 1;  
        D[0] = 6 * (Fi[0] - pLine->begin_k1) / H[0];  
        D[i] = 6 * (pLine->end_k1 - Fi[i-1]) / H[i-1];  
        //若边界条件为2号条件，则  
        //U[i] = 0;  
        //A[0] = 0;  
        //D[0] = 2 * pLine->begin_k2;  
        //D[i] = 2 * pLine->end_k2;  
    /////////////////////////////////////////追赶法求解M矩阵  
    B[0] = A[0] / 2;  
    for(i = 1;i <= pLine->point_num - 2;i++)  
    {  
        B[i] = A[i] / (2 - U[i] * B[i-1]);  
    }  
    Y[0] = D[0] / 2;  
    for(i = 1;i <= pLine->point_num - 1;i++)  
    {  
        Y[i] = (D[i] - U[i] * Y[i-1]) / (2 - U[i] * B[i-1]);  
    }  
    M[pLine->point_num - 1] = Y[pLine->point_num - 1];  
    for(i = pLine->point_num - 1;i > 0;i--)  
    {  
        M[i-1] = Y[i-1] - B[i-1] * M[i];  
    }  
    //////////////////////////////////////////计算方程组最终结果  
    for(i = 0;i <= pLine->point_num - 2;i++)  
    {  
        pLine->a3[i] = M[i] / (6 * H[i]);  
        pLine->a1[i] = (pLine->y[i] - M[i] * H[i] * H[i] / 6) / H[i];  
        pLine->b3[i] = M[i+1] / (6 * H[i]);  
        pLine->b1[i] = (pLine->y[i+1] - M[i+1] * H[i] * H[i] / 6) /H[i];  
        printf("i=%d,a3[i]=%f,a1[i]=%f,b3[i]=%f,b1[i]=%f\n",i,pLine->a3[i],pLine->a1[i],pLine->b3[i],pLine->b1[i]);
    }  
    
    return TRUE;  
}  

SPLINE line1;  
pSPLINE pLine1 = &line1;  

int main()  
{    
    line1.point_num = 10; 
    FILE *fp = fopen("testdata.txt","r"); 

    for(int i=0;i<line1.point_num;i++){
        fscanf(fp,"%f %f",&line1.x[i],&line1.y[i]);
        //printf("%f %f\n",line1.x[i],line1.y[i]);
    }

    line1.begin_k1 = 1;  
    line1.end_k1 = 0.1; 
    //line1.begin_k2 = 1;  
    //line1.end_k2 = 2;   
    Spline3(pLine1);  
    return 0;  
}  
