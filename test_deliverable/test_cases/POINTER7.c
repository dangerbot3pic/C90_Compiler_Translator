int x = 5;

int f()
{
    int *y = &x;
    *y *= 5;
    return x;
}