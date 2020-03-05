# Frame

## Class

Frame... This is an unit of GIF image. We need a thumbnail frame on the tape to display
a sequence of frames in GIF, and a bigger one to display currently selected frame. The task
of the frame is to display image, so why not to have one class for both cases? Hm, why not?
But for the current frame we need the image to be scaled to the size of the available area with keeping
aspect ratio, whereas for the frame on the tape we need an image scaled to the height of the tape.
The image on the frame should automatically resize on parent resizing, and it should be
clickable. I guess that this is enough for our application. Possibly we will need something
additional in the future, possibly, but for the first attempt this is all that we need.
Let declare a class of our frame.

```cpp
#ifndef GIF_EDITOR_FRAME_HPP_INCLUDED
#define GIF_EDITOR_FRAME_HPP_INCLUDED

// Qt include.
#include <QWidget>
#include <QScopedPointer>


//
// Frame
//

class FramePrivate;

//! This is just an image with frame that fit the given size or height.
class Frame final
	:	public QWidget
{
	Q_OBJECT

signals:
	//! Clicked.
	void clicked();

public:
	//! Resize mode.
	enum class ResizeMode {
		//! Fit to size.
		FitToSize,
		//! Fit to height.
		FitToHeight
	}; // enum class ResizeMode

	Frame( const QImage & img, ResizeMode mode, QWidget * parent = nullptr );
	~Frame() noexcept override;

	//! \return Image.
	const QImage & image() const;
	//! Set image.
	void setImage( const QImage & img );

	QSize sizeHint() const override;

protected:
	void paintEvent( QPaintEvent * ) override;
	void resizeEvent( QResizeEvent * e ) override;
	void mouseReleaseEvent( QMouseEvent * e ) override;

private:
	Q_DISABLE_COPY( Frame )

	QScopedPointer< FramePrivate > d;
}; // class Frame

#endif // GIF_EDITOR_FRAME_HPP_INCLUDED
```

## Implementation

With private data class all is simple, it's better to see one time than hear thousand times.

```cpp
class FramePrivate {
public:
	FramePrivate( const QImage & img, Frame::ResizeMode mode,
		Frame * parent )
		:	m_image( img )
		,	m_mode( mode )
		,	q( parent )
	{
	}

	//! Create thumbnail.
	void createThumbnail();
	//! Frame widget was resized.
	void resized();

	//! Image.
	QImage m_image;
	//! Thumbnail.
	QImage m_thumbnail;
	//! Resize mode.
	Frame::ResizeMode m_mode;
	//! Parent.
	Frame * q;
}; // class FramePrivate
```

I declared two methods to create a thumbnail of needed size and auxiliary method to do some stuff
when widget will be resized, as creating a thumbnail, notifying layouts about size change and
updating our frame.

The creation of a thumbnail is different for different resize modes of a frame. For fit to size mode
we need to scale in both directions keeping the aspect ratio of the image, whereas for the fit to height
mode we just need to scale to height keeping aspect ratio too. Let's have a look.

```cpp
void
FramePrivate::createThumbnail()
{
	if( m_image.width() > q->width() || m_image.height() > q->height() )
	{
		switch( m_mode )
		{
			case Frame::ResizeMode::FitToSize :
				m_thumbnail = m_image.scaled( q->width(), q->height(),
					Qt::KeepAspectRatio, Qt::SmoothTransformation );
			break;

			case Frame::ResizeMode::FitToHeight :
				m_thumbnail = m_image.scaledToHeight( q->height(),
					Qt::SmoothTransformation );
			break;
		}
	}
	else
		m_thumbnail = m_image;
}

void
FramePrivate::resized()
{
	createThumbnail();

	q->updateGeometry();

	q->update();
}
```

Some methods' implementations of Frame class are quite simple and don't need an explanation.

```cpp
Frame::Frame( const QImage & img, ResizeMode mode, QWidget * parent )
	:	QWidget( parent )
	,	d( new FramePrivate( img, mode, this ) )
{
	switch( mode )
	{
		case ResizeMode::FitToSize :
			setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
		break;

		case ResizeMode::FitToHeight :
			setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding );
		break;
	}
}

Frame::~Frame() noexcept
{
}

const QImage &
Frame::image() const
{
	return d->m_image;
}

void
Frame::setImage( const QImage & img )
{
	d->m_image = img;

	d->resized();
}

QSize
Frame::sizeHint() const
{
	return d->m_thumbnail.size();
}
```

### Events

Painting needs just to draw a thumbnail in the center of the widget.

```cpp
void
Frame::paintEvent( QPaintEvent * )
{
	const int x = ( width() - d->m_thumbnail.width() ) / 2;
	const int y = ( height() - d->m_thumbnail.height() ) / 2;

	QPainter p( this );
	QRect r = d->m_thumbnail.rect();
	r.moveTopLeft( QPoint( x, y ) );
	p.drawImage( r, d->m_thumbnail, d->m_thumbnail.rect() );
}
```

We want an image to be resized automatically on resizing of widget. That is why I overrided
resizeEvent().

```cpp
void
Frame::resizeEvent( QResizeEvent * e )
{
	if( d->m_mode == ResizeMode::FitToSize ||
		( d->m_mode == ResizeMode::FitToHeight && e->size().height() != d->m_thumbnail.height() ) )
			d->resized();

	e->accept();
}
```

And mouseReleaseEvent() to notify about clicking on the frame.

```cpp
void
Frame::mouseReleaseEvent( QMouseEvent * e )
{
	if( e->button() == Qt::LeftButton )
	{
		emit clicked();

		e->accept();
	}
	else
		e->ignore();
}
```

Great, a few hundred lines of code (with blank ones and comments) and we have a class that will display
image of the frame, have different behaviour for different cases. Qt rocks!

[Back](plans.md) | [Contents](../README.md) | [Next](frame-on-tape.md)