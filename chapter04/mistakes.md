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

[Back](../chapter03/impl.md) | [Contents](../README.md) | [Next](../links.md)
