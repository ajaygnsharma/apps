'''
Created on Mar 20, 2019

@author: user1
'''
from tkinter import *
from tkinter.messagebox import showinfo
from atk import Window

def reply():
    showinfo(title='popup',message='Button pressed!')

def gui():
    window = Tk()
    button = Button(window, text='press', command=reply)
    button.pack()
    window.mainloop()

if __name__ == '__main__':
    gui()
    pass