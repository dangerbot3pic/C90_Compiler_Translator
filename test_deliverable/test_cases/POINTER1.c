int f()
{
    int y = 5;
    int x = 20;
    int *m = &x;
    y = *m + y;
    return y;
}