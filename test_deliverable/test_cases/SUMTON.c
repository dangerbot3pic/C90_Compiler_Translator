int sumToN(int n)
{
    if (n != 0)
    {
        return n + sumToN(n - 1);
    }
    return 0;
}

int f()
{
    return sumToN(7) - 3; 
}