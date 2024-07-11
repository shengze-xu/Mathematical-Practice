function M = sort1spline(x,y,s0,sn)
n=length(x)-1;
M=zeros(1,n+1);m=zeros(n+1,1);
h=zeros(1,n);
a=zeros(1,n); b=zeros(1,n);
A=zeros(n+1,n+1);B=zeros(n+1,1);

for i=1:n
    h(i)=x(i+1)-x(i);
    if i==1
        a(i)=0;
        b(i)=0;
    else
        a(i)=1.0*h(i-1)/(h(i-1)+h(i));
        b(i)=3*1.0*((1-a(i))/h(i-1)*(y(i)-y(i-1))+a(i)*1.0/h(i)*(y(i+1)-y(i)));
    end
end

A(1,1)=2;A(1,2)=1;B(1,1)=3/h(1)*(y(2)-y(1))-h(1)/2*s0;%方程组中第一个式子
for i=2:n%方程组中第2-n个式子
    A(i,i-1)=1-a(i);A(i,i)=2;A(i,i+1)=a(i);
    B(i,1)=b(i);
end
A(n+1,n)=1;A(n+1,n+1)=2;B(n+1,1)=3/h(n)*(y(n+1)-y(n))+h(n)/2*sn;%方程组中最后一个式子

m=A\B;M=m';

