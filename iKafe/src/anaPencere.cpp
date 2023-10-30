/***************************************************************************
 *   Copyright (C) 2008 by dogangolcuk   *
 *   dogangolcuk@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "anaPencere.h"
anaPencere::anaPencere()
{
	setupUi ( this );

	connect ( Pb_Lock, SIGNAL ( clicked() ), SLOT ( hostLock () ) );
	connect ( Pb_Unlock, SIGNAL ( clicked () ), SLOT ( hostUnlock () ) );
	connect ( Pb_Connect, SIGNAL ( clicked () ), SLOT ( hostConnect () ) );

}

void anaPencere::hostLock()
{

	srv->stopComputer ();
}
void anaPencere::hostUnlock()
{

	srv->startComputer ();

}

void anaPencere::hostConnect()
{

	QHostAddress host;
	host.setAddress ( Le_Host->text() );
	srv = new server ( host );
}
