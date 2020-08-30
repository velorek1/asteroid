#!/bin/bash
# file: build.sh
echo -e "\e[93mAst3r0id Game - Coded By v3l0r3k\e[0m"
echo
read -p "[+] Install SDL2 libraries? (sudo required) [Y/N]: " -n 1 -r
echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
then
 sudo-apt get install libsdl2-2.0-0 
 sudo apt-get install libsdl2-dev
 sudo-apt get install libsdl2-mixer-2.0-0 
 sudo apt-get install libsdl2-mixer-dev
 sudo-apt get install libsdl2-image-2.0-0 
 sudo apt-get install libsdl2-image-dev
fi
echo
echo [+] Remember to install [SDL2] libraries: 
echo _____________________________________________________________________________________
echo sudo-apt get install libsdl2-2.0-0 
echo sudo apt-get install libsdl2-dev
echo sudo-apt get install libsdl2-mixer-2.0-0 
echo sudo apt-get install libsdl2-mixer-dev
echo sudo-apt get install libsdl2-image-2.0-0 
echo sudo apt-get install libsdl2-image-dev
echo _____________________________________________________________________________________
echo
echo [+] Attempting to compile game...
gcc main.c -lSDL2 -lSDL2_image -lSDL2_mixer -o asteroid
echo gcc main.c -lSDL2 -lSDL2_image -lSDL2_mixer -o asteroid
echo
echo -e "Run as \e[93m./asteroid\e[0m" 
echo Enjoy it!

