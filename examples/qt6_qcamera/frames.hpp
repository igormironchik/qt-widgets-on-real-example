
/*!
	\author Igor Mironchik (igor dot mironchik at gmail dot com).

	MIT License

	Copyright (c) 2022 Igor Mironchik

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#ifndef FRAMES_HPP_INCLUDED
#define FRAMES_HPP_INCLUDED

// Qt include.
#include <QVideoSink>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QPointer>


namespace Qt6QCamera {

//
// Frames
//

//! Frames listener.
class Frames
	:	public QVideoSink
{
	Q_OBJECT

	Q_PROPERTY( QVideoSink* videoSink READ videoSink WRITE setVideoSink NOTIFY videoSinkChanged )
	Q_PROPERTY( qreal angle READ angle NOTIFY angleChanged )
	Q_PROPERTY( qreal xScale READ xScale NOTIFY xScaleChanged )
	Q_PROPERTY( qreal yScale READ yScale NOTIFY yScaleChanged )
	Q_PROPERTY( QString formatString READ formatString NOTIFY formatStringChanged )
	Q_PROPERTY( QString fpsString READ fpsString NOTIFY fpsStringChanged )

signals:
	//! Video sink changed.
	void videoSinkChanged();

	void angleChanged();
	void xScaleChanged();
	void yScaleChanged();
	void formatStringChanged();
	void fpsStringChanged();

public:
	static void registerQmlType();

	explicit Frames( QObject * parent = nullptr );
	~Frames() override;

	//! \return Sink of video output.
	QVideoSink * videoSink() const;
	//! Set sink of video output.
	void setVideoSink( QVideoSink * newVideoSink );

	//! \return Rotation angle.
	qreal angle() const;
	//! \return X scale factor.
	qreal xScale() const;
	//! \return Y scale factor
	qreal yScale() const;
	//! \return Format of video frame as string.
	QString formatString() const;
	//! \return FPS as string.
	QString fpsString() const;

private slots:
	//! Init camera.
	void initCam();
	//! Stop camera.
	void stopCam();
	//! Video frame changed.
	void newFrame( const QVideoFrame & frame );
	//! 1s timer.
	void timer();

private:
	Q_DISABLE_COPY( Frames )

	//! Camera.
	QCamera * m_cam;
	//! Capture.
	QMediaCaptureSession m_capture;
	//! Video sink of video output.
	QPointer< QVideoSink > m_videoSink;
	//! Format string.
	QString m_formatString;
	//! FPS string.
	QString m_fpsString;
	//! FPS.
	int m_fps;
	//! Timer.
	QTimer * m_1s;
}; // class Frames

} /* namespace Qt6QCamera */

#endif // FRAMES_HPP_INCLUDED
