# Crop

As said we need to implement a crop function. First of all we need to implement a widget
that will draw a rectangle on top of the current frame, that will show the crop region. I spent
on this widget not so little time, a day, this is because this widget is very custom
and complicated in implementation, not hard but complicated. There are a lot of calculations
of regions of handles for adjusting the rectangle, mouse cursor handling, cursor overriding,
etc. Let's have a look at this widget.

## Widget

Declaration.

```
#ifndef GIF_EDITOR_CROP_HPP_INCLUDED
#define GIF_EDITOR_CROP_HPP_INCLUDED

// Qt include.
#include <QWidget>
#include <QScopedPointer>

class Frame;


//
// CropFrame
//

class CropFramePrivate;

//! Crop frame.
class CropFrame final
	:	public QWidget
{
	Q_OBJECT

public:
	CropFrame( Frame * parent = nullptr );
	~CropFrame() noexcept override;

	//! \return Crop rectangle.
	QRect cropRect() const;

public slots:
	//! Start.
	void start();
	//! Stop.
	void stop();

private slots:
	//! Frame resized.
	void frameResized();

protected:
	void paintEvent( QPaintEvent * ) override;
	void mousePressEvent( QMouseEvent * e ) override;
	void mouseMoveEvent( QMouseEvent * e ) override;
	void mouseReleaseEvent( QMouseEvent * e ) override;
	void enterEvent( QEvent * e ) override;
	void leaveEvent( QEvent * e ) override;

private:
	Q_DISABLE_COPY( CropFrame )

	QScopedPointer< CropFramePrivate > d;
}; // class CropFrame

#endif // GIF_EDITOR_CROP_HPP_INCLUDED
```

API is simple, but let's look at what is under the hood.

Crop rectangle will have handles to change the geometry of the rectangle. And in the code
I defined a constant to store the size for it.

```
//! Size of the handle to change geometry of selected region.
static const int c_handleSize = 15;
```

Private data class.

```
class CropFramePrivate {
public:
	CropFramePrivate( CropFrame * parent, Frame * toObserve )
		:	m_started( false )
		,	m_nothing( true )
		,	m_clicked( false )
		,	m_hovered( false )
		,	m_cursorOverriden( false )
		,	m_handle( Handle::Unknown )
		,	m_frame( toObserve )
		,	q( parent )
	{
	}

	enum class Handle {
		Unknown,
		TopLeft,
		Top,
		TopRight,
		Right,
		BottomRight,
		Bottom,
		BottomLeft,
		Left
	}; // enum class Handle

	//! Bound point to available space.
	QPoint boundToAvailable( const QPoint & p ) const;
	//! Bound left top point to available space.
	QPoint boundLeftTopToAvailable( const QPoint & p ) const;
	//! Check and override cursor if necessary.
	void checkAndOverrideCursor( Qt::CursorShape shape );
	//! Override cursor.
	void overrideCursor( const QPoint & pos );
	//! Resize crop.
	void resize( const QPoint & pos ) ;
	//! \return Cropped rect.
	QRect cropped( const QRect & full ) const;
	//! \return Is handles should be outside selected rect.
	bool isHandleOutside() const
	{
		return ( qAbs( m_selected.width() ) / 3 < c_handleSize + 1 ||
			qAbs( m_selected.height() ) / 3 < c_handleSize + 1 );
	}
	//! \return Top-left handle rect.
	QRect topLeftHandleRect() const
	{
		return ( isHandleOutside() ?
			QRect( m_selected.x() - ( m_selected.width() > 0 ? c_handleSize : 0 ),
				m_selected.y() - ( m_selected.height() > 0 ? c_handleSize : 0 ),
				c_handleSize, c_handleSize ) :
			QRect( m_selected.x() - ( m_selected.width() > 0 ? 0 : c_handleSize ),
				m_selected.y() - ( m_selected.height() > 0 ? 0 : c_handleSize ),
				c_handleSize, c_handleSize ) );
	}
	//! \return Top-right handle rect.
	QRect topRightHandleRect() const
	{
		return ( isHandleOutside() ?
			QRect( m_selected.x() + m_selected.width() - 1 -
					( m_selected.width() > 0 ? 0 : c_handleSize ),
				m_selected.y() - ( m_selected.height() > 0 ? c_handleSize : 0 ),
				c_handleSize, c_handleSize ) :
			QRect( m_selected.x() + m_selected.width() -
					( m_selected.width() > 0 ? c_handleSize : 0 ) - 1,
				m_selected.y() - ( m_selected.height() > 0 ? 0 : c_handleSize ),
				c_handleSize, c_handleSize ) );
	}
	//! \return Bottom-right handle rect.
	QRect bottomRightHandleRect() const
	{
		return ( isHandleOutside() ?
			QRect( m_selected.x() + m_selected.width() - 1 -
					( m_selected.width() > 0 ? 0 : c_handleSize ),
				m_selected.y() + m_selected.height() -
					( m_selected.height() > 0 ? 0 : c_handleSize ),
				c_handleSize, c_handleSize ) :
			QRect( m_selected.x() + m_selected.width() -
					( m_selected.width() > 0 ? c_handleSize : 0 ) - 1,
				m_selected.y() + m_selected.height() -
					( m_selected.height() > 0 ? c_handleSize : 0 )  - 1,
				c_handleSize, c_handleSize ) );
	}
	//! \return Bottom-left handle rect.
	QRect bottomLeftHandleRect() const
	{
		return ( isHandleOutside() ?
			QRect( m_selected.x() - ( m_selected.width() > 0 ? c_handleSize : 0 ),
				m_selected.y() + m_selected.height() - 1 -
					( m_selected.height() > 0 ? 0 : c_handleSize),
				c_handleSize, c_handleSize ) :
			QRect( m_selected.x() - ( m_selected.width() > 0 ? 0 : c_handleSize),
				m_selected.y() + m_selected.height() -
					( m_selected.height() > 0 ? c_handleSize : 0 ) - 1,
				c_handleSize, c_handleSize ) );
	}
	//! \return Y handle width.
	int yHandleWidth() const
	{
		const int w = m_selected.width() - 1;

		return ( isHandleOutside() ? w :
			w - 2 * c_handleSize - ( w - 2 * c_handleSize ) / 3 );
	}
	//! \return X handle height.
	int xHandleHeight() const
	{
		const int h = m_selected.height() - 1;

		return ( isHandleOutside() ? h :
			h - 2 * c_handleSize - ( h - 2 * c_handleSize ) / 3 );
	}
	//! \return Y handle x position.
	int yHandleXPos() const
	{
		return ( m_selected.x() + ( m_selected.width() - yHandleWidth() ) / 2 );
	}
	//! \return X handle y position.
	int xHandleYPos() const
	{
		return ( m_selected.y() + ( m_selected.height() - xHandleHeight() ) / 2 );
	}
	//! \return Top handle rect.
	QRect topHandleRect() const
	{
		return ( isHandleOutside() ?
			QRect( yHandleXPos(), m_selected.y() - ( m_selected.height() > 0 ? c_handleSize : 0 ),
				yHandleWidth(), c_handleSize ) :
			QRect( yHandleXPos(), m_selected.y() - ( m_selected.height() > 0 ? 0 : c_handleSize ),
				yHandleWidth(), c_handleSize ) );
	}
	//! \return Bottom handle rect.
	QRect bottomHandleRect() const
	{
		return ( isHandleOutside() ?
			QRect( yHandleXPos(), m_selected.y() + m_selected.height() - 1 -
					( m_selected.height() > 0 ? 0 : c_handleSize ),
				yHandleWidth(), c_handleSize ) :
			QRect( yHandleXPos(), m_selected.y() + m_selected.height() - 1 -
					( m_selected.height() > 0 ? c_handleSize : 0 ),
				yHandleWidth(), c_handleSize ) );
	}
	//! \return Left handle rect.
	QRect leftHandleRect() const
	{
		return ( isHandleOutside() ?
			QRect( m_selected.x() - ( m_selected.width() > 0 ? c_handleSize : 0 ),
				xHandleYPos(), c_handleSize, xHandleHeight() ) :
			QRect( m_selected.x() - ( m_selected.width() > 0 ? 0 : c_handleSize ),
				xHandleYPos(), c_handleSize, xHandleHeight() ) );
	}
	//! \return Right handle rect.
	QRect rightHandleRect() const
	{
		return ( isHandleOutside() ?
			QRect( m_selected.x() + m_selected.width() - 1 -
					( m_selected.width() > 0 ? 0 : c_handleSize ),
				xHandleYPos(), c_handleSize, xHandleHeight() ) :
			QRect( m_selected.x() + m_selected.width() - 1 -
					( m_selected.width() > 0 ? c_handleSize : 0 ),
				xHandleYPos(), c_handleSize, xHandleHeight() ) );
	}

	//! Selected rectangle.
	QRect m_selected;
	//! Available rectangle.
	QRect m_available;
	//! Mouse pos.
	QPoint m_mousePos;
	//! Selecting started.
	bool m_started;
	//! Nothing selected yet.
	bool m_nothing;
	//! Clicked.
	bool m_clicked;
	//! Hover entered.
	bool m_hovered;
	//! Cursor overriden.
	bool m_cursorOverriden;
	//! Current handle.
	Handle m_handle;
	//! Frame to observe resize event.
	Frame * m_frame;
	//! Parent.
	CropFrame * q;
}; // class CropFramePrivate
```

Uhh, so many methods... I defined some methods in the class, these methods just returns
rectangles of the handles, you could understand it from their names.

We will resize, move selection rectangle, and we don't want this rectangle to go out of
frame boundary. And for this task we have two auxiliary methods.

```
QPoint
CropFramePrivate::boundToAvailable( const QPoint & p ) const
{
	QPoint ret = p;

	if( p.x() < m_available.x() )
		ret.setX( m_available.x() );
	else if( p.x() > m_available.x() + m_available.width() - 1 )
		ret.setX( m_available.x() + m_available.width() - 1 );

	if( p.y() < m_available.y() )
		ret.setY( m_available.y() );
	else if( p.y() > m_available.y() + m_available.height() - 1 )
		ret.setY( m_available.y() + m_available.height() - 1 );

	return ret;
}

QPoint
CropFramePrivate::boundLeftTopToAvailable( const QPoint & p ) const
{
	QPoint ret = p;

	if( p.x() < m_available.x() )
		ret.setX( m_available.x() );
	else if( p.x() > m_available.x() + m_available.width() - m_selected.width() - 1)
		ret.setX( m_available.x() + m_available.width() - m_selected.width() - 1 );

	if( p.y() < m_available.y() )
		ret.setY( m_available.y() );
	else if( p.y() > m_available.y() + m_available.height() - m_selected.height() - 1 )
		ret.setY( m_available.y() + m_available.height() - m_selected.height() - 1 );

	return ret;
}
```

When the user moves the mouse cursor over the widget, different regions we need to override cursor
to help the user understand what he can do. Auxiliary methods to override cursor.

```
void
CropFramePrivate::checkAndOverrideCursor( Qt::CursorShape shape )
{
	if( QApplication::overrideCursor() )
	{
		if( *QApplication::overrideCursor() != QCursor( shape ) )
		{
			if( m_cursorOverriden )
				QApplication::restoreOverrideCursor();
			else
				m_cursorOverriden = true;

			QApplication::setOverrideCursor( QCursor( shape ) );
		}
	}
	else
	{
		m_cursorOverriden = true;

		QApplication::setOverrideCursor( QCursor( shape ) );
	}
}

void
CropFramePrivate::overrideCursor( const QPoint & pos )
{
	if( topLeftHandleRect().contains( pos ) )
	{
		m_handle = CropFramePrivate::Handle::TopLeft;
		checkAndOverrideCursor( Qt::SizeFDiagCursor );
	}
	else if( bottomRightHandleRect().contains( pos ) )
	{
		m_handle = CropFramePrivate::Handle::BottomRight;
		checkAndOverrideCursor( Qt::SizeFDiagCursor );
	}
	else if( topRightHandleRect().contains( pos ) )
	{
		m_handle = CropFramePrivate::Handle::TopRight;
		checkAndOverrideCursor( Qt::SizeBDiagCursor );
	}
	else if( bottomLeftHandleRect().contains( pos ) )
	{
		m_handle = CropFramePrivate::Handle::BottomLeft;
		checkAndOverrideCursor( Qt::SizeBDiagCursor );
	}
	else if( topHandleRect().contains( pos ) )
	{
		m_handle = CropFramePrivate::Handle::Top;
		checkAndOverrideCursor( Qt::SizeVerCursor );
	}
	else if( bottomHandleRect().contains( pos ) )
	{
		m_handle = CropFramePrivate::Handle::Bottom;
		checkAndOverrideCursor( Qt::SizeVerCursor );
	}
	else if( leftHandleRect().contains( pos ) )
	{
		m_handle = CropFramePrivate::Handle::Left;
		checkAndOverrideCursor( Qt::SizeHorCursor );
	}
	else if( rightHandleRect().contains( pos ) )
	{
		m_handle = CropFramePrivate::Handle::Right;
		checkAndOverrideCursor( Qt::SizeHorCursor );
	}
	else if( m_selected.contains( pos ) )
	{
		m_handle = CropFramePrivate::Handle::Unknown;
		checkAndOverrideCursor( Qt::SizeAllCursor );
	}
	else if( m_cursorOverriden )
	{
		m_cursorOverriden = false;
		m_handle = CropFramePrivate::Handle::Unknown;
		QApplication::restoreOverrideCursor();
	}
}
```

When user presses and moves handle selection rectangle should resize, so the method for it.

```
void
CropFramePrivate::resize( const QPoint & pos )
{
	switch( m_handle )
	{
		case CropFramePrivate::Handle::Unknown :
			m_selected.moveTo( boundLeftTopToAvailable(
				m_selected.topLeft() - m_mousePos + pos ) );
		break;

		case CropFramePrivate::Handle::TopLeft :
			m_selected.setTopLeft( boundToAvailable( m_selected.topLeft() -
				m_mousePos + pos ) );
		break;

		case CropFramePrivate::Handle::TopRight :
			m_selected.setTopRight( boundToAvailable( m_selected.topRight() -
				m_mousePos + pos ) );
		break;

		case CropFramePrivate::Handle::BottomRight :
			m_selected.setBottomRight( boundToAvailable( m_selected.bottomRight() -
				m_mousePos + pos ) );
		break;

		case CropFramePrivate::Handle::BottomLeft :
			m_selected.setBottomLeft( boundToAvailable( m_selected.bottomLeft() -
				m_mousePos + pos ) );
		break;

		case CropFramePrivate::Handle::Top :
			m_selected.setTop( boundToAvailable( QPoint( m_selected.left(), m_selected.top() ) -
				m_mousePos + pos ).y() );
		break;

		case CropFramePrivate::Handle::Bottom :
			m_selected.setBottom( boundToAvailable( QPoint( m_selected.left(),
				m_selected.bottom() ) - m_mousePos + pos ).y() );
		break;

		case CropFramePrivate::Handle::Left :
			m_selected.setLeft( boundToAvailable( QPoint( m_selected.left(),
				m_selected.top() ) - m_mousePos + pos ).x() );
		break;

		case CropFramePrivate::Handle::Right :
			m_selected.setRight( boundToAvailable( QPoint( m_selected.right(),
				m_selected.top() ) - m_mousePos + pos ).x() );
		break;
	}

	m_mousePos = pos;
}
```

We can draw a crop rectangle on the scaled frame, but for cropping we need to know rectangle
to crop in the original frame's coordinates.

```
QRect
CropFramePrivate::cropped( const QRect & full ) const
{
	const auto oldR = m_available;

	const qreal xRatio = static_cast< qreal > ( full.width() ) /
		static_cast< qreal > ( oldR.width() );
	const qreal yRatio = static_cast< qreal > ( full.height() ) /
		static_cast< qreal > ( oldR.height() );

	QRect r;

	if( !m_nothing )
	{
		const auto x = static_cast< int >( ( m_selected.x() - oldR.x() ) * xRatio ) +
			full.x();
		const auto y = static_cast< int >( ( m_selected.y() - oldR.y() ) * yRatio ) +
			full.y();
		const auto dx = full.bottomRight().x() - static_cast< int >(
			( oldR.bottomRight().x() - m_selected.bottomRight().x() ) * xRatio );
		const auto dy = full.bottomRight().y() - static_cast< int >(
			( oldR.bottomRight().y() - m_selected.bottomRight().y() ) * yRatio );

		r.setTopLeft( QPoint( x, y ) );
		r.setBottomRight( QPoint( dx, dy ) );
	}

	return r;
}
```

You can ask how it's possible to write all these methods first and only then
implement methods of the widget? I guess that this is impossible. I wrote a skeleton of widget
and step by step wrote code, so these private data methods were born from time to time
when they were needed. Developing is an iterative process. With some experience you
will come to it, but I believe that you are an experienced C++ developer and just want
to quickly look at working methods to develop on Qt's widgets. Let's go.

The widget as is very simple with all these auxiliary methods, have a look.

```
CropFrame::CropFrame( Frame * parent )
	:	QWidget( parent )
	,	d( new CropFramePrivate( this, parent ) )
{
	setAutoFillBackground( false );
	setAttribute( Qt::WA_TranslucentBackground, true );
	setMouseTracking( true );

	d->m_available = parent->imageRect();

	connect( d->m_frame, &Frame::resized,
		this, &CropFrame::frameResized );
}

CropFrame::~CropFrame() noexcept
{
	if( d->m_cursorOverriden )
		QApplication::restoreOverrideCursor();

	if( d->m_hovered )
		QApplication::restoreOverrideCursor();
}

QRect
CropFrame::cropRect() const
{
	return d->cropped( d->m_frame->image().rect() );
}

void
CropFrame::start()
{
	d->m_started = true;
	d->m_nothing = true;

	update();
}

void
CropFrame::stop()
{
	d->m_started = false;

	update();
}
```

I added to Frame class resized() signal to handle resizing and correctly
resize selection region.

```
void
CropFrame::frameResized()
{
	d->m_selected = d->cropped( d->m_frame->imageRect() );

	setGeometry( QRect( 0, 0, d->m_frame->width(), d->m_frame->height() ) );

	d->m_available = d->m_frame->imageRect();

	update();
}
```

Painting of our widget.

```
void
CropFrame::paintEvent( QPaintEvent * )
{
	static const QColor dark( 0, 0, 0, 100 );

	QPainter p( this );
	p.setPen( Qt::black );
	p.setBrush( dark );

	if( d->m_started && !d->m_nothing )
	{
		QPainterPath path;
		path.addRect( QRectF( d->m_available ).adjusted( 0, 0, -1, -1 ) );

		if( d->m_available != d->m_selected )
		{
			QPainterPath spath;
			spath.addRect( QRectF( d->m_selected ).adjusted( 0, 0, -1, -1 ) );
			path = path.subtracted( spath );
		}
		else
			p.setBrush( Qt::transparent );

		p.drawPath( path );
	}

	p.setBrush( Qt::transparent );

	if( d->m_started && !d->m_clicked && !d->m_nothing &&
		d->m_handle == CropFramePrivate::Handle::Unknown )
	{
		p.drawRect( d->topLeftHandleRect() );
		p.drawRect( d->topRightHandleRect() );
		p.drawRect( d->bottomRightHandleRect() );
		p.drawRect( d->bottomLeftHandleRect() );
	}
	else if( d->m_started && !d->m_nothing &&
		d->m_handle != CropFramePrivate::Handle::Unknown )
	{
		switch( d->m_handle )
		{
			case CropFramePrivate::Handle::TopLeft :
				p.drawRect( d->topLeftHandleRect() );
			break;

			case CropFramePrivate::Handle::TopRight :
				p.drawRect( d->topRightHandleRect() );
			break;

			case CropFramePrivate::Handle::BottomRight :
				p.drawRect( d->bottomRightHandleRect() );
			break;

			case CropFramePrivate::Handle::BottomLeft :
				p.drawRect( d->bottomLeftHandleRect() );
			break;

			case CropFramePrivate::Handle::Top :
				p.drawRect( d->topHandleRect() );
			break;

			case CropFramePrivate::Handle::Bottom :
				p.drawRect( d->bottomHandleRect() );
			break;

			case CropFramePrivate::Handle::Left :
				p.drawRect( d->leftHandleRect() );
			break;

			case CropFramePrivate::Handle::Right :
				p.drawRect( d->rightHandleRect() );
			break;

			default:
				break;
		}
	}
}
```

The behaviour of crop region is like in Gimp. When user has drawn rectangle on mouse release
he will see a transparent rectangle with darkening semi-transparent background on a non-selected
region and corner handles. To access the top, bottom, left and right handles user should
move the mouse cursor in the centres of the edges. And when the mouse cursor is on a handle,
only this handle will be drawn and the mouse cursor will be overridden, like in Gimp.

And mouse handling.

```
void
CropFrame::mousePressEvent( QMouseEvent * e )
{
	if( e->button() == Qt::LeftButton )
	{
		d->m_clicked = true;

		if( !d->m_cursorOverriden )
			d->m_selected.setTopLeft( d->boundToAvailable( e->pos() ) );
		else
			d->m_mousePos = e->pos();

		update();

		e->accept();
	}
	else
		e->ignore();
}

void
CropFrame::mouseMoveEvent( QMouseEvent * e )
{
	if( d->m_clicked )
	{
		if ( !d->m_cursorOverriden )
		{
			d->m_selected.setBottomRight( d->boundToAvailable( e->pos() ) );

			d->m_nothing = false;
		}
		else
			d->resize( e->pos() );

		update();

		e->accept();
	}
	else if( !d->m_hovered )
	{
		d->m_hovered = true;

		QApplication::setOverrideCursor( QCursor( Qt::CrossCursor ) );
	}
	else if( d->m_hovered && !d->m_nothing )
	{
		d->overrideCursor( e->pos() );

		update();
	}
	else
		e->ignore();
}

void
CropFrame::mouseReleaseEvent( QMouseEvent * e )
{
	d->m_clicked = false;

	if( e->button() == Qt::LeftButton )
	{
		d->m_selected = d->m_selected.normalized();

		update();

		e->accept();
	}
	else
		e->ignore();
}

void
CropFrame::enterEvent( QEvent * e )
{
	if( d->m_started )
	{
		d->m_hovered = true;

		QApplication::setOverrideCursor( QCursor( Qt::CrossCursor ) );

		e->accept();
	}
	else
		e->ignore();
}

void
CropFrame::leaveEvent( QEvent * e )
{
	if( d->m_started )
	{
		d->m_hovered = false;

		QApplication::restoreOverrideCursor();

		e->accept();
	}
	else
		e->ignore();
}
```

## Integrating crop frame into view

In the View private data class I added pointer to CropFrame widget.

```
//! Crop.
CropFrame * m_crop;
```

And two slots to start and stop crop operation.

```
void
View::startCrop()
{
	if( !d->m_crop )
	{
		d->m_crop = new CropFrame( d->m_currentFrame );
		d->m_crop->setGeometry( QRect( 0, 0,
			d->m_currentFrame->width(), d->m_currentFrame->height() ) );
		d->m_crop->show();
		d->m_crop->raise();
		d->m_crop->start();
	}
}

void
View::stopCrop()
{
	if( d->m_crop )
	{
		d->m_crop->stop();
		d->m_crop->deleteLater();
		d->m_crop = nullptr;
	}
}
```

So crop frame will be the same size as current frame widget and will be placed
on top of it.

To access crop region I added a method.

```
QRect
View::cropRect() const
{
	if( d->m_crop )
		return d->m_crop->cropRect();
	else
		return QRect();
}
```

Nothing more.

## Cropping

We need menu and tool bar to start, finish and cancel crop operation,
so in MainWindow's constructor we added.

```
d->m_crop = new QAction( QIcon( ":/img/transform-crop.png" ),
	tr( "Crop" ), this );
d->m_crop->setShortcut( tr( "Ctrl+C" ) );
d->m_crop->setShortcutContext( Qt::ApplicationShortcut );
d->m_crop->setCheckable( true );
d->m_crop->setChecked( false );
d->m_crop->setEnabled( false );

d->m_applyEdit = new QAction( this );
d->m_applyEdit->setShortcut( Qt::Key_Return );
d->m_applyEdit->setShortcutContext( Qt::ApplicationShortcut );
d->m_applyEdit->setEnabled( false );

d->m_cancelEdit = new QAction( this );
d->m_cancelEdit->setShortcut( Qt::Key_Escape );
d->m_cancelEdit->setShortcutContext( Qt::ApplicationShortcut );
	d->m_cancelEdit->setEnabled( false );

addAction( d->m_applyEdit );
addAction( d->m_cancelEdit );

connect( d->m_crop, &QAction::triggered, this, &MainWindow::crop );
connect( d->m_applyEdit, &QAction::triggered, this, &MainWindow::applyEdit );
connect( d->m_cancelEdit, &QAction::triggered, this, &MainWindow::cancelEdit );

auto edit = menuBar()->addMenu( tr( "&Edit" ) );
edit->addAction( d->m_crop );

auto editToolBar = new QToolBar( tr( "Edit" ), this );
editToolBar->addAction( d->m_crop );

addToolBar( Qt::LeftToolBarArea, editToolBar );
```

Reaction on triggering crop action is simple.

```
void
MainWindow::crop( bool on )
{
	if( on )
	{
		d->enableFileActions( false );

		d->m_editMode = MainWindowPrivate::EditMode::Crop;

		d->m_view->startCrop();
	}
	else
	{
		d->m_view->stopCrop();

		d->m_editMode = MainWindowPrivate::EditMode::Unknow;

		d->enableFileActions();
	}
}
```

Where.

```
//! Enable file actions.
void enableFileActions( bool on = true )
{
	m_save->setEnabled( on );
	m_saveAs->setEnabled( on );
	m_open->setEnabled( on );

	m_applyEdit->setEnabled( !on );
	m_cancelEdit->setEnabled( !on );
}
```

Cancelling and applying crop operation.

```
void
MainWindow::cancelEdit()
{
	switch( d->m_editMode )
	{
		case MainWindowPrivate::EditMode::Crop :
		{
			d->m_view->stopCrop();

			d->enableFileActions();

			d->m_crop->setChecked( false );

			d->m_editMode = MainWindowPrivate::EditMode::Unknow;
		}
			break;

		default :
			break;
	}
}

void
MainWindow::applyEdit()
{
	switch( d->m_editMode )
	{
		case MainWindowPrivate::EditMode::Crop :
		{
			const auto rect = d->m_view->cropRect();

			if( !rect.isNull() && rect != d->m_view->currentFrame()->image().rect() )
			{
				QVector< int > unchecked;

				for( int i = 1; i <= d->m_view->tape()->count(); ++i )
				{
					if( !d->m_view->tape()->frame( i )->isChecked() )
						unchecked.append( i );
				}

				try {
					auto tmpFrames = d->m_frames;

					std::for_each( tmpFrames.begin(), tmpFrames.end(),
						[&rect] ( auto & frame )
						{
							frame.crop( Magick::Geometry( rect.width(), rect.height(),
								rect.x(), rect.y() ) );
							frame.repage();
						} );

					const auto current = d->m_view->tape()->currentFrame()->counter();
					d->m_view->tape()->clear();
					d->m_frames = tmpFrames;

					d->initTape();

					d->m_view->tape()->setCurrentFrame( current );

					for( const auto & i : qAsConst( unchecked ) )
						d->m_view->tape()->frame( i )->setChecked( false );

					setWindowModified( true );

					cancelEdit();
				}
				catch( const Magick::Exception & x )
				{
					cancelEdit();

					QMessageBox::warning( this, tr( "Failed to crop GIF..." ),
						QString::fromLocal8Bit( x.what() ) );
				}
			}
			else
				cancelEdit();
		}
			break;

		default :
			break;
	}
}
```

That's all. Now our editor can crop GIFs. So first Alpha version almost done.

[Back](what-else.md) | [Contents](../README.md) | [Next](about.md)