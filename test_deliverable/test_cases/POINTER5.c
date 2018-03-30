int f()
{
    int y = 24;
    int *x = &y;
    int z = ++ *x;
    return z;
}