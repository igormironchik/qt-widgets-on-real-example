# Basics of the main window

## Introduction

Each Qt widgets application should have one or more top-level widgets. For GIF editor we need one top-level window
where we will display frames, current frame, toolbar with actions for editing, a menu bar with different actions.
Qt has ready to use class QMainWindow which we can derive from and implement needed for our functionality.

## Inheritance

Let's inherit from QMainWindow to have the ability to implement our functionality. We will start from basics, in the mainwindow.hpp we have:

```cpp
#ifndef GIF_EDITOR_MAINWINDOW_HPP_INCLUDED
#define GIF_EDITOR_MAINWINDOW_HPP_INCLUDED

// Qt include.
#include <QMainWindow>
#include <QScopedPointer>


//
// MainWindow
//

class MainWindowPrivate;

//! Main window.
class MainWindow final
	:	public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
	~MainWindow() noexcept override;

private:
	Q_DISABLE_COPY( MainWindow )

	QScopedPointer< MainWindowPrivate > d;
}; // class MainWindow

#endif // GIF_EDITOR_MAINWINDOW_HPP_INCLUDED
```

I publicly inherited from QMainWindow and in private section you can see usage of Q\_OBJECT macros.
This macro is needed by Qt's moc to generate auxiliary code for signals and slots. At this time we
don't have any signals or slots, but it's a good practice to use Q\_OBJECT macros in every class derived from
QObject.

I use in my Qt applications private implementation idiom, for this I declared class MainWindowPrivate and in
MainWindow I declared member - smart pointer to MainWindowPrivate. The private implementation is good for reducing
compile time, it hides details of implementation from interface.

Implementation at this point is very simple (mainwindow.cpp):

```cpp
// GIF editor include.
#include "mainwindow.hpp"


//
// MainWindowPrivate
//

class MainWindowPrivate {
public:
	MainWindowPrivate( MainWindow * parent )
		:	q( parent )
	{
	}

	//! Parent.
	MainWindow * q;
}; // class MainWindowPrivate


//
// MainWindow
//

MainWindow::MainWindow()
	:	d( new MainWindowPrivate( this ) )
{
}
```

For the future, I defined member to the parent object of MainWindow in MainWindowPrivate class. It can help us
in the future to access MainWindow methods from data class (MainWindowPrivate).

## Menu

Ok. We have the skeleton of our main window. Let's add File menu with open, save, save as and quit actions. We
want to implement GIF editor and without such basic functions our application will cost nothing. First of all,
let's define slots in MainWindow class for these actions.

```cpp
private slots:
	//! Open GIF.
	void openGif();
	//! Save GIF.
	void saveGif();
	//! Save GIF as.
	void saveGifAs();
	//! Quit.
	void quit();
```

QMainWindow has a menu bar, status bar, central widget, etc. For such actions it's a good place in the File menu,
as in almost all desktop applications. In the constructor of MainWindow we will add code to create the File
menu and fill it with actions. Let's see:

```cpp
MainWindow::MainWindow()
	:	d( new MainWindowPrivate( this ) )
{
	setWindowTitle( tr( "GIF Editor" ) );

	auto file = menuBar()->addMenu( tr( "&File" ) );
	file->addAction( QIcon( ":/img/document-open.png" ), tr( "Open" ),
		this, &MainWindow::openGif, tr( "Ctrl+O" ) );
	file->addSeparator();
	file->addAction( QIcon( ":/img/document-save.png" ), tr( "Save" ),
		this, &MainWindow::saveGif, tr( "Ctrl+S" ) );
	file->addAction( QIcon( ":/img/document-save-as.png" ), tr( "Save As" ),
		this, &MainWindow::saveGifAs );
	file->addSeparator();
	file->addAction( QIcon( ":/img/application-exit.png" ), tr( "Quit" ),
		this, &MainWindow::quit, tr( "Ctrl+Q" ) );
}
```

I set title of the main window and created File menu with actions and separators.

## Quit from the application

The first slot that we will implement is quit from the editor and empty implementations of other slots.

```cpp
void
MainWindow::openGif()
{

}

void
MainWindow::saveGif()
{

}

void
MainWindow::saveGifAs()
{

}

void
MainWindow::quit()
{
	if( isWindowModified() )
	{
		auto btn = QMessageBox::question( this, tr( "GIF was changed..." ),
			tr( "GIF was changed. Do you want to save changes?" ) );

		if( btn == QMessageBox::Yes )
			saveGif();
	}

	QApplication::quit();
}
```

QWidget, the parent of QMainWindow, has a mechanism to read/set a flag if something was changed in. Why not?
In our editor we will set this flag on user's changes and clear it on saving. For the future I added in data
class member m_currentGif of QString type, where I will store the full path to the current GIF image.

```cpp
	//! Current file name.
	QString m_currentGif;
	//! Parent.
	MainWindow * q;
}; // class MainWindowPrivate
```

Great. But application can be closed with the close button in the window's header. And it's a good idea to invoke
MainWindow::quit() slot in handler of this event. For this case we will override closeEvent(), so in MainWindow:

```cpp
protected:
	void closeEvent( QCloseEvent * e ) override;
```

And implementation:

```cpp
void
MainWindow::closeEvent( QCloseEvent * e )
{
	quit();

	e->accept();
}
```

[Back](intro.md) | [Contents](../README.md) | [Next](launching.md)
