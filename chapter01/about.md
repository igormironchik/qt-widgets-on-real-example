# About

And the last step. Let's add Help menu with about dialogues.

In the MainWindow constructor.

```cpp
auto help = menuBar()->addMenu( tr( "&Help" ) );
help->addAction( QIcon( ":/img/icon_22x22.png" ), tr( "About" ),
	this, &MainWindow::about );
help->addAction( QIcon( ":/img/qt.png" ), tr( "About Qt" ),
	this, &MainWindow::aboutQt );
```

And slots.

```cpp
void
MainWindow::about()
{
	QMessageBox::about( this, tr( "About GIF editor" ),
		tr( "GIF editor.\n\n"
			"Author - Igor Mironchik (igor.mironchik at gmail dot com).\n\n"
			"Copyright (c) 2018 Igor Mironchik.\n\n"
			"Licensed under GNU GPL 3.0." ) );
}

void
MainWindow::aboutQt()
{
	QMessageBox::aboutQt( this );
}
```

Have a good day!

[Back](crop.md) | [Contents](../README.md) | [Next](../chapter02/intro.md)