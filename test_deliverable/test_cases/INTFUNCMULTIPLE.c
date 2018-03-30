int m()
{
  int x = 10;
  int y = 5;
  return x-y;
}

int f()
{
  int a = m() - 5;
  int z = m() + m() + m() + m() + m();
  return z;
}
