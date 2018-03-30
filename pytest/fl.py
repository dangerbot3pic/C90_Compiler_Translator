
def func():
	x = 4
	for i in range(10, -1, -1):
		x += 1
	return x;
    
def main():
    return func()

# Boilerplate
if __name__ == "__main__":
    import sys
    ret=main()
    sys.exit(ret)

