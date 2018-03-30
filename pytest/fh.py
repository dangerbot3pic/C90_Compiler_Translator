def func():
    x = 5
    x -= 1
    return x
    
def main():
    return func()

# Boilerplate
if __name__ == "__main__":
    import sys
    ret=main()
    sys.exit(ret)

