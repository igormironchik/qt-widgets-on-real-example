# Camera

We have video surface that can be used as view finder for QCamera,
we have view that will display stream from the camera, now we need
to initialize the camera.

```
void
MainWindowPrivate::initCamera()
{
	if( !m_cfg.camera().isEmpty() )
	{
		auto infos = QCameraInfo::availableCameras();

		if( !infos.isEmpty() )
		{
			QCameraInfo info;

			foreach( auto & i, infos )
			{
				if( i.deviceName() == m_cfg.camera() )
				{
					info = i;

					m_currCamInfo.reset( new QCameraInfo( info ) );

					break;
				}
			}

			if( !info.isNull() )
			{
				m_cam = new QCamera( info, q );

				QObject::connect( m_cam, &QCamera::statusChanged,
					q, &MainWindow::camStatusChanged );

				m_cam->setViewfinder( m_frames );

				m_capture = new QCameraImageCapture( m_cam, q );

				m_cam->setCaptureMode( QCamera::CaptureStillImage );

				q->setStatusLabel();

				m_cam->start();
			}
			else
			{
				QTimer::singleShot( c_cameraReinitTimeout,
					[&] () { q->cameraError(); } );
			}
		}
	}
}
```

m_cfg.camera() is saved device name configured in the options dialog, when the application
started it reads configuration and initializes camera, we just are looking for saved
camera in the system, and if found allocating new QCamera, QCameraImageCapture objects,
set view finder - our video surface (m_frames).

We connected to QCamera::statusChanged signal to set resolution of the camera, we
need to do it exactly in QCamera::LoadedStatus state, as only there we can ask the
camera for supported view finder settings.

```
void
MainWindow::camStatusChanged( QCamera::Status st )
{
	if( st == QCamera::LoadedStatus )
	{
		const auto settings = d->m_cam->supportedViewfinderSettings();

		QCameraViewfinderSettings toApply;

		for( const auto & s : settings )
		{
			if( s.resolution().width() == d->m_cfg.resolution().width() &&
				s.resolution().height() == d->m_cfg.resolution().height() &&
				qAbs( s.maximumFrameRate() - d->m_cfg.resolution().fps() ) <= 0.001 )
			{
				toApply = s;

				break;
			}
		}

		if( !toApply.isNull() )
		{
			d->m_cam->stop();

			d->m_cam->setViewfinderSettings( toApply );

			setStatusLabel();

			d->m_cam->start();
		}
	}
}
```

On any error in the camera we do.

```
void
MainWindow::cameraError()
{
	d->stopCamera();

	d->m_view->draw( QImage() );

	QTimer::singleShot( c_cameraReinitTimeout, this,
		[&] () { d->initCamera(); } );
}
```

Where d->m_view is our view. And.

```
void
MainWindowPrivate::stopCamera()
{
	m_stopTimer->stop();

	if( m_cam )
	{
		m_cam->stop();

		m_capture->deleteLater();

		m_capture = Q_NULLPTR;

		m_cam->deleteLater();

		m_cam = Q_NULLPTR;

		m_currCamInfo.reset();
	}
}
```

This allow us to have always initialized camera (if this is possible) with
correct resolution and frame rate.

[Back](surface.md) | [Contents](../README.md) | [Next](../links.md)