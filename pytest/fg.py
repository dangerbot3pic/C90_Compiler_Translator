def func():
	x = 5
	y = 2
	if x < y:
		x += 1
		if x == y:
			return y
		return x*y
	return x + y
			

    
def main():
    return func()

# Boilerplate
if __name__ == "__main__":
    import sys
    ret=main()
    sys.exit(ret)

