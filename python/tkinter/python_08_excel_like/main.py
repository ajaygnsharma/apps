try:
    from tkinter import *
except ImportError:
    from Tkinter import *

root = Tk()

height = 5
width = 5
for i in range(height): #Rows
    for j in range(width): #Columns
        b = Entry(root, text="hi")
        b.grid(row=i, column=j)

mainloop();