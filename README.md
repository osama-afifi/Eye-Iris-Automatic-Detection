Eye-Iris-Automatic-Detection
============================

C++ Eye Iris Automatic Detection based on entropy &amp; Iris color score using openCV

An Automatic Eye Iris Detection Method 
Main phases: 

1- Image Preprocessing
2- Selecting Window sizes 
3- Entropy Score 
4 - Iris Darkness Score 
5- Hypothesis Calculation
6- Testing results on a dataset


1- Image Preprocessing  
Converting images into grayscale and passing them into Viola & Jones face detector which uses haar-like features so we can focus on our area of interest (faces).

2- Selecting Window sizes:  
We set up windows to scan the image, where the iris radius r is approx third the face width.
w = 2r + deltaX , h = 2r + deltaY.
 where deltaX ,deltaY are arbitrary constants added to our random windows.
we select the best k windows in terms of entropy score.

3- Entropy Score
Entropy is a good measure for uncertainty it's continuous, a strictly convex function, which reaches a maximum value when all probabilities are equal, and maximized in a uniform probability distribution context.
Shannon introduced an important concept which is the entropy, in the form H(S)= -∑_(i=1)^n▒〖p(xi)〗  log_2⁡〖p(xi)〗
H score = (Entropy(wi))/(∑_(i=1)^n▒〖Entropy(wi)〗) , where wi is the ith window

4 - Iris Darkness Score    
we sum up the pixel values within the range of radius r from the eye center to calculate our iris darkness score
 C score = 1-  (Darkness(wi))/(∑_(i=1)^n▒〖Darkness(wi)〗)
 
5- Hypothesis Calculation  
Our Hypothesis is based on the summation of the two scores
T score = H score + C score
and we will select the center of the highest scored window as our our iris center.
 
6- Testing results on a dataset  
Our Automatic Iris detector accuracy will be tested using a labeled dataset


