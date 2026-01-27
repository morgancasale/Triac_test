def res(i, j):
    return 1 if i >= (12+2*j-9*(-1)**j)/4 else 0


for i in range(0, 11):
    for j in range(0, 10):
        print(f"{res(i,j)}, ", end=" ")
    print()
