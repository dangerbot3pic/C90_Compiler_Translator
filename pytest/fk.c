int x;

void f(){
  x = x+1;
}

int func(){
  x=5;
  f();
  return x;
}

int main()
{
	return func();
} 