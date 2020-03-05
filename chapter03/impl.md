# Implementation

Long Magick++ operations like readImages(), coalesceImages(),
writeImages() can longs very much. And during these operations and another UI
preparations I'd like to show busy animation. I moved the view of the application to the
QStackedWidget, that was set as a central widget of QMainWindow, and in this stacked widget
I added a page with busy animation. During long operations I will show the page with
animation, and when all is done I will show the ready result.

With Qt's stuff all is simple, I just dilute the code with
QApplication::processEvents(), like.

```cpp
//! Initialize tape.
void initTape()
{
	std::for_each( m_frames.cbegin(), m_frames.cend(),
		[this] ( const Magick::Image & img )
		{
			this->m_view->tape()->addFrame( this->convert( img ) );

			QApplication::processEvents();
		} );
}
```

But Magick++ operation should work in separate thread as we can't change the code
of Magick++ functions. I decided to run these functions with QRunnable on
QThreadPool. Magick++ can throw exceptions, so I declared the base class for all
my runnables.

```cpp
class RunnableWithException
	:	public QRunnable
{
public:
	std::exception_ptr exception() const
	{
		return m_eptr;
	}

	~RunnableWithException() noexcept override = default;

protected:
	std::exception_ptr m_eptr;
}; // class RunnableWithException
```

And let's look at the implementation of the readImages() as runnable object.

```cpp
class ReadGIF final
	:	public RunnableWithException
{
public:
	ReadGIF( std::vector< Magick::Image > * container,
		const std::string & fileName )
		:	m_container( container )
		,	m_fileName( fileName )
	{
		setAutoDelete( false );
	}

	void run() override
	{
		try {
			Magick::readImages( m_container, m_fileName );
		}
		catch( ... )
		{
			m_eptr = std::current_exception();
		}
	}

private:
	std::vector< Magick::Image > * m_container;
	std::string m_fileName;
}; // class ReadGIF
```

Voila. And when I need to read GIF.

```cpp
std::vector< Magick::Image > frames;

ReadGIF read( &frames, fileName.toStdString() );
QThreadPool::globalInstance()->start( &read );

d->waitThreadPool();

if( read.exception() )
	std::rethrow_exception( read.exception() );
```

Where d->waitThreadPool() is.

```cpp
//! Wait for thread pool.
void waitThreadPool()
{
	while( !QThreadPool::globalInstance()->waitForDone( 100 / 6 ) )
		QApplication::processEvents();
}
```

That's all. Now GIF editor shows busy animation during long operations, UI is responsive.

I decided to disable all actions during such operations, even quit from the application. But
what if the user wants to exit from the application during opening? We can allow to do it on the close
button in the window's title click.

```cpp
void
MainWindow::closeEvent( QCloseEvent * e )
{
	if ( d->m_busyFlag )
	{
		const auto btn = QMessageBox::question( this, tr( "GIF editor is busy..." ),
			tr( "GIF editor is busy.\nDo you want to terminate the application?" ) );

		if( btn == QMessageBox::Yes )
			exit( -1 );
		else
			e->ignore();
	}
	else
		e->accept();

	quit();
}
```

Where d->m_busyFlag is a bool that I set to true when showing busy animation.

Wonderful, UI is always responsive and the user can terminate the application during
the long operation at any time.

[Back](intro.md) | [Contents](../README.md) | [Next](../chapter04/mistakes.md)