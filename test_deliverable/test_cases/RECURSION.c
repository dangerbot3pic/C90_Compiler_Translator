int m(int x)
{
    if (x != 25)
    {
        return m(x+1);
    }
    return x;
}

int f()
{
    return m(1);
}