int f()
{
int x = 0;
int y = 0;
int z = 0;
do{
    do{
      z = z + 1;
      y = y + 1;
    }while(y<5);
    y = 0;
    x = x + 1;
}while(x<5);
return z;
}
