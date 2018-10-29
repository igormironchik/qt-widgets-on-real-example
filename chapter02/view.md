# View

In this project we want to detect motion in the frame, so we need to have
access to each frame in the camera's stream. So the only solution is
QAbstractVideoSurface. And we want to display stream from camera in
some case of view finder. We need to tie together QAbstractVideoSurface
and any view finder. I see only one solution - is to transmit QImage
with current frame from QAbstractVideoSurface to custom view finder,
that will display current frame.

So let's do such a view finder.

```
#ifndef SECURITYCAM_VIEW_HPP_INCLUDED
#define SECURITYCAM_VIEW_HPP_INCLUDED

// Qt include.
#include <QWidget>
#include <QScopedPointer>


namespace SecurityCam {

//
// View
//

class ViewPrivate;

//! View of the video data from the camera.
class View final
	:	public QWidget
{
	Q_OBJECT

public:
	explicit View( QWidget * parent );
	~View() noexcept override;

public slots:
	//! Draw image.
	void draw( const QImage & image );

protected:
	void paintEvent( QPaintEvent * ) override;
	void resizeEvent( QResizeEvent * e ) override;

private:
	Q_DISABLE_COPY( View )

	QScopedPointer< ViewPrivate > d;
}; // class View

} /* namespace SecurityCam */

#endif // SECURITYCAM_VIEW_HPP_INCLUDED
```

Private data class.

```
class ViewPrivate {
public:
	explicit ViewPrivate( View * parent )
		:	m_resized( false )
		,	q( parent )
	{
	}

	//! Init.
	void init();

	//! Image.
	QImage m_image;
	//! Resized?
	bool m_resized;
	//! Parent.
	View * q;
}; // class ViewPrivate

void
ViewPrivate::init()
{
	q->setAutoFillBackground( false );
}
```

I the data class I store current frame and a flag that current frame was resized. This is the main trick,
draw() slot will be connected to video surface signal and will receive frames at maximum speed in the
background, where we will just copy frame and set resized flag to false, and will trigger update
of the widget. GUI part of the view will draw new frame when it can do it, so we will not have a
long queue of frames to draw, we will quickly process this queue. Let's look.

```
View::View( QWidget * parent )
	:	QWidget( parent )
	,	d( new ViewPrivate( this ) )
{
	d->init();
}

View::~View() noexcept
{
}

void
View::draw( const QImage & image )
{
	d->m_resized = false;

	d->m_image = image;

	update();
}

void
View::paintEvent( QPaintEvent * )
{
	if( isVisible() )
	{
		QPainter p( this );
		p.setBackground( Qt::black );

		if( !d->m_image.isNull() )
		{
			if( !d->m_resized )
			{
				d->m_image = d->m_image.scaled( size(), Qt::KeepAspectRatio );

				d->m_resized = true;
			}

			const int x = rect().x() + ( size().width() - d->m_image.width() ) / 2;
			const int y = rect().y() + ( size().height() - d->m_image.height() ) / 2;

			p.drawImage( x, y, d->m_image );
		}
	}
}

void
View::resizeEvent( QResizeEvent * e )
{
	e->accept();

	d->m_resized = false;

	update();
}
```

We do actual resize of the frame only in paint event and only if it was not done before. Believe me
in the running application I don't see any flickering. This simple code do what it was designed for.
Memory and CPU usage is constant and very low.

[Back](intro.md) | [Contents](../README.md) | [Next](view.md)