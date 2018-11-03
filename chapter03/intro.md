# Chapter 3

As you can saw operations like open, save and crop in GIF editor, described in
chapter 1, can longs very much. And during these processes UI is frozen. This is sad.
Why not to add some busy animation during these operations? Good! But animations should
work in main thread. Well we can dilute code of operations with QApplications::processEvents(),
and move Magick++ operations in separate thread. Amazing, let's do it.

Want to add that multithreading, especially in GUI, is not a panacea. I saw in my
practice that very much amount of threads in the application can only slow down the
performance, and very much. But approach described above sounds very good. Our
application will be very responsive.

In Qt there are a lot of mechanisms of multithreading, like QThread, QThreadPool,
QRunnable, QtConcurrent, queued connections of signals and slots. So let's look
at the implementation.

[Back](../chapter02/capture.md) | [Contents](../README.md) | [Next](impl.md)