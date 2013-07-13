/*
  Copyright 2013 Mats Sjöberg
  
  This file is part of the Pumpa programme.

  Pumpa is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Pumpa is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU General Public License
  along with Pumpa.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "objectwidgetwithsignals.h"

//------------------------------------------------------------------------------

ObjectWidgetWithSignals::ObjectWidgetWithSignals(QWidget* parent) :
  QFrame(parent) {}

//------------------------------------------------------------------------------

void ObjectWidgetWithSignals::connectSignals(ObjectWidgetWithSignals* ow, 
                                             QWidget* w) 
{
  connect(ow, SIGNAL(linkHovered(const QString&)),
          w, SIGNAL(linkHovered(const QString&)));
  connect(ow, SIGNAL(like(QASObject*)),
          w, SIGNAL(like(QASObject*)));
  connect(ow, SIGNAL(share(QASObject*)),
          w, SIGNAL(share(QASObject*)));
  connect(ow, SIGNAL(newReply(QASObject*)),
          w, SIGNAL(newReply(QASObject*)));
  connect(ow, SIGNAL(follow(QString, bool)),
          w, SIGNAL(follow(QString, bool)));
}
