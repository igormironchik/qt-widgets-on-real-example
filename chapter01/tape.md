# Tape

Ok, now we have FrameOnTape class, but this class can display a single frame.
But animated GIF has several frames. And we should display all frames in a sequence -
tape. The tape should be a horizontally scrollable widget with all available frames in
the GIF. Scrollable? This is simple, in Qt we have QScrollArea class, and we just need
a widget that should have ability to add, remove frames on it, and should grow in width
on adding new frames, as well as it should reduce its width on removing a frame.

Let's reuse as much code as possible. We just need a QWidget with QHBoxLayout
where we will add FrameOnTape objects.

So, as usuaul, let's have a look at class declarartion.

```cpp
#ifndef GIF_EDITOR_TAPE_HPP_INCLUDED
#define GIF_EDITOR_TAPE_HPP_INCLUDED

// Qt include.
#include <QWidget>
#include <QScopedPointer>


class FrameOnTape;


//
// Tape
//

class TapePrivate;

//! Tape with frames.
class Tape final
	:	public QWidget
{
	Q_OBJECT

signals:
	//! Frame clicked.
	void clicked( int idx );
	//! Current frame changed.
	void currentFrameChanged( int idx );

public:
	Tape( QWidget * parent = nullptr );
	~Tape() noexcept override;

	//! \return Count of frames.
	int count() const;
	//! Add frame.
	void addFrame( const QImage & img );
	//! \return Frame.
	FrameOnTape * frame( int idx ) const;
	//! \return Current frame.
	FrameOnTape * currentFrame() const;
	//! Set current frame.
	void setCurrentFrame( int idx );
	//! Remove frame.
	void removeFrame( int idx );
	//! Clear.
	void clear();

private:
	Q_DISABLE_COPY( Tape )

	QScopedPointer< TapePrivate > d;
}; // class Tape

#endif // GIF_EDITOR_TAPE_HPP_INCLUDED
```

API is intuitive, it doesn't need an explanation, so let's look at the implementation.

Private data class looks like.

```cpp
//
// TapePrivate
//

class TapePrivate {
public:
	TapePrivate( Tape * parent )
		:	m_currentFrame( nullptr )
		,	m_layout( new QHBoxLayout( parent ) )
		,	q( parent )
	{
		m_layout->setMargin( 5 );
		m_layout->setSpacing( 5 );
	}

	//! Frames.
	QList< FrameOnTape* > m_frames;
	//! Current frame.
	FrameOnTape * m_currentFrame;
	//! Layout.
	QHBoxLayout * m_layout;
	//! Parent.
	Tape * q;
}; // class TapePrivate
```

We need access to all frames, so we have a data member of type QList\< FrameOnTape \>,
a auxiliary member that will hold a pointer to the currently selected frame, and our layout.

Trivial methods.

```cpp
Tape::Tape( QWidget * parent )
	:	QWidget( parent )
	,	d( new TapePrivate( this ) )
{
}

Tape::~Tape() noexcept
{
}

int
Tape::count() const
{
	return d->m_frames.count();
}

FrameOnTape *
Tape::frame( int idx ) const
{
	if( idx >= 1 && idx <= count() )
		return d->m_frames.at( idx - 1 );
	else
		return nullptr;
}

FrameOnTape *
Tape::currentFrame() const
{
	return d->m_currentFrame;
}

void
Tape::clear()
{
	const int c = count();

	for( int i = 1; i <= c; ++i )
		removeFrame( 1 );
}
```

Just will say that indexes in our API start from 1. Let's look at addFrame() method.

```cpp
void
Tape::addFrame( const QImage & img )
{
	d->m_frames.append( new FrameOnTape( img, count() + 1, this ) );
	d->m_layout->addWidget( d->m_frames.back() );

	connect( d->m_frames.back(), &FrameOnTape::clicked,
		[this] ( int idx )
		{
			if( this->currentFrame() )
				this->currentFrame()->setCurrent( false );

			this->d->m_currentFrame = this->frame( idx );

			this->d->m_currentFrame->setCurrent( true );

			emit this->currentFrameChanged( idx );

			emit this->clicked( idx );
		} );

	adjustSize();
}
```

We created new FrameOnTape object, added it to the list and to the layout. Connected
clicked() signal to do stuff for the current frame. And resized the entire widget. So, when
a new frame will be added the tape will grow in width.

setCurrentFrame() is quite simple.

```cpp
void
Tape::setCurrentFrame( int idx )
{
	if( idx >= 1 && idx <= count() )
	{
		if( d->m_currentFrame )
			d->m_currentFrame->setCurrent( false );

		d->m_currentFrame = frame( idx );
		d->m_currentFrame->setCurrent( true );

		emit currentFrameChanged( idx );
	}
	else
		d->m_currentFrame = nullptr;
}
```

And some magic in the removeFrame() method. I implemented it so when current frame deletes,
a new one will become current, so we always will have selected frame.

```cpp
void
Tape::removeFrame( int idx )
{
	if( idx <= count() )
	{
		d->m_layout->removeWidget( d->m_frames.at( idx - 1 ) );
		d->m_frames.at( idx - 1 )->deleteLater();

		if( d->m_frames.at( idx - 1 ) == d->m_currentFrame )
		{
			d->m_currentFrame = nullptr;

			if( idx > 1 )
			{
				d->m_currentFrame = d->m_frames.at( idx - 2 );
				d->m_currentFrame->setCurrent( true );

				emit currentFrameChanged( idx - 1 );
			}
			else if( idx < count() )
			{
				d->m_currentFrame = d->m_frames.at( idx );
				d->m_currentFrame->setCurrent( true );

				emit currentFrameChanged( idx + 1 );
			}
			else
				d->m_currentFrame = nullptr;
		}

		d->m_frames.removeAt( idx - 1 );

		adjustSize();
	}
}
```

[Back](frame-on-tape.md) | [Contents](../README.md) | [Next](view.md)
