% use natural cubic spline to interpolate data point
   % a、(0,3),(1,5),(2,4),(3,1)
   % b、(-1,3),(0,5),(3,1),(4,1),(5,1)
function page_178_1_natural_spline_script
ax = [0 1 2 3];
ay = [3 5 4 1];  %对数据点(0,3),(1,5),(2,4),(3,1)进行三次样条建模，并输出表达式和图像
bx = [-1 0 3 4 5];
by = [3 5 1 1 1];%对数据点(-1,3),(0,5),(3,1),(4,1),(5,1)进行三次样条建模，并输出表达式和图像
csa = spline(ax,[0 ay 0]);
xxa = linspace(0,3,1000);
subplot(1,2,1);
plot(ax,ay,'o',xxa,ppval(csa,xxa),'-');
xlabel('a x 0~3');
ylabel('a y');
title('equation a');
csb = spline(bx,[0 by 0]);
xxb = linspace(-1,5,10000);
subplot(1,2,2);
plot(bx,by,'o',xxb,ppval(csb,xxb),'-');
xlabel('b x -1~5');
ylabel('b y');
title('equation b');

pp=interp1(ax,ay,'spline','pp');
breaks=pp.breaks; %breaks的第i和i+1个元素为m和n
coefs=pp.coefs; %假设coefs的第i行为a b c d，
     %breaks的第i和i+1个元素为m和n那么在区间[m,n]的函数表达式就是
     %a(x-m)^3+b(x-m)^2+c(x-m)+d
syms x
disp('对于数据点(0,3),(1,5),(2,4),(3,1)的三次样条表达式为：');
for i = 1:3
    y = coefs(i,1)*((x - breaks(i))^3) + coefs(i,2)*((x - breaks(i))^2) + coefs(i,3)*((x - breaks

    (i))) + coefs(i,4)
end
ppb=interp1(bx,by,'spline','pp');
breaksb=ppb.breaks; %breaks的第i和i+1个元素为m和n
coefsb=ppb.coefs; %假设coefs的第i行为a b c d，
%breaks的第i和i+1个元素为m和n那么在区间[m,n]的函数表达式就是
%a(x-m)^3+b(x-m)^2+c(x-m)+d
syms x
disp('对于数据点(-1,3),(0,5),(3,1),(4,1),(5,1)的三次样条表达式为：');
for i = 1:4
   y = coefsb(i,1)*((x - breaksb(i))^3) + coefsb(i,2)*((x - breaksb(i))^2) + coefsb(i,3)*((x - 

   breaksb(i))) + coefsb(i,4)
end
