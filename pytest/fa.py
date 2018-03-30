def func():
	x = 0
	y = 0
	x = 5
	y = 5
	if (x == y):
		return 5
	else:
		return 4
    
def main():
    return func()

# Boilerplate
if __name__ == "__main__":
    import sys
    ret=main()
    sys.exit(ret)

