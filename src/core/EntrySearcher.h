/*
 *  Copyright (C) 2014 Florian Geyer <debfx@fobos.de>
 *  Copyright (C) 2017 KeePassXC Team <team@keepassxc.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 or (at your option)
 *  version 3 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KEEPASSX_ENTRYSEARCHER_H
#define KEEPASSX_ENTRYSEARCHER_H

#include <QString>
#include <QRegularExpression>

class Group;
class Entry;

class EntrySearcher
{
public:
    EntrySearcher(bool caseSensitive = false);

    QList<Entry*> search(const QString& searchString, const Group* group);
    QList<Entry*> searchEntries(const QString& searchString, const QList<Entry*>& entries);

    void setCaseSensitive(bool state);

private:
    bool searchEntryImpl(const QString& searchString, Entry* entry);

    enum Field {
        All,
        Title,
        Username,
        Password,
        Url,
        Notes,
        Attribute,
        Attachment
    };

    struct SearchTerm
    {
        Field field;
        QString word;
        QRegularExpression regex;
        bool exclude;
    };

    QList<SearchTerm*> parseSearchTerms(const QString& searchString);

    bool m_caseSensitive;

    friend class TestEntrySearcher;
};

#endif // KEEPASSX_ENTRYSEARCHER_H
