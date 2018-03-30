def func():
	a = 4 
	while (a<4):
		a += 1
	return a
    
def main():
    return func()

# Boilerplate
if __name__ == "__main__":
    import sys
    ret=main()
    sys.exit(ret)

