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

#include "util.h"

#include <QRegExp>

#include "sundown/markdown.h"
#include "sundown/html.h"
#include "sundown/buffer.h"

//------------------------------------------------------------------------------

QString markDown(QString text) {
  struct sd_callbacks callbacks;
  struct html_renderopt options;
  sdhtml_renderer(&callbacks, &options, 0);

  struct sd_markdown* markdown = sd_markdown_new(0, 16, &callbacks, &options);

  struct buf* ob = bufnew(64);
  // QByteArray ba = text.toLocal8Bit();
  QByteArray ba = text.toUtf8();

  sd_markdown_render(ob, (const unsigned char*)ba.constData(), ba.size(),
                     markdown);
  sd_markdown_free(markdown);

  // QString ret = QString::fromLocal8Bit((char*)ob->data, ob->size);
  QString ret = QString::fromUtf8((char*)ob->data, ob->size);
  bufrelease(ob);

  return ret;
}

//------------------------------------------------------------------------------

QString siteUrlFixer(QString url) {
  if (!url.startsWith("http://") && !url.startsWith("https://"))
    url = "https://" + url;

  if (url.endsWith('/'))
    url.chop(1);

  return url;
}

//------------------------------------------------------------------------------

QString linkifyUrls(QString text, QString before, QString after) {
  QRegExp rx(QString("(%2)%1(%3)").arg(URL_REGEX).arg(before).arg(after));

  int pos = 0;
  while ((pos = rx.indexIn(text, pos)) != -1) {
    int len = rx.matchedLength();
    QString newText = QString("%2<a href=\"%1\">%1</a>%3").arg(rx.cap(2)).
      arg(rx.cap(1)).arg(rx.cap(rx.captureCount()));

    text.replace(pos, len, newText);
    pos += newText.count();
  }
  return text;
}

//------------------------------------------------------------------------------

QString changePairedTags(QString text, 
                         QString begin, QString end,
                         QString newBegin, QString newEnd,
                         QString nogoItems) {
  QRegExp rx(QString(MD_PAIR_REGEX).arg(begin).arg(end).arg(nogoItems));
  int pos = 0;
  while ((pos = rx.indexIn(text, pos)) != -1) {
    int len = rx.matchedLength();
    QString newText = QString(newBegin + rx.cap(1) + newEnd);
    text.replace(pos, len, newText);
    pos += newText.count();
  }
  return text;
}

//------------------------------------------------------------------------------

QString siteUrlToAccountId(QString username, QString url) {
  if (url.startsWith("http://"))
    url.remove(0, 7);
  if (url.startsWith("https://"))
    url.remove(0, 8);

  if (url.endsWith('/'))
    url.chop(1);
 
  return username + "@" + url;
}

