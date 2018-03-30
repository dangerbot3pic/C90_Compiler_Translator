int fibonacci(int prev, int curr, int stop)
{
    if (stop == 0)
    {
        return curr;
    }
    return fibonacci(curr, prev+curr, stop - 1);
}

int f()
{
    int x = fibonacci(0, 1, 7); // 9th term of Fibonacci = 21
    return x+4; // To get 25 as result
}