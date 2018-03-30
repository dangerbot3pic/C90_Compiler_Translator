int f()
{
    int y = 55;
    int *x = &y;

    while (--*x != 25)
    {}

    return y;
}