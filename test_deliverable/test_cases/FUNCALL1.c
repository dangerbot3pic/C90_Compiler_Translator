int j(int a, int b)
{
    return b - a;
}

int m(int x)
{
    return j(10, 30) + x;
}

int f()
{
    int x = 5;
    return m(x);
}