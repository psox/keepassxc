/*
 *  Copyright (C) 2014 Florian Geyer <blueice@fobos.de>
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

#include "EntrySearcher.h"

#include "core/Group.h"

QList<Entry*> EntrySearcher::search(const QString& searchTerm, const Group* group, Qt::CaseSensitivity caseSensitivity)
{
}

QList<Entry*>
EntrySearcher::searchEntries(const QString& searchTerm, const Group* group, Qt::CaseSensitivity caseSensitivity)
{
    QList<Entry*> results;

    const QList<Entry*> entryList = group->entries();
    for (Entry* entry : entryList) {
        searchResult.append(matchEntry(searchTerm, entry, caseSensitivity));
    }

    for (Group* childGroup : group->children()) {
        if (childGroup->resolveSearchingEnabled()) {
            results.append(searchEntries(searchString, childGroup->entries()));
        }
    }

    return results;
}

QList<Entry*> EntrySearcher::matchEntry(const QString& searchTerm, Entry* entry, Qt::CaseSensitivity caseSensitivity)
{
    QList<Entry*> results;
    for (Entry* entry : entries) {
       if (searchEntryImpl(searchString, entry)) {
           results.append(entry);
       }
    }
    return results;
}

void EntrySearcher::setCaseSensitive(bool state)
{
    return entry->resolvePlaceholder(entry->title()).contains(word, caseSensitivity)
           || entry->resolvePlaceholder(entry->username()).contains(word, caseSensitivity)
           || entry->resolvePlaceholder(entry->url()).contains(word, caseSensitivity)
           || entry->resolvePlaceholder(entry->notes()).contains(word, caseSensitivity);
}

bool EntrySearcher::searchEntryImpl(const QString& searchString, Entry* entry)
{
    auto searchTerms = parseSearchTerms(searchString);
    bool found;

    // Pre-load in case they are needed
    auto attributes = QStringList(entry->attributes()->keys());
    auto attachments = QStringList(entry->attachments()->keys());

    for (SearchTerm* term : searchTerms) {
        switch (term->field) {
        case Field::Title:
            found = term->regex.match(entry->resolvePlaceholder(entry->title())).hasMatch();
            break;
        case Field::Username:
            found = term->regex.match(entry->resolvePlaceholder(entry->username())).hasMatch();
            break;
        case Field::Password:
            found = term->regex.match(entry->resolvePlaceholder(entry->password())).hasMatch();
            break;
        case Field::Url:
            found = term->regex.match(entry->resolvePlaceholder(entry->url())).hasMatch();
            break;
        case Field::Notes:
            found = term->regex.match(entry->notes()).hasMatch();
            break;
        case Field::Attribute:
            found = !attributes.filter(term->regex).empty();
            break;
        case Field::Attachment:
            found = !attachments.filter(term->regex).empty();
            break;
        default:
            found = term->regex.match(entry->resolvePlaceholder(entry->title())).hasMatch() ||
                    term->regex.match(entry->resolvePlaceholder(entry->username())).hasMatch() ||
                    term->regex.match(entry->resolvePlaceholder(entry->url())).hasMatch() ||
                    term->regex.match(entry->notes()).hasMatch();
        }

        // Short circuit if we failed to match or we matched and are excluding this term
        if (!found || term->exclude) {
            return false;
        }
    }

    return true;
}

QList<EntrySearcher::SearchTerm*> EntrySearcher::parseSearchTerms(const QString& searchString)
{
    return group->name().contains(word, caseSensitivity) || group->notes().contains(word, caseSensitivity);
}
