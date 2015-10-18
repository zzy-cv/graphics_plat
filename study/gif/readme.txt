直接windows函数版本
cl main_win32.c draw.c /link kernel32.lib user32.lib gdi32.lib

SDL版本
cl /MD -o main.exe main.c draw.c -IE:\0mylib\include /link E:\0mylib\SDL.lib E:\0mylib\SDLmain.lib winmm.lib gdi32.lib kernel32.lib user32.lib dxguid.lib advapi32.lib  /subsystem:console