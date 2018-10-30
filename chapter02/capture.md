# Capture images

When video surface detects motion.

```
void
MainWindow::motionDetected()
{
	if( d->m_cam )
	{
		if( !d->m_isRecording )
		{
			d->m_isRecording = true;

			takeImage();

			d->m_timer->start( d->m_takeImageInterval );
		}
		else
			d->m_stopTimer->stop();
	}
}
```

We start to capture images from camera with configured interval.

```
void
MainWindow::takeImage()
{
	if( d->m_capture )
	{
		const QDateTime current = QDateTime::currentDateTime();

		QDir dir( d->m_cfg.folder() );

		const QString path = dir.absolutePath() +
			current.date().toString( QLatin1String( "/yyyy/MM/dd/" ) );

		dir.mkpath( path );

		d->m_cam->searchAndLock();

		d->m_capture->capture( path +
			current.toString( QLatin1String( "hh.mm.ss" ) ) );
	}
}
```

Where d->m_timer->timeout() connected to.

```
MainWindow::connect( m_timer, &QTimer::timeout,
		q, &MainWindow::takeImage );
```

And when there is no mo motion in the frame.

```
void
MainWindow::noMoreMotion()
{
	if( d->m_cam )
	{
		if( d->m_isRecording )
			d->m_stopTimer->start( d->m_takeImagesYetInterval );
	}
}

void
MainWindow::stopRecording()
{
	if( d->m_cam )
	{
		d->m_stopTimer->stop();

		d->m_timer->stop();

		d->m_isRecording = false;
	}
}
```

Where d->m_stopTimer->timeout() connected to MainWindow::stopRecording().

Thus, we will have pictures of the attackers on the protected area.

[Back](camera.md) | [Contents](../README.md) | [Next](../links.md)