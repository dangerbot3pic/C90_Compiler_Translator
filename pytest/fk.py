x = 0

def f():
	global x
	x += 1


def func():
	global x
	x = 5
	f()
	return x
    
def main():
    return func()

# Boilerplate
if __name__ == "__main__":
    import sys
    ret=main()
    sys.exit(ret)

