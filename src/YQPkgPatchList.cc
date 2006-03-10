/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:	      YQPkgPatchList.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/


#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <qpopupmenu.h>
#include <qaction.h>

#include "YQi18n.h"
#include "utf8.h"

#include "YQPkgPatchList.h"
#include "YQPkgTextDialog.h"

using std::list;


YQPkgPatchList::YQPkgPatchList( QWidget * parent )
    : YQPkgObjList( parent )
    , _patchCategory( InstallablePatches )
{
    y2debug( "Creating patch list" );

    int numCol = 0;
    addColumn( "" );			_statusCol	= numCol++;
    addColumn( _( "Patch"	) );	_summaryCol	= numCol++;
    addColumn( _( "Category" 	) );	_categoryCol	= numCol++;
    addColumn( _( "Size" 	) );	_sizeCol	= numCol++;
    setAllColumnsShowFocus( true );
    setColumnAlignment( sizeCol(), Qt::AlignRight );

    connect( this,	SIGNAL( selectionChanged	( QListViewItem * ) ),
	     this,	SLOT  ( filter()				    ) );

    fillList();
    setSorting( categoryCol() );
    selectSomething();

#ifdef FIXME
    QString label = _( "Show &Raw Patch Info" );
    actionShowRawPatchInfo = new QAction( label,		// text
					  label + "\tr",	// menu text
					  ( QKeySequence ) 0,	// accel
					  ( QObject * ) 0 );	// parent

    connect( actionShowRawPatchInfo, SIGNAL( activated() ), SLOT( showRawPatchInfo() ) );
#endif

    y2debug( "Creating patch list done" );
}


YQPkgPatchList::~YQPkgPatchList()
{
    // NOP
}


void
YQPkgPatchList::fillList()
{
    clear();
    y2debug( "Filling patch list" );


    for ( ZyppPoolIterator it = zyppPatchesBegin();
	  it != zyppPatchesEnd();
	  ++it )
    {
	ZyppPatch zyppPatch = tryCastToZyppPatch( (*it)->theObj() );

	if ( zyppPatch )
	{
#ifdef FIXME
	    switch ( _patchCategory )
	    {
		case InstallablePatches:
		    if ( zyppPatch->installable() && status != S_KeepInstalled )
			addPatchItem( *it, zyppPatch );
		    break;

		case InstallableAndInstalledPatches:
		    if ( zyppPatch->installable() )
			addPatchItem( *it, zyppPatch );
		    break;

		case AllPatches:
		    addPatchItem( *it, zyppPatch );
	    }
#else
	    y2debug( "Found patch %s", zyppPatch->name().c_str() );
	    addPatchItem( *it, zyppPatch);
#endif
	}
	else
	{
	    y2error( "Found non-patch selectable" );
	}
    }


    if ( ! firstChild() )
	message( _( "No patches available." ) );

    y2debug( "patch list filled" );
}



void
YQPkgPatchList::message( const QString & text )
{
    QY2ListViewItem * item = new QY2ListViewItem( this );
    CHECK_PTR( item );

    item->setText( 1, text );
    item->setBackgroundColor( QColor( 0xE0, 0xE0, 0xF8 ) );
}



void
YQPkgPatchList::filterIfVisible()
{
    if ( isVisible() )
	filter();
}


void
YQPkgPatchList::filter()
{
    emit filterStart();

    if ( selection() )
    {
	ZyppPatch patch = selection()->zyppPatch();

	if ( patch )
	{
#ifdef FIXME
	    //
	    // Check for a pre-script
	    //

	    if ( ! patch->preScript().empty() )
	    {
		// Translators: (Fixed) name for a script that is executed
		// at the start of installation of a patch
		emit filterMatch( _( "[Pre-Script]" ), fromUTF8( patch->preScript() ), -1 );
	    }


	    //
	    // Add all packages
	    //

	    list<ZyppPkg> pkgList = patch->packages();
	    list<ZyppPkg>::const_iterator it = pkgList.begin();

	    while ( it != pkgList.end() )
	    {
		emit filterMatch( ( *it ) );
		++it;
	    }


	    //
	    // Check for extra files outside packages
	    //

	    list<PMYouFile> files = patch->files();

	    for ( list<PMYouFile>::iterator it = files.begin(); it != files.end(); ++it )
	    {
		// Translators: (Fixed) name for an extra file (outside packages)
                // that comes with a patch
		emit filterMatch( _( "[File]" ), fromUTF8( (*it).name() ), (*it).size() );
	    }


	    //
	    // Check for a post-script
	    //

	    if ( ! patch->postScript().empty() )
	    {
		// Translators: (Fixed) name for a script that is executed
                // at the end of installation of a patch
		emit filterMatch( _( "[Post-Script]" ), fromUTF8( patch->postScript() ), -1 );
	    }
#endif
	}
    }

    emit filterFinished();
}


void
YQPkgPatchList::addPatchItem( ZyppSel	selectable,
			      ZyppPatch zyppPatch )
{
    if ( ! selectable )
    {
	y2error( "NULL ZyppSel!" );
	return;
    }

    new YQPkgPatchListItem( this, selectable, zyppPatch );
}


YQPkgPatchListItem *
YQPkgPatchList::selection() const
{
    QListViewItem * item = selectedItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgPatchListItem *> (item);
}


void
YQPkgPatchList::createNotInstalledContextMenu()
{
    _notInstalledContextMenu = new QPopupMenu( this );
    CHECK_PTR( _notInstalledContextMenu );

    actionSetCurrentInstall->addTo( _notInstalledContextMenu );
    actionSetCurrentDontInstall->addTo( _notInstalledContextMenu );
    actionSetCurrentTaboo->addTo( _notInstalledContextMenu );

    addAllInListSubMenu( _notInstalledContextMenu );
}


void
YQPkgPatchList::createInstalledContextMenu()
{
    _installedContextMenu = new QPopupMenu( this );
    CHECK_PTR( _installedContextMenu );

    actionSetCurrentKeepInstalled->addTo( _installedContextMenu );
    actionSetCurrentUpdate->addTo( _installedContextMenu );
    actionSetCurrentProtected->addTo( _installedContextMenu );

    addAllInListSubMenu( _installedContextMenu );
}


QPopupMenu *
YQPkgPatchList::addAllInListSubMenu( QPopupMenu * menu )
{
    QPopupMenu * submenu = new QPopupMenu( menu );
    CHECK_PTR( submenu );

    actionSetListInstall->addTo( submenu );
    actionSetListDontInstall->addTo( submenu );
    actionSetListKeepInstalled->addTo( submenu );
    actionSetListUpdate->addTo( submenu );
    actionSetListUpdateForce->addTo( submenu );
    actionSetListTaboo->addTo( submenu );
    actionSetListProtected->addTo( submenu );

    menu->insertItem( _( "&All in This List" ), submenu );

    return submenu;
}


void
YQPkgPatchList::showRawPatchInfo()
{
    if ( selection() )
    {
	ZyppPatch patch = selection()->zyppPatch();
#ifdef FIXME
	YQPkgTextDialog::showText( this, patch, Y2PM::youPatchManager().rawPatchInfo( patch ) );
#endif
    }
}


void
YQPkgPatchList::keyPressEvent( QKeyEvent * event )
{
    if ( event )
    {
	if ( event->ascii() == '-' )
	{
	    QListViewItem * selectedListViewItem = selectedItem();

	    if ( selectedListViewItem )
	    {
		YQPkgPatchListItem * item = dynamic_cast<YQPkgPatchListItem *> (selectedListViewItem);

		if ( item && item->selectable()->hasInstalledObj() )
		{
		    y2warning( "Deleting patches is not supported" );
		    return;
		}
	    }
	}
	else if ( event->ascii() == 'r' )
	{
	    showRawPatchInfo();
	}
    }

    YQPkgObjList::keyPressEvent( event );
}




YQPkgPatchListItem::YQPkgPatchListItem( YQPkgPatchList * 	patchList,
					ZyppSel			selectable,
					ZyppPatch 		zyppPatch )
    : YQPkgObjListItem( patchList, selectable, zyppPatch )
    , _patchList( patchList )
    , _zyppPatch( zyppPatch )
{
    if ( ! _zyppPatch )
	_zyppPatch = tryCastToZyppPatch( selectable->theObj() );

    setStatusIcon();
    setText( categoryCol(), _zyppPatch->category() );
    
#ifdef FIXME

    switch ( _zyppPatch->category() )
    {
	case zypp::Patch::category_yast:	setTextColor( QColor( 0, 0, 0xC0 ) );	break;	// medium blue
	case zypp::Patch::category_security:	setTextColor( Qt::red );		break;
	case zypp::Patch::category_recommended:	setTextColor( QColor( 0, 0, 0xC0 ) );	break;	// medium blue
	case zypp::Patch::category_optional:	break;
	case zypp::Patch::category_document:	break;
	default:				break;
    }
#endif
}


YQPkgPatchListItem::~YQPkgPatchListItem()
{
    // NOP
}


void
YQPkgPatchListItem::setStatus( ZyppStatus newStatus )
{
    YQPkgObjListItem::setStatus( newStatus );
    
#ifdef FIXME
    // Call the resolver on this object
#endif
    _patchList->sendUpdatePackages();
}


void
YQPkgPatchListItem::cycleStatus()
{
    YQPkgObjListItem::cycleStatus();

    if ( status() == S_Del )	// Can't delete patches
	setStatus( S_KeepInstalled );
}


QString
YQPkgPatchListItem::toolTip( int col )
{
    QString text;

    if ( col == statusCol() )
    {
	text = YQPkgObjListItem::toolTip( col );
    }
    else
    {
	text = fromUTF8( zyppPatch()->category() );

	if ( ! text.isEmpty() )
	    text += "\n";

	text += fromUTF8( zyppPatch()->size().asString().c_str() );
    }

    return text;
}


/**
 * Comparison function used for sorting the list.
 * Returns:
 * -1 if this <	 other
 *  0 if this == other
 * +1 if this >	 other
 **/
int
YQPkgPatchListItem::compare( QListViewItem * otherListViewItem,
				int		col,
				bool		ascending ) const
{
    YQPkgPatchListItem * other = dynamic_cast<YQPkgPatchListItem *> (otherListViewItem);

    if ( other )
    {
	if ( col == categoryCol() )
	{
#ifdef FIXME
	    // This used to be a numeric field, now doing simple string sorting
#endif
	    if ( this->zyppPatch()->category() < other->zyppPatch()->category() ) return -1;
	    if ( this->zyppPatch()->category() > other->zyppPatch()->category() ) return  1;
	    return 0;
	}
    }
    return YQPkgObjListItem::compare( otherListViewItem, col, ascending );
}



#include "YQPkgPatchList.moc"