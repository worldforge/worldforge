#Installs a GDB hook that removed the mouse locking feature of SDL whenever we enter into the debugger.
#This is needed because Ember grabs the mouse, and not having access to the mouse makes debugging hard.
#To make sure this runs you need to edit your ~/.config/gdb/gdbinit file and add these two lines to it
#set auto-load local-gdbinit on
#add-auto-load-safe-path /

python
def release_mouse (event):
    gdb.write("GDB/SDL2: Releasing mouse\n")
    try:
        gdb.execute("call SDL_SetRelativeMouseMode(0)")
    except gdb.error:
        pass


gdb.events.stop.connect(release_mouse)
gdb.write("GDB/SDL2: installed release mouse for SDL2\n")

end
