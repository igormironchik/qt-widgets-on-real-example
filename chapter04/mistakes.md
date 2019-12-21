# Mistakes handling

Who didn't do mistakes? Only that who didn't do anything. I showed this book on
[Reddit](https://www.reddit.com/) and got some comments. I want to discuss these
comments, and fix found problems.

## Possible blocking of GUI when resizing application

I created a thumbnail of the central frame's image in resize event handler. But resize events
can come very frequently, and do hard work here is not a good idea. To remind what were
done let's have a look at the code.

```
void
Frame::resizeEvent( QResizeEvent * e )
{
	if( d->m_mode == ResizeMode::FitToSize ||
		( d->m_mode == ResizeMode::FitToHeight && e->size().height() != d->m_thumbnail.height() ) )
			d->resized();

	e->accept();

	emit resized();
}
```

In `d->resized()` I created a thumbnail of the image to fit new size. I did actual work there.
But it's possible issue.

Solution is quite simple. Instead of actual resizing let's set special flag to true and in
paint event will check if this flag is set then will do resize before drawing. And I emited
signal `resized()` in resize event. This signal is a subject of interest for crop widget,
so I will emit this signal only after real resizing that will be done in paint event.

This approach will offload the application.

## Issue with resizing of crop widget corresponding to resize of the window.

In the application exists feature: when you are in crop mode and resize entire window
of the application crop widget resizes corresponding to new size of the image, that fit window
size. I had a bug here. I used `QRect` to store crop area and calculated new crop area with
`int`. Casting of `qreal` to `int` brought to the issue. I changed `QRect` to `QRectF` and
wrapped calculated values with `qRound()` where were necessary to cast from `qreal` to
`int`.

## Using of QStringLiteral

I used plain C++ string literals for resources paths, etc... It's good practice to
use `QStringLiteral` instead of it. Have a look at the Qt's documentation about
`QStringLiteral`.

## Private implementation

As you can saw I use PIMPL idiom in the code. I think that this is a holly war - use or no PIMPL in
such small applications. I used and my code is clean, look at the header files, they are simple and
clear. I hided details of implementation, I reduced recompilation time on changes. Why not?
This is a choice of you, use or not use PIMPL in your projects.

## Inheritance vs composition

In the comments I got that I use inheritance where possible to use just a composition.
In the `Frame` class, for example. Well, yes it's possible to use composition in this
case, it's possible to handle resizes with event filter, yes, it's doable. But how
many times I caught myself on the thought that I was lazy, implemented something with
composition, all worked, code was just a few lines. But at some point of time I need
to implement something yet, and voila, I understand that with composition it's
impossible to do a customer request. And what? I rewrite the code with inheritance,
add new class, rewrite functionality with inheritance, add new featrues. This is time,
this is possible issues. And how would be simpler if at the beginning I did it
with inheritance.

So, it's my choice to use inheritance even in such simple things. This is very simple
to change something, add new fetaures. I was not lazy to implement `Frame` with
inheritance, and at any time I can do with `Frame` what I need.

This is the same question of using `*.ui` files with Qt. It's fast, chop-chop and ready
form, great. But be very attentive with using `*.ui`. At some time you probably
will need to add something, relayout the form, and what if your layouts have
custom margins, for example, set in the `*.ui` file. Ops, they broken, you forgot about
it, and customer will say to you: "You broke my application!" Nice, really.

This is not simple question use inheritance or composition. Think twice if you are
going to use composition or `*.ui` file. Using of inheritance is not a mistake, but can
save your time and nerves in the future, and eat time at the beginning. So it's my choice...

[Back](../chapter03/impl.md) | [Contents](../README.md) | [Next](../links.md)
