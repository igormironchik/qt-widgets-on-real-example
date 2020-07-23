# Chapter 5

In this small book I wanted to show Qt Widgets usage on the real examples. My idea was to show how it's
nice to write GUI applications with Qt, with widgets. But Qt it's not only C++ instrument. I believe that
you heard about QML. QML is very good in some cases. If you want the same feel and look on absolutelly all
platforms, QML is for you. But it's not the only one reason to use QML. Maybe in your application a lot of
animations? QML can help you a lot with it.

Sometimes it's very difficult question what technology to use, QML or widgets. I suggest to use widgets
for desktop applications, and QML for mobile platforms. Widgets are very stable in API, they are pure C++,
they effective. QML is very dynamically developing, it's not so stable as widgets, from time to time
the same QML code on Qt updates can break, yes, you should keep in mind it when using QML. Sometimes
you can think that QML is ambiguous, especially in placing elements, but these are working questions.
QML is good, as I already said, for mobile platforms, or if you want the same UX for both desktop
and mobile.

I'm not an expert in QML, Look at this chapter like on my own opinion and expirience. QML can save a lot of development
time. Only bindings of properties in QML can help a lot. Sometimes I write QML code and understand
how it's simple to use this technology. Really. But sometimes I'm in struggle with QML. Possibly this is
because I don't have a lot of practice with QML, possibly. But I saw that the same QML code worked on previous
version of Qt, and something was broken with update, and I was in need to look for a workaround.

But I believe that QML will become more stable and effective instrument in the near future. This is a
part of Qt and we need to keep hands on pulse. So let have a look at one small, known to everybody
game - chess, written with QML and C++. The source code of this example is placed on GitHub
[https://github.com/igormironchik/chess](https://github.com/igormironchik/chess)

[Back](../chapter04/mistakes.md) | [Contents](../README.md) | [Next](c++-qml.md)
