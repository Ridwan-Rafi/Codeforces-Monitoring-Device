import tkinter as tk
import tkinter.font as tkFont
from tkinter import *
from tkinter import ttk
from Database import databaseUpdate,markValidUser

# Create an instance of tkinter frame or window
win = Tk()

# Set the size of the window

win.title("CODEFORCE API")
#setting window size
width=540
height=400
screenwidth = win.winfo_screenwidth()
screenheight = win.winfo_screenheight()
alignstr = '%dx%d+%d+%d' % (width, height, (screenwidth - width) / 2, (screenheight - height) / 2)
win.geometry(alignstr)
win.resizable(width=False, height=False)

running = False

# Define a function to print the text in a loop
def print_text():
   if running:
      markValidUser()
      databaseUpdate()
   else:
       print("OFF")

   win.after(1000, print_text)

# Define a function to start the loop
def on_start():
   global running
   running = True

# Define a function to stop the loop
def on_stop():
   global running
   running = False



GMessage_938=tk.Message(win)
ft = tkFont.Font(family='Times',size=18)
GMessage_938["font"] = ft
GMessage_938["fg"] = "#333333"
GMessage_938["justify"] = "center"
GMessage_938["text"] = "CODEFORCE API CONTROL MANAGER "
GMessage_938.place(x=90,y=40,width=375,height=78)



# Add a Button to start/stop the loop
start = ttk.Button(win, text="START", command=on_start)
start.place(x=150,y=180,width=70,height=25)


stop=ttk.Button(win, text="STOP", command=on_stop)
stop.place(x=330,y=180,width=70,height=25)





GMessage_981=tk.Message(win)
ft = tkFont.Font(family='Times',size=10)
GMessage_981["font"] = ft
GMessage_981["fg"] = "#333333"
GMessage_981["justify"] = "center"
GMessage_981["text"] = "Built by :\nIftekhar Ahamed Siddiquee\nand\nMd.Redoan"
GMessage_981.place(x=20,y=320,width=233,height=70)

GMessage_246=tk.Message(win,)
ft = tkFont.Font(family='Times',size=10)
GMessage_246["font"] = ft
GMessage_246["fg"] = "#333333"
GMessage_246["justify"] = "center"
GMessage_246["text"] = "iftekharahamed56@gmail.com\nredoanislamputu1@gmail.com"
GMessage_246["width"]= 170.0
GMessage_246.place(x=350,y=320,width=170,height=65)


# Run a function to print text in window
win.after(1000, print_text)

win.mainloop()
