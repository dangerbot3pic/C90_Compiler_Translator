int m(int x, int y, int z, int a, int fail)
{
    int d = x + y + z - a + fail;
    return d;
}

int f()
{
    int x = 15;
    int y = 15;
    int a = 15;
    int d = 20;
    int fail = 0;
    return m(x, y, a, d, fail);
}