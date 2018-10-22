# Plans

How do you see UI, the main UI, of the editor? I want to have horizontal scrollable tape with frames
of GIF at bottom, frames should be checkable with check box, so user will be able to delete some frames
from the GIF. And resized to fit available space current frame in the center of main window. Frames
should be clickable, so user can select any frame. And for first Alpha version of the application
I want to implement crop function. Crop will be accessible from tool bar, and in this mode user
should be able to draw rectangle to crop to, and on pressing Enter crop should do the job for all
frames.

That's all. Sounds not so complicated, let's do it. And let's start from creating a widget
that will represent a frame in the tape of frames.

I wrote the code for the next five chapters before continuing writing this book, because it's
very difficult to keep in mind all possible issues that could be during coding. So I wrote
the code, debugged it, checked, and started to write overview of my adventure. After reading
next five chapters the editor will can open GIF images, user will see the tape with clickable
frames, and on clicking in the center of window will be displayed selected frame. Believe me, it looks
nice, and I spend only half a day for developing it, that is why I love Qt so much.

[Back](launching.md) | [Contents](README.md) | [Next](frame.md)