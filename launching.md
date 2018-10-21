# First launch of application

We created basic main window, let's have a look at it. We need a main() function to start the application.

```
// Qt include.
#include <QApplication>
#include <QTranslator>
#include <QLocale>

// GIF editor include.
#include "mainwindow.hpp"


int main( int argc, char ** argv )
{
	QApplication app( argc, argv );

	QIcon appIcon( ":/img/icon_256x256.png" );
	appIcon.addFile( ":/img/icon_128x128.png" );
	appIcon.addFile( ":/img/icon_64x64.png" );
	appIcon.addFile( ":/img/icon_48x48.png" );
	appIcon.addFile( ":/img/icon_32x32.png" );
	appIcon.addFile( ":/img/icon_22x22.png" );
	appIcon.addFile( ":/img/icon_16x16.png" );
	app.setWindowIcon( appIcon );

	QTranslator appTranslator;
	appTranslator.load( "./tr/gif-editor_" + QLocale::system().name() );
	app.installTranslator( &appTranslator );

	MainWindow w;
	w.resize( 800, 600 );
	w.show();

	return app.exec();
}
```

We created QApplication object, icon of our application, translator, that will load translation according to
system's locale, and created MainWindow object on the stack. Set default size, and invoked show() method. Voila,
now we need to start application's event loop, what app.exec() do.

[Back](mainwindow-basics.md) | [Contents](README.md) | [Next](plans.md)