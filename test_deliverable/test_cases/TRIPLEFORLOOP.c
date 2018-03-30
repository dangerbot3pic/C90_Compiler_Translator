int f()
{
    int v = 0;
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            for (int k = 0; k < 5; k++)
            {
                v++;
            }
        }
    }
    return v - 100;
}