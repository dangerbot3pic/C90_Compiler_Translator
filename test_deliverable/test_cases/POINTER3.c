int f()
{
    int y = 20;
    int *x = &y;
    *x += 5;
    return y;
}