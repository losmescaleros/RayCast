Mitchell Neville
CSC 561
Program 1 - Raycasting

Pg1_raycast
=======

This is a Visual Studio C++ application which implements raycasting. 

# Running the Program

The main portion of the program is found in 'RayCast.cpp'. This is where a single OBJ file is specified and read, and it is where any window or light settings are read. In order change 
the OBJ file read, please refer to the '_tWinMain' function within RayCast.cpp. Within this function, you will see a commented
section of code which describes the file read into the program. All OBJ input files are assumed to be in a folder called 'inputs'.
If you wish to load an OBJ file, please make sure that it is in this 'inputs' folder.

## Window Settings and Light Sources

Please refer to the extra credit file for information about providing custom window settings and multiple light sources. 

# Limitations

* The program reads in OBJ files, but it assumes that these only consist of triangle definintions. That means that any shapes with
more than three coordinates cannot be interpreted correctly, and program behavior with such a file is unknown. 

* There is currently no way to provide an attenuation factor for lights specified in the scene, and all lights within the scene
are not attenuated. 

# 3rd Party Resources

## tinyobjloader

The [tinyobjloader] (https://github.com/syoyo/tinyobjloader) project was used and modified slightly in this project for reading
in OBJ files. The modifications included the ability to read 'group' definitions in OBJ files. 
