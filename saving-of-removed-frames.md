# Saving

We can open GIF, we can navigate through the frames, we can uncheck some frames.
Let's do saving of GIF with regards of unchecked frames. This is a basics of any
GIF editor. First of all we should notify user that file was changed when he
checks/unchecks frames. For it we should connect to Tape's checkStateChanged()
signal. Let's do it in the constrauctor of MainWindow.

```
connect( d->m_view->tape(), &Tape::checkStateChanged,
		this, &MainWindow::frameChecked );
```

I changed a little checkStateChanged() signal, so it looks like.

```
//! Frame checked/unchecked.
void checkStateChanged( int idx, bool checked );
```

MainWindow's frameChecked() slot is simple.

```
void
MainWindow::frameChecked( int, bool )
{
	setWindowModified( true );
}
```

We just notifying a user that GIF was changed. Let's have a look at saveGifAs()
slot

```
void
MainWindow::saveGifAs()
{
	auto fileName = QFileDialog::getSaveFileName( this,
		tr( "Choose file to save to..." ), QString(), tr( "GIF (*.gif)" ) );

	if( !fileName.isEmpty() )
	{
		if( !fileName.endsWith( QLatin1String( ".gif" ), Qt::CaseInsensitive ) )
			fileName.append( QLatin1String( ".gif" ) );

		d->m_currentGif = fileName;

		QFileInfo info( fileName );

		setWindowTitle( tr( "GIF Editor - %1[*]" ).arg( info.fileName() ) );

		saveGif();
	}
}
```

So the main work is done in saveGif() slot.

```
void
MainWindow::saveGif()
{
	std::vector< Magick::Image > toSave;

	for( int i = 0; i < d->m_view->tape()->count(); ++i )
	{
		if( d->m_view->tape()->frame( i + 1 )->isChecked() )
			toSave.push_back( d->m_frames.at( static_cast< std::size_t > ( i ) ) );
	}

	if( !toSave.empty() )
	{
		try {
			Magick::writeImages( toSave.begin(), toSave.end(), d->m_currentGif.toStdString() );

			d->m_view->tape()->removeUnchecked();

			d->m_frames = toSave;

			setWindowModified( false );
		}
		catch( const Magick::Exception & x )
		{
			QMessageBox::warning( this, tr( "Failed to save GIF..." ),
				QString::fromLocal8Bit( x.what() ) );
		}
	}
	else
	{
		QMessageBox::information( this, tr( "Can't save GIF..." ),
			tr( "Can't save GIF image with no frames." ) );
	}
}
```

We just iterating through the frames and checking if they checked, saving all checked frames to the GIF,
and if all is ok we updating UI. I added a new method to the Tape class to simplify this process.

```
void
Tape::removeUnchecked()
{
	const int c = count();
	int removed = 0;

	for( int i = 1; i <= c; ++i )
	{
		if( !frame( i - removed )->isChecked() )
		{
			removeFrame( i - removed );

			++removed;
		}
		else
			frame( i - removed )->setCounter( i - removed );
	}
}
```

We just removing unchecked frames and updating counter.

To be more user frendly I changed a little openGif() slot.

```
oid
MainWindow::openGif()
{
	const auto fileName = QFileDialog::getOpenFileName( this,
		tr( "Open GIF..." ), QString(), tr( "GIF (*.gif)" ) );

	if( !fileName.isEmpty() )
	{
		if( isWindowModified() )
		{
			const auto btn = QMessageBox::question( this,
				tr( "GIF was changed..." ),
				tr( "\"%1\" was changed.\n"
					"Do you want to save it?" ) );

			if( btn == QMessageBox::Yes )
				saveGif();
		}

		d->clearView();

		setWindowModified( false );

		setWindowTitle( tr( "GIF Editor" ) );

		d->m_view->currentFrame()->setImage( QImage() );

		try {
			std::vector< Magick::Image > frames;

			Magick::readImages( &frames, fileName.toStdString() );

			Magick::coalesceImages( &d->m_frames, frames.begin(), frames.end() );

			QFileInfo info( fileName );

			setWindowTitle( tr( "GIF Editor - %1[*]" ).arg( info.fileName() ) );

			d->m_currentGif = fileName;

			std::for_each( d->m_frames.cbegin(), d->m_frames.cend(),
				[this] ( const Magick::Image & img )
				{
					this->d->m_view->tape()->addFrame( this->d->convert( img ) );
				} );

			if( !d->m_frames.empty() )
				d->m_view->tape()->setCurrentFrame( 1 );
		}
		catch( const Magick::Exception & x )
		{
			d->clearView();

			QMessageBox::warning( this, tr( "Failed to open GIF..." ),
				QString::fromLocal8Bit( x.what() ) );
		}
	}
}
```

Just added a question, set window's title and a small exception safety.

[Back](reading.md) | [Contents](README.md) | [Next](saving-of-removed-frames.md)