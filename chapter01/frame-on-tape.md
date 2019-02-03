# Frame on tape

Well, we have a frame widget that will display a thumbnail image. But this is not enough
for a frame on the tape. The frame on the tape should have a border, that should signal that
this frame is current or not, the frame on the tape should have a checkbox to have the ability
to remove some frames from the GIF, just deselect some frames, save file, and voila. And
frame on the tape should have a counter, indicating the position of the frame on the tape.

Sounds like we can do it with standard widgets and layouts. We can create a widget, inherited
from QFrame to have a border, QCheckBox for checkbox, QLabel for position indicator,
and QVBoxLayout and QHBoxLayout for layout. Amazing, there is nothing better than
reusing of the code, especially if this code written not by us.

The declaration of the new class looks like.

```
#ifndef GIF_EDITOR_FRAMEONTAPE_HPP_INCLUDED
#define GIF_EDITOR_FRAMEONTAPE_HPP_INCLUDED

// Qt include.
#include <QFrame>
#include <QScopedPointer>


//
// FrameOnTape
//

class FrameOnTapePrivate;

//! Frame on tape.
class FrameOnTape final
	:	public QFrame
{
	Q_OBJECT

signals:
	//! Clicked.
	void clicked( int idx );
	//! Checked.
	void checked( bool on );

public:
	FrameOnTape( const QImage & img, int counter, QWidget * parent = nullptr );
	~FrameOnTape() noexcept override;

	//! \return Image.
	const QImage & image() const;
	//! Set image.
	void setImage( const QImage & img );

	//! \return Is frame checked.
	bool isChecked() const;
	//! Set checked.
	void setChecked( bool on = true );

	//! \return Counter.
	int counter() const;
	//! Set counter.
	void setCounter( int c );

	//! \return Is this frame current?
	bool isCurrent() const;
	//! Set current flag.
	void setCurrent( bool on = true );

private:
	Q_DISABLE_COPY( FrameOnTape )

	QScopedPointer< FrameOnTapePrivate > d;
}; // class FrameOnTape

#endif // GIF_EDITOR_FRAMEONTAPE_HPP_INCLUDED
```

Nothing difficult. We just added some auxiliary API to have access to the underlying full image,
counter or position of the frame, ability to set and check if the current frame is current, and
ability to check if this frame is checked.

Implementation really very simple. Look at private data class.

```
class FrameOnTapePrivate {
public:
	FrameOnTapePrivate( const QImage & img, int counter, FrameOnTape * parent )
		:	m_counter( counter )
		,	m_current( false )
		,	m_frame( new Frame( img, Frame::ResizeMode::FitToHeight, parent ) )
		,	m_label( new QLabel( parent ) )
		,	m_checkBox( new QCheckBox( parent ) )
		,	q( parent )
	{
		m_checkBox->setChecked( true );

		m_label->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
		m_label->setText( FrameOnTape::tr( "#%1" ).arg( m_counter ) );
	}

	//! Set current state.
	void setCurrent( bool on );

	//! Counter.
	int m_counter;
	//! Is current?
	bool m_current;
	//! Frame.
	Frame * m_frame;
	//! Counter label.
	QLabel * m_label;
	//! Check box.
	QCheckBox * m_checkBox;
	//! Parent.
	FrameOnTape * q;
}; // class FrameOnTapePrivate
```

We declared setCurrent() method as we will use this code more than once.

```
void
FrameOnTapePrivate::setCurrent( bool on )
{
	m_current = on;

	if( m_current )
		q->setFrameStyle( QFrame::Panel | QFrame::Sunken );
	else
		q->setFrameStyle( QFrame::Panel | QFrame::Raised );
}
```

We just changing a frame's style to indicate that this frame is currently selected.

And the implementation of the class is so simple that even doesn't need any comments.

```
FrameOnTape::FrameOnTape( const QImage & img, int counter, QWidget * parent )
	:	QFrame( parent )
	,	d( new FrameOnTapePrivate( img, counter, this ) )
{
	auto vlayout = new QVBoxLayout( this );
	vlayout->setMargin( 0 );
	vlayout->addWidget( d->m_frame );

	auto hlayout = new QHBoxLayout;
	hlayout->setMargin( 0 );
	hlayout->addWidget( d->m_checkBox );
	hlayout->addWidget( d->m_label );

	vlayout->addLayout( hlayout );

	d->setCurrent( false );

	setLineWidth( 2 );

	setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding );

	connect( d->m_checkBox, &QCheckBox::stateChanged,
		[this] ( int state ) { emit this->checked( state != 0 ); } );
	connect( d->m_frame, &Frame::clicked,
		[this] ()
		{
			this->d->setCurrent( true );

			emit this->clicked( this->d->m_counter );
		} );
}

FrameOnTape::~FrameOnTape() noexcept
{
}

const QImage &
FrameOnTape::image() const
{
	return d->m_frame->image();
}

void
FrameOnTape::setImage( const QImage & img )
{
	d->m_frame->setImage( img );
}

bool
FrameOnTape::isChecked() const
{
	return d->m_checkBox->isChecked();
}

void
FrameOnTape::setChecked( bool on )
{
	d->m_checkBox->setChecked( on );
}

int
FrameOnTape::counter() const
{
	return d->m_counter;
}

void
FrameOnTape::setCounter( int c )
{
	d->m_counter = c;

	d->m_label->setText( tr( "#%1" ).arg( c ) );
}

bool
FrameOnTape::isCurrent() const
{
	return d->m_current;
}

void
FrameOnTape::setCurrent( bool on )
{
	d->setCurrent( on );
}
```

[Back](frame.md) | [Contents](../README.md) | [Next](tape.md)