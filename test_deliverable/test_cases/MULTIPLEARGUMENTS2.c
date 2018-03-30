int m(int x, int y, int z, int a)
{
    int d = x + y + z - a;
    return d;
}

int f()
{
    int x = 15;
    int y = 15;
    int a = 15;
    int d = 20;
    return m(x, y, a, d);
}