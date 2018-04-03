/*
 *  Copyright (C) 2014 Florian Geyer <blueice@fobos.de>
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

#include "TestEntrySearcher.h"
#include "TestGlobal.h"

QTEST_GUILESS_MAIN(TestEntrySearcher)

void TestEntrySearcher::initTestCase()
{
    m_groupRoot = new Group();
}

void TestEntrySearcher::cleanupTestCase()
{
    delete m_groupRoot;
}

void TestEntrySearcher::testSearch()
{
    Group* group1 = new Group();
    Group* group2 = new Group();
    Group* group3 = new Group();

    group1->setParent(m_groupRoot);
    group2->setParent(m_groupRoot);
    group3->setParent(m_groupRoot);

    Group* group11 = new Group();

    group11->setParent(group1);

    Group* group21 = new Group();
    Group* group211 = new Group();
    Group* group2111 = new Group();

    group21->setParent(group2);
    group211->setParent(group21);
    group2111->setParent(group211);

    group1->setSearchingEnabled(Group::Disable);

    Entry* eRoot = new Entry();
    eRoot->setNotes("test search term test");
    eRoot->setGroup(m_groupRoot);

    Entry* eRoot2 = new Entry();
    eRoot2->setNotes("test term test");
    eRoot2->setGroup(m_groupRoot);

    Entry* e1 = new Entry();
    e1->setNotes("test search term test");
    e1->setGroup(group1);

    Entry* e11 = new Entry();
    e11->setNotes("test search term test");
    e11->setGroup(group11);

    Entry* e2111 = new Entry();
    e2111->setNotes("test search term test");
    e2111->setGroup(group2111);

    Entry* e2111b = new Entry();
    e2111b->setNotes("test search test");
    e2111b->setGroup(group2111);

    Entry* e3 = new Entry();
    e3->setNotes("test search term test");
    e3->setGroup(group3);

    Entry* e3b = new Entry();
    e3b->setNotes("test search test");
    e3b->setGroup(group3);

    m_searchResult = m_entrySearcher.search("search term", m_groupRoot);
    QCOMPARE(m_searchResult.count(), 2);

    m_searchResult = m_entrySearcher.search("search term", group211);
    QCOMPARE(m_searchResult.count(), 1);

    // Parent group disabled search
    m_searchResult = m_entrySearcher.search("search term", group11);
    QCOMPARE(m_searchResult.count(), 0);
}

void TestEntrySearcher::testAndConcatenationInSearch()
{
    Entry* entry = new Entry();
    entry->setNotes("abc def ghi");
    entry->setTitle("jkl");
    entry->setGroup(m_groupRoot);

    m_searchResult = m_entrySearcher.search("", m_groupRoot);
    QCOMPARE(m_searchResult.count(), 1);

    m_searchResult = m_entrySearcher.search("def", m_groupRoot);
    QCOMPARE(m_searchResult.count(), 1);

    m_searchResult = m_entrySearcher.search("  abc    ghi  ", m_groupRoot);
    QCOMPARE(m_searchResult.count(), 1);

    m_searchResult = m_entrySearcher.search("ghi ef", m_groupRoot);
    QCOMPARE(m_searchResult.count(), 1);

    m_searchResult = m_entrySearcher.search("abc ef xyz", m_groupRoot);
    QCOMPARE(m_searchResult.count(), 0);

    m_searchResult = m_entrySearcher.search("abc kl", m_groupRoot);
    QCOMPARE(m_searchResult.count(), 1);
}

void TestEntrySearcher::testAllAttributesAreSearched()
{
    Entry* entry = new Entry();
    entry->setGroup(m_groupRoot);

    entry->setTitle("testTitle");
    entry->setUsername("testUsername");
    entry->setUrl("testUrl");
    entry->setNotes("testNote");

    m_searchResult =
        m_entrySearcher.search("testTitle testUsername testUrl testNote", m_groupRoot, Qt::CaseInsensitive);
    QCOMPARE(m_searchResult.count(), 1);
}

void TestEntrySearcher::testSearchTermParser()
{
    // Test standard search terms
    auto terms = m_entrySearcher.parseSearchTerms("-test \"quoted \\\"string\\\"\"  user:user pass:\"test me\" noquote  ");

    QCOMPARE(terms.length(), 5);

    QCOMPARE(terms[0]->field, EntrySearcher::All);
    QCOMPARE(terms[0]->word, QString("test"));
    QCOMPARE(terms[0]->exclude, true);

    QCOMPARE(terms[1]->field, EntrySearcher::All);
    QCOMPARE(terms[1]->word, QString("quoted \\\"string\\\""));
    QCOMPARE(terms[1]->exclude, false);

    QCOMPARE(terms[2]->field, EntrySearcher::Username);
    QCOMPARE(terms[2]->word, QString("user"));

    QCOMPARE(terms[3]->field, EntrySearcher::Password);
    QCOMPARE(terms[3]->word, QString("test me"));

    QCOMPARE(terms[4]->field, EntrySearcher::All);
    QCOMPARE(terms[4]->word, QString("noquote"));

    qDeleteAll(terms);

    // Test wildcard and regex search terms
    terms = m_entrySearcher.parseSearchTerms("+url:*.google.com *user:\\d+\\w{2}");

    QCOMPARE(terms.length(), 2);

    QCOMPARE(terms[0]->field, EntrySearcher::Url);
    QCOMPARE(terms[0]->regex.pattern(), QString("^.*\\.google\\.com$"));

    QCOMPARE(terms[1]->field, EntrySearcher::Username);
    QCOMPARE(terms[1]->regex.pattern(), QString("\\d+\\w{2}"));

    qDeleteAll(terms);
}
