int f()
{
    int y = 20;
    int *x = &y;
    *x = *x + 5;
    return *x;
}