# ctypes_python_numpy_example
example of using ctypes in python to call a C++ function to edit a 2D numpy array

Rough descriptions of how to call a C++ function through ctypes in Python on Windows. Calling a C++ function on a 2D numpy array took me about 2 hours to figure out, even with the entire internet as a reference, so I'm writing this up. You're welcome.

How to use this: Set up your system so you can get main.py to work.  
1: Build C++ DLL's. I used `cl /LD /EHsc "cpp_dither.cpp"`; that requires setting up the Windows environment correctly with a vcvars.bat first. The changes from `vcvars` aren't permanent, so you have to re-run it each time you use a new command window.  
2: In Python, get ctypes to call a C++ function on a 2D numpy array. That's the hard part.

## Prep:

•C++ file needs to wrap function declarations in an extern "C" {} block.  

•Each function declaration must be DLL exported: "__declspec(dllexport)" goes before each declaration, or else define a nicer-looking function at the top for dllexport and use that instead.

(See the .h file for an example of how to do those.)

I wanted to pass a numpy array of float64 (intended to be C++ doubles). Originally, I tried using vector<vector<double>>'s in the C++ code because vector vector double was slightly easier for me to write. That didn't seem to work. Instead, I got the ctypes byref "pass by reference" to work with double** pointers in the C++ functions. The pointers themselves were not passed by reference; using `double** &pixels` in the C++ code would cause the ctypes functions to fail (I think...). Additionally, dimension parameters "rows" and "cols" had to be passed into the function because pointer arrays don't have a function to get their size like vectors don't.

bad: `double get2DArrayMean(vector<vector<double>> pixels)`  
good: `double get2DArrayMean(double** pixels, int rows, int cols)`

### Building the .dll's

On my Windows, I ran the Visual Studio 2017 vcvars64.bat file, which sets the environment so cl.exe works and does the right thing. Then, I navigated to the folder with the .cpp file to build. I then called `cl /LD /EHsc "cpp_dither.cpp"`, which builds .dll, .lib, and .exp files.  
(To save time, in practice I just run a command that calls the vcvars64.bat inside folder with the .cpp file, so I don't have to navigate around a lot. There are also ways to make permanent the environment changes done through the vcvars batch scripts. Or you could use Visual Studio.)

Note that the cl.exe (C++ compiler) must build to the same operating system type as your Python version: x86 for x86 (32-bit), x64 for x64 (64-bit). You can see which version of cl.exe is running by typing "cl", and it will say "for x86" or "for x64" at the end. I hope you know your Python version, because I forgot how to check that but it's pretty easy.

I did encounter a version mismatch when trying to do this: I was using x86 cl.exe to build for x64 Python. The error I got was `OSError: [WinError 193] %1 is not a valid Win32 application`.

### Then, in the Python file...

```python
from ctypes import WinDLL

#load DLL
mylib = WinDLL("./cpp_dither.dll")

#load image stuff
import cv2
import numpy as np
myimage = cv2.imread("starry.jpg")
pixels = myimage[:,:,1].copy() * (1/255.) #get just one channel and convert to float64

import ctypes
from ctypes import POINTER, c_double
def ctypes_ditherVarcoBlue(myimage_orig): #for a one-channel image, a 2D numpy array
    myimage = myimage_orig.copy()
	#Create pointers.
    myimage_pointers = [np.ctypeslib.as_ctypes(x) for x in myimage]
    myimage_pointers = (POINTER(c_double) * myimage.shape[0])(*myimage_pointers)
	#Call C++ function.
    mylib.dither_VarcoBlue(myimage_pointers, ctypes.c_int(myimage.shape[0]), ctypes.c_int(myimage.shape[1]), ctypes.c_double(2), ctypes.c_bool(False))
	#Calling this function on the pointers edits the array in-place. Success!
    return myimage

myimage_dithered = ctypes_ditherVarcoBlue(pixels)
#convert and write image.
myimage_dithered_int = np.zeros(myimage_dithered.shape)
myimage_dithered_int = cv2.normalize(myimage_dithered, myimage_dithered_int, 0, 255, cv2.NORM_MINMAX).astype("uint8")
cv2.imwrite("testimage_out_channel_1_dithered.png", myimage_dithered_int) #did it work?
```

This was the definition header for "dither_VarcoBlue" in C++:

```c++
void dither_VarcoBlue(double** pixels, int rows, int cols, double numcolors, bool serpentine)
```

It takes a pointer array "pixels" and some other parameters, and it returns nothing. The pointer array "pixels" is edited in-place.

This "pass by reference" behavior was difficult to replicate in Python. The above Python function does work. For example, when trying similar castings without the list comprehension `[f(x) for x in alist]`, I would often get `OSError: exception: access violation reading 0xFFFFFFFFFFFFFFFF`. That's annoying because you'd think a 500x500 array of pointer arrays would be the same thing regardless of how it was made, but apparently it isn't.

Here's another common error messages when working with ctypes:  
`ctypes.ArgumentError: argument 1: <class 'TypeError'>: Don't know how to convert parameter 1`
If you get that one, try casting to a ctypes type.

Note that there are differences in interfacing between C and C++, and C# is something else entirely.
