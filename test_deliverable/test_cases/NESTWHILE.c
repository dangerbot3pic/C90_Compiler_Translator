int f()
{
int x = 0;
int y = 0;
int z = 0;
while(x<5){
    while(y<5){
      z = z + 1;
      y = y + 1;
    }
    y = 0;
    x = x + 1;
}
return z;
}
