# Chapter 2

In this chapter I will show how to work with QCamera, QAbstractVideoSurface.
How to detect motion with OpenCV, capture with QCamera frames and store them
in some place on disk. This chapter is based on the real project SecurityCam
that places on GitHub [https://github.com/igormironchik/security-cam](https://github.com/igormironchik/security-cam)

In this chapter I will describe only most interesting parts of the code of
SecurityCam, I won't show you how I save configuration file, how I
organized configuration of application. Only a few words, that for
reading/saving configuration file I used cfgfile. This is Open Source
library for reading and saving configurations. It places on GitHub
[https://github.com/igormironchik/cfgfile](https://github.com/igormironchik/cfgfile)

SecurityCam is an application that connects to USB camera and tries to
detect motions in the frame, as soon as motion is detected camera starts
to capure images and store them in the configured folder with
yyy/MM/dd hierarchy. SecurityCam can do clean at the configured time
and delete folders with images that stored more than N days.

Window of the application displays stream from the camera and on closing
minimizes to tray, so the application works in the background and
protects the entrusted territory.

[Back](../chapter01/about.md) | [Contents](../README.md) | [Next](view.md)