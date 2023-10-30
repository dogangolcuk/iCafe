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
/* server.c*/

#include "server.h"

#define PORT 7010

server::server ( QHostAddress  name )
{
	if ( !name.isNull() )
	{
		sock = new QTcpSocket ( this );
		sock->connectToHost ( name, PORT );
		hostName = name;
	}
	else
		qDebug ( "Geçerli bir Adres girin." );
}

server::~server ()
{
	if ( hostName.isNull() )
		return;
	sock->close ();
}

void server::stopComputer ()
{
	if ( hostName.isNull() )
// 		return;
		qDebug ( "Geçerli bir Adres girin." );
	sendCmd ( "<ZEIBERBUDE><STOP /></ZEIBERBUDE>" );
// sendCmd ("LOCK\n");
}

void server::startComputer ()
{
	if ( hostName.isNull() )
// 		return;
		qDebug ( "Geçerli bir Adres girin." );
	sendCmd ( "<ZEIBERBUDE><START /></ZEIBERBUDE>" );
// sendCmd ("UNLOCK\n");
}

void server::sendStatus ( QString time, QString price )
{
	QString buf;
	QString msg =
	    "<ZEIBERBUDE><STATUS TIME=\"" + time + "\" PRICE=\"" + price +
	    "\" /></ZEIBERBUDE>";
	sendCmd ( msg );
}

void server::sendCmd ( QString msg )
{
	if ( sock->state() == QAbstractSocket::UnconnectedState ||
	        sock->waitForDisconnected ( 1000 ) )
		qDebug ( "Sokete bağlantı yok." );
	QTextStream s ( sock );
	s << msg << "\n";
}
