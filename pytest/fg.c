int func(){
  int x = 5;
  int y = 2;
  if(x<y){
    x=x+1;
    if(x==y){
      return y;
    }
    return x*y;
  }
  return x+y;
}

int main()
{
return func();
} 