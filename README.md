# Sudoku AR
**Authors:** [Pablo R. Palafox](https://pablorpalafox.github.io/), Hosein Mirzai, Yoon He Ha and Gonzalo Munilla

<p align="center">
  <img src="/assets/images/sudoku.gif" alt="sudokuAR">
</p>

Augmented Reality application based on OpenCV and OpenGL to solve Sudokus with a hand-held camera in real-time.


## Prerequisites
We have tested the system in **Windows 10**, and implemented the solution in **Visual Studio 2017**.

### OpenCV
We use [OpenCV](http://opencv.org) to manipulate images and features. Dowload and install instructions can be found at: http://opencv.org. **Required at leat 3.4.1. Tested with OpenCV 3.4.1**.

### OpenGL
We use [OpenGL](http://opencv.org) to augment video feed with bouncing balls when a solution for the sudoku is found.

## Usage
Clone the project into a local folder in your machine and open the _.sln_ file with Visual Studio. If you have OpenCV and OpenGL set up properly for Visual Studio, then you should be able to compile the code.

You can use either your webcam or an external USB camera. Connect it to your machine and then run the project in Visual Studio. You should get 3 windows. Left-most is input image with overlaid detection of the sudoku grid; middle window shows the warped grid; right-most window should eventually show the original sudoku grid plus the solution, which augments the input video. Some bounding balls will also start bouncing on every corner of the sudoku grid once a solution is computed.

So... get an unsolved sudoku, put it in front of the camera, and when all (81) grids of the sudoku are detected (green boxes should be assigned to each grid), then press the 'S' key and, hopefully, a solution will be displayed on the middle and right most windows.

## Known issues
- The 'S' key event is handled throught OpenCV. So before pressing the 'S' key to solve the visualized sudoku, you must have clicked on the left-most (or middle) windows that appear once you run the code.

- Since we rely on an external python script to run a CNN for digit recognition, after launching the code and pressing 'S' on the keyboard for the first time in the current run, it will take some time to load tensorflow and then run the network for inference.

- After detecing and warping the sudoku (the warped sudoku isdisplayed in the middle window), and once the user presses 'S' on the keybord, we crop the warped image into 81 subimages and preprocess them before sending them to the digit recognition network. By preprocessing we mean removing the contours. This preprocessing step does not always produce good enough images of the digits (e.g., we might crop the subimage of the digit too much), so the digit recognition network might have some trouble recognizing the digits. This will cause the system to not be able to solve the sudoku on the first try.

## License

Sudoku AR is released under a MIT license.
