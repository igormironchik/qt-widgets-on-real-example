# Video surface

As said we need to access each frame in the camera's stream. For such cases in Qt
is QAbstractVideoSurface. Custom video surface can be set to QCamera as view
fider, what we will do. But video surface doesn't draw anything, it just got
access to video frames. Painting of frames will do view that was described
in the previous section.

When derriving from QAbstractVideoSurface developer should understand that
present() method will be invoked from non-GUI thread. And very important to
return correct list of supported formats from supportedPixelFormats()
method. Video frames can come from the device in various formats, but
we want to convert QVideoFrame to QImage, so format of video frame
should be of compatible with QImage format. Qt can do the trick by
pre-converting of video frames format to supported by our video
surface format, so we will return just convertable to QImage pixel
formats.

Our video surface will detect motions and notify the application about it.
Surface will emit signal with new frames with QImage, but for performance reasons
we will emit every frame only if motion is detected, otherwise we will emit
only key frames. Surface will have abilities to transform video frame
before emitting for painting, such as mirroring and rotating.

Detection of motions is based on comparing key frame with the current one.
Surface will emit signal about difference value of the current image
and key frame. If this value (L2 relative error) is bigger than threshold
then motion is detected. Each device has its own parameters of noice in the
frames, so threshold is configurable.

Declaration.

```
#ifndef SECURITYCAM_FRAMES_HPP_INCLUDED
#define SECURITYCAM_FRAMES_HPP_INCLUDED

// Qt include.
#include <QAbstractVideoSurface>
#include <QTransform>
#include <QMutex>
#include <QTimer>

// SecurityCam include.
#include "cfg.hpp"


namespace SecurityCam {

//! Count of processed frames when key farme changes.
static const int c_keyFrameChangesOn = 10;


//
// Frames
//

//! Frames listener.
class Frames final
	:	public QAbstractVideoSurface
{
	Q_OBJECT

signals:
	//! New frame arrived.
	void newFrame( QImage );
	//! Motion detected.
	void motionDetected();
	//! No more motions.
	void noMoreMotions();
	//! Images difference.
	void imgDiff( qreal diff );
	//! No frames.
	void noFrames();
	//! FPS.
	void fps( int v );

public:
	Frames( const Cfg::Cfg & cfg, QObject * parent );

	//! \return Rotation.
	qreal rotation() const;
	//! Set rotation.
	void setRotation( qreal a );

	//! \return Mirrored?
	bool mirrored() const;
	//! Set mirrored.
	void setMirrored( bool on );

	//! \return Threshold.
	qreal threshold() const;
	//! Set threshold.
	void setThreshold( qreal v );

	//! Apply new transformations.
	void applyTransform( bool on = true );

	bool present( const QVideoFrame & frame ) override;

	QList< QVideoFrame::PixelFormat > supportedPixelFormats(
		QAbstractVideoBuffer::HandleType type =
			QAbstractVideoBuffer::NoHandle ) const override;

private:
	//! Detect motion.
	void detectMotion( const QImage & key, const QImage & image );

private slots:
	//! No frames timeout.
	void noFramesTimeout();
	//! 1 second.
	void second();

private:
	Q_DISABLE_COPY( Frames )

	//! Key frame.
	QImage m_keyFrame;
	//! Frames counter.
	int m_counter;
	//! Motions was detected.
	bool m_motion;
	//! Mutex.
	mutable QMutex m_mutex;
	//! Transformation applied.
	bool m_transformApplied;
	//! Transformation.
	QTransform m_transform;
	//! Threshold.
	qreal m_threshold;
	//! Rotation.
	qreal m_rotation;
	//! Mirrored.
	bool m_mirrored;
	//! Timer.
	QTimer * m_timer;
	//! 1 second timer.
	QTimer * m_secTimer;
	//! FPS.
	int m_fps;
}; // class Frames

} /* namespace SecurityCam */

#endif // SECURITYCAM_FRAMES_HPP_INCLUDED
```

This is a formalization in C\+\+ delcaration of what was said above. Some methods are
trivial.

```
static const int c_noFramesTimeout = 3000;

//
// Frames
//

Frames::Frames( const Cfg::Cfg & cfg, QObject * parent )
	:	QAbstractVideoSurface( parent )
	,	m_counter( 0 )
	,	m_motion( false )
	,	m_threshold( cfg.threshold() )
	,	m_rotation( cfg.rotation() )
	,	m_mirrored( cfg.mirrored() )
	,	m_timer( new QTimer( this ) )
	,	m_secTimer( new QTimer( this ) )
	,	m_fps( 0 )
{
	if( cfg.applyTransform() )
		applyTransform();

	m_timer->setInterval( c_noFramesTimeout );
	m_secTimer->setInterval( 1000 );

	connect( m_timer, &QTimer::timeout, this, &Frames::noFramesTimeout );
	connect( m_secTimer, &QTimer::timeout, this, &Frames::second );

	m_secTimer->start();
}

qreal
Frames::rotation() const
{
	return m_rotation;
}

void
Frames::setRotation( qreal a )
{
	m_rotation = a;
}

bool
Frames::mirrored() const
{
	return m_mirrored;
}

void
Frames::setMirrored( bool on )
{
	m_mirrored = on;
}

qreal
Frames::threshold() const
{
	QMutexLocker lock( &m_mutex );

	return m_threshold;
}

void
Frames::setThreshold( qreal v )
{
	QMutexLocker lock( &m_mutex );

	m_threshold = v;
}

void
Frames::applyTransform( bool on )
{
	QMutexLocker lock( &m_mutex );

	if( on )
	{		
		m_transform = QTransform();

		m_transform.rotate( m_rotation );

		if( qAbs( m_rotation ) > 0.01 )
			m_transformApplied = true;

		if( m_mirrored )
		{
			m_transform.scale( -1.0, 1.0 );

			m_transformApplied = true;
		}
	}
	else
	{
		m_transformApplied = false;

		m_transform = QTransform();
	}
}
```

The main work is done in present() method.

```
bool
Frames::present( const QVideoFrame & frame )
{
	if( !isActive() )
		return false;

	QMutexLocker lock( &m_mutex );

	QVideoFrame f = frame;
	f.map( QAbstractVideoBuffer::ReadOnly );

	QImage image( f.bits(), f.width(), f.height(), f.bytesPerLine(),
		QVideoFrame::imageFormatFromPixelFormat( f.pixelFormat() ) );

	f.unmap();

	if( m_counter == c_keyFrameChangesOn )
		m_counter = 0;

	QImage tmp = ( m_transformApplied ? image.transformed( m_transform )
		:	image.copy() );

	if( m_counter == 0 )
	{
		if( !m_keyFrame.isNull() )
			detectMotion( m_keyFrame, tmp );

		m_keyFrame = tmp;

		emit newFrame( m_keyFrame );
	}
	else if( m_motion )
		emit newFrame( tmp );

	++m_counter;
	++m_fps;

	m_timer->start();

	return true;
}
```

We are converting QVideoFrame to QImage, aplying transformation if needed, detecting
motion on each key frame, updating counters and emitting frames for drawing.
Important to connect to newFrame() signal as queued one, as present() method
invoked in non-GUI thread. And very important to emit full copy of image because
if we will emit temporary image object the data in it will be destroyed as
original QImage uses data from QVideoFrame directly and in slot we will
try to aceess destroyed memory.

Motion detection is made with help of OpenCV and is quite simple, look.

```
inline cv::Mat QImageToCvMat( const QImage & inImage )
{
	switch ( inImage.format() )
	{
		case QImage::Format_ARGB32:
		case QImage::Format_ARGB32_Premultiplied:
		{
			cv::Mat mat( inImage.height(), inImage.width(),
				CV_8UC4,
				const_cast< uchar* >( inImage.bits() ),
				static_cast< size_t >( inImage.bytesPerLine() ) );

			return mat;
		}

		case QImage::Format_RGB32:
		case QImage::Format_RGB888:
		{
			QImage swapped;

			if( inImage.format() == QImage::Format_RGB32 )
				swapped = inImage.convertToFormat( QImage::Format_RGB888 );

			swapped = inImage.rgbSwapped();

			return cv::Mat( swapped.height(), swapped.width(),
				CV_8UC3,
				const_cast< uchar* >( swapped.bits() ),
				static_cast< size_t >( swapped.bytesPerLine() ) ).clone();
		}

		default:
			break;
	}

	return cv::Mat();
}

void
Frames::detectMotion( const QImage & key, const QImage & image )
{
	bool detected = false;

	try {
		const cv::Mat A = QImageToCvMat( key );
		const cv::Mat B = QImageToCvMat( image );

		// Calculate the L2 relative error between images.
		const double errorL2 = cv::norm( A, B, CV_L2 );
		// Convert to a reasonable scale, since L2 error is summed across
		// all pixels of the image.
		const double similarity = errorL2 / (double)( A.rows * A.cols );

		detected = similarity > m_threshold;

		emit imgDiff( similarity );
	}
	catch( const cv::Exception & )
	{
	}

	if( m_motion && !detected )
	{
		m_motion = false;

		emit noMoreMotions();
	}
	else if( !m_motion && detected )
	{
		m_motion = true;

		emit motionDetected();
	}
}
```

And auxiliary trivial methods.

```
QList< QVideoFrame::PixelFormat >
Frames::supportedPixelFormats( QAbstractVideoBuffer::HandleType type ) const
{
	Q_UNUSED( type )

	return QList< QVideoFrame::PixelFormat > ()
		<< QVideoFrame::Format_ARGB32
		<< QVideoFrame::Format_ARGB32_Premultiplied
		<< QVideoFrame::Format_RGB32
		<< QVideoFrame::Format_RGB24;
}

void
Frames::noFramesTimeout()
{
	QMutexLocker lock( &m_mutex );

	m_timer->stop();

	emit noFrames();
}

void
Frames::second()
{
	QMutexLocker lock( &m_mutex );

	emit fps( m_fps );

	m_fps = 0;
}
```

If to set this video surface to QCamera as view finder and connect newFrame() signal to
View's draw() slot then we will see the stream from the camera in View widget.

[Back](view.md) | [Contents](../README.md) | [Next](camera.md)