int m()
{
    int x = 5;
    int y = 20;
    return y - x;
}

int f()
{
    int a = m() - 5;
    int z = a + m();
    return z;
}