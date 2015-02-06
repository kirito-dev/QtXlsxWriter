#include "xlsxspreadsheetcomments.h"
#include "xlsxspreadsheetcomments_p.h"
#include "xlsxcellreference.h"
#include <QXmlStreamWriter>
#include <QBuffer>
QT_BEGIN_NAMESPACE_XLSX
SpreadSheetCommentPrivate::SpreadSheetCommentPrivate(SpreadSheetComment* p, 
    SpreadSheetComment::CreateFlag flag)
    : AbstractOOXmlFilePrivate(p, flag)
{}
const Comment* SpreadSheetCommentPrivate::getComment(int row, int col) const
{
    const QMap<int, Comment*>* colList = m_Comments.value(row, NULL);
    if (!colList) return NULL;
    return colList->value(col, NULL);
}
SpreadSheetCommentPrivate::~SpreadSheetCommentPrivate()
{
    for (QMap<int, QMap<int, Comment* >* >::iterator i = m_Comments.begin();
        i != m_Comments.end(); ++i) {
        for (QMap<int, Comment* >::iterator j = i.value()->begin(); j != i.value()->end(); ++j) {
            delete j.value();
        }
        delete i.value();
    }
}
void SpreadSheetCommentPrivate::setComment(int row, int col, const Comment& val)
{
    QMap<int, QMap<int, Comment* >* >::iterator comRow = m_Comments.find(row);
    if (comRow == m_Comments.end()) {
        comRow = m_Comments.insert(row, new QMap<int, Comment*>);
    }
    QMap<int, Comment* >::iterator comCol = comRow.value()->find(col);
    if (comCol == comRow.value()->end()) {
        comRow.value()->insert(col, new Comment(val));
    }
    else {
        comCol.value()->operator=(val);
    }
}
void SpreadSheetCommentPrivate::removeComment(int row, int col)
{
    QMap<int, QMap<int, Comment* >* >::iterator comRow = m_Comments.find(row);
    if (comRow == m_Comments.end()) return;
    QMap<int, Comment* >::iterator comCol = comRow.value()->find(col);
    if (comCol == comRow.value()->end()) return;
    delete comCol.value();
    comRow.value()->erase(comCol);
    if (comRow.value()->isEmpty()) {
        delete comRow.value();
        m_Comments.erase(comRow);
    }
}
void SpreadSheetCommentPrivate::moveComment(int fromRow, int fromCol, int toRow, int toCol)
{
    QMap<int, Comment*>* fromColList = m_Comments.value(fromRow, NULL);
    if (!fromColList) return;
    Comment* oldComment = fromColList->value(fromCol, NULL);
    if (!oldComment) return;
    fromColList->remove(fromCol);
    if (fromColList->isEmpty()) {
        delete fromColList;
        m_Comments.remove(fromRow);
    }

    QMap<int, QMap<int, Comment* >* >::iterator toRowIter = m_Comments.find(toRow);
    if (toRowIter == m_Comments.end())
        toRowIter = m_Comments.insert(toRow, new  QMap<int, Comment* >);
    QMap<int, Comment* >::iterator toColIter = toRowIter.value()->find(toCol);
    if (toColIter == toRowIter.value()->end()) {
        toRowIter.value()->insert(toCol, oldComment);
    }
    else {
        delete toColIter.value();
        toColIter.value() = oldComment;
    }
    

}
bool SpreadSheetCommentPrivate::hasRow(int row) const
{
    return m_Comments.contains(row);
}
bool SpreadSheetCommentPrivate::isEmpty() const
{
    return m_Comments.isEmpty();
}
const QMap<int, QMap<int, Comment* >* >& SpreadSheetCommentPrivate::comments() const{
    return m_Comments;
}
Comment SpreadSheetComment::getComment(int row, int col) const
{
    Q_D(const SpreadSheetComment);
    const Comment* res = d->getComment(row, col);
    if (res) return *res;
    return Comment();
}

void SpreadSheetComment::setComment(int row, int col, const Comment& val)
{
    Q_D(SpreadSheetComment);
    d->setComment(row, col, val);
}

void SpreadSheetComment::setComment(const CellReference& rowCol, const Comment& val)
{
    if (!rowCol.isValid()) return;
    setComment(rowCol.row(), rowCol.column(), val);
}

void SpreadSheetComment::removeComment(int row, int col)
{
    Q_D(SpreadSheetComment);
    d->removeComment(row, col);
}

void SpreadSheetComment::removeComment(const CellReference& rowCol)
{
    if (!rowCol.isValid()) return;
    removeComment(rowCol.row(), rowCol.column());
}

void SpreadSheetComment::moveComment(int fromRow, int fromCol, int toRow, int toCol)
{
    Q_D(SpreadSheetComment);
    d->moveComment(fromRow, fromCol, toRow, toCol);
}

void SpreadSheetComment::moveComment(const CellReference& fromRowCol, int toRow, int toCol)
{
    if (!fromRowCol.isValid()) return;
    moveComment(fromRowCol.row(), fromRowCol.column(), toRow, toCol);
}

void SpreadSheetComment::moveComment(const CellReference& fromRowCol, const CellReference& toRowCol)
{
    if (!fromRowCol.isValid() || !toRowCol.isValid()) return;
    moveComment(fromRowCol.row(), fromRowCol.column(), toRowCol.row(), toRowCol.column());
}

void SpreadSheetComment::moveComment(int fromRow, int fromCol, const CellReference& toRowCol)
{
    if (!toRowCol.isValid()) return;
    moveComment(fromRow, fromCol, toRowCol.row(), toRowCol.column());
}
void SpreadSheetComment::saveShapeToXmlFile(QIODevice *device) const
{
    Q_D(const SpreadSheetComment);
    if (d->isEmpty()) return;
    QXmlStreamWriter writer(device);
    writer.writeStartDocument(QStringLiteral("1.0"));
    writer.writeStartElement(QStringLiteral("xml"));
    writer.writeAttribute(QStringLiteral("xmlns:v"),
        QStringLiteral("urn:schemas-microsoft-com:vml"));
    writer.writeAttribute(QStringLiteral("xmlns:o"),
        QStringLiteral("urn:schemas-microsoft-com:office:office"));
    writer.writeAttribute(QStringLiteral("xmlns:x"),
        QStringLiteral("urn:schemas-microsoft-com:office:excel"));
    writer.writeStartElement(QStringLiteral("o:shapelayout"));
    writer.writeEmptyElement(QStringLiteral("o:idmap"));
    writer.writeAttribute(QStringLiteral("v:ext"), QStringLiteral("edit"));
    writer.writeAttribute(QStringLiteral("data"), QStringLiteral("1"));
    writer.writeEndElement(); //o:shapelayout
    writer.writeStartElement(QStringLiteral("v:shapetype"));
    writer.writeAttribute(QStringLiteral("id"), QStringLiteral("_x0000_t202"));
    writer.writeAttribute(QStringLiteral("coordsize"), QStringLiteral("21600,21600"));
    writer.writeAttribute(QStringLiteral("o:spt"), QStringLiteral("202"));
    writer.writeAttribute(QStringLiteral("path"), QStringLiteral("m,l,21600r21600,l21600,xe"));
    writer.writeEmptyElement(QStringLiteral("v:stroke"));
    writer.writeAttribute(QStringLiteral("joinstyle"), QStringLiteral("miter"));
    writer.writeEmptyElement(QStringLiteral("v:path"));
    writer.writeAttribute(QStringLiteral("gradientshapeok"), QStringLiteral("t"));
    writer.writeAttribute(QStringLiteral("o:connecttype"), QStringLiteral("rect"));
    writer.writeEndElement(); //v:shapetype
    int commentsCounters = 0;
    for (QMap<int, QMap<int, Comment* >* >::const_iterator i = d->comments().constBegin();
        i != d->comments().constEnd(); ++i) {
        for (QMap<int, Comment* >::const_iterator j = i.value()->constBegin();
            j != i.value()->constEnd(); ++j) {
            writer.writeStartElement(QStringLiteral("v:shape"));
            writer.writeAttribute(QStringLiteral("id"), QStringLiteral("_x0000_s%1").arg(1024 + (++commentsCounters)));
            writer.writeAttribute(QStringLiteral("type"), QStringLiteral("#_x0000_t202"));
            writer.writeAttribute(QStringLiteral("style"), QStringLiteral("position:absolute;margin-left:59.25pt;margin-top:1.5pt;width:108pt;height:59.25pt;z-index:1;visibility:hidden"));
            writer.writeAttribute(QStringLiteral("fillcolor"), QStringLiteral("#ffffe1"));
            writer.writeAttribute(QStringLiteral("o:insetmode"), QStringLiteral("#auto"));
            writer.writeEmptyElement(QStringLiteral("v:fill"));
            writer.writeAttribute(QStringLiteral("color2"), QStringLiteral("#ffffe1"));
            writer.writeEmptyElement(QStringLiteral("v:shadow"));
            writer.writeAttribute(QStringLiteral("on"), QStringLiteral("t"));
            writer.writeAttribute(QStringLiteral("color"), QStringLiteral("black"));
            writer.writeAttribute(QStringLiteral("obscured"), QStringLiteral("t"));
            writer.writeEmptyElement(QStringLiteral("v:path"));
            writer.writeAttribute(QStringLiteral("o:connecttype"), QStringLiteral("none"));
            writer.writeStartElement(QStringLiteral("v:textbox"));
            writer.writeAttribute(QStringLiteral("style"), QStringLiteral("mso-direction-alt:auto"));
            writer.writeEmptyElement(QStringLiteral("div"));
            writer.writeAttribute(QStringLiteral("style"), QStringLiteral("text-align:left"));
            writer.writeEndElement(); //v:textbox
            writer.writeStartElement(QStringLiteral("x:ClientData"));
            writer.writeAttribute(QStringLiteral("ObjectType"), QStringLiteral("Note"));
            writer.writeEmptyElement(QStringLiteral("x:MoveWithCells"));
            writer.writeEmptyElement(QStringLiteral("x:SizeWithCells"));
            writer.writeStartElement(QStringLiteral("x:Anchor"));
            writer.writeCharacters(QStringLiteral("1, 15, 0, 2, 3, 31, 4, 1"));
            writer.writeEndElement(); //x:Anchor
            writer.writeStartElement(QStringLiteral("x:AutoFill"));
            writer.writeCharacters(QStringLiteral("False"));
            writer.writeEndElement(); //x:AutoFill
            writer.writeStartElement(QStringLiteral("x:Row"));
            writer.writeCharacters(QString::number(i.key()-1));
            writer.writeEndElement(); //x:Row
            writer.writeStartElement(QStringLiteral("x:Column"));
            writer.writeCharacters(QString::number(j.key()-1));
            writer.writeEndElement(); //x:Column
            writer.writeEndElement(); //x:ClientData
            writer.writeEndElement(); //v:shape
        }
    }
    writer.writeEndElement(); //xml
}
void SpreadSheetComment::saveToXmlFile(QIODevice *device) const
{
    Q_D(const SpreadSheetComment);
    if (d->isEmpty()) return;
    QXmlStreamWriter writer(device);
    writer.writeStartDocument(QStringLiteral("1.0"), true);
    writer.writeStartElement(QStringLiteral("comments"));
    writer.writeAttribute(QStringLiteral("xmlns"),
        QStringLiteral("http://schemas.openxmlformats.org/spreadsheetml/2006/main"));
    writer.writeStartElement(QStringLiteral("authors"));
    QStringList authorsList;
    for (QMap<int, QMap<int, Comment* >* >::const_iterator comRowIter = d->comments().constBegin();
        comRowIter != d->comments().constEnd(); ++comRowIter) {
        for (QMap<int, Comment* >::const_iterator comColIter = comRowIter.value()->constBegin(); 
            comColIter != comRowIter.value()->constEnd(); ++comColIter) {
            if (!authorsList.contains(comColIter.value()->author(), Qt::CaseInsensitive))
                authorsList.append(comColIter.value()->author());
        }
    }

    for (QStringList::const_iterator authIter = authorsList.constBegin();
        authIter != authorsList.constEnd(); ++authIter) {
        writer.writeStartElement(QStringLiteral("author"));
        writer.writeCharacters(*authIter);
        writer.writeEndElement();//author
    }
    writer.writeEndElement();//authors
    writer.writeStartElement(QStringLiteral("commentList"));
    QStringList commentLines;
    for (QMap<int, QMap<int, Comment* >* >::const_iterator comRowIter = d->comments().constBegin();
        comRowIter != d->comments().constEnd(); ++comRowIter) {
        for (QMap<int, Comment* >::const_iterator comColIter = comRowIter.value()->constBegin();
            comColIter != comRowIter.value()->constEnd(); ++comColIter) {
            writer.writeStartElement(QStringLiteral("comment"));
            writer.writeAttribute(QStringLiteral("ref"), 
                CellReference(comRowIter.key(), comColIter.key()).toString());
            writer.writeAttribute(QStringLiteral("authorId"),
                QString::number(authorsList.indexOf(QRegExp(comColIter.value()->author(), 
                    Qt::CaseInsensitive))));
            writer.writeStartElement(QStringLiteral("text"));
            commentLines = comColIter.value()->text().toPlainString().split(
                QRegExp("(?:\r\n)|\r|\n"));
            for (QStringList::const_iterator comLinIter = commentLines.constBegin();
                comLinIter != commentLines.constEnd(); ++comLinIter) {
                writer.writeStartElement(QStringLiteral("r"));
                //////////////////////////////////////////////////////////////////////////
                writer.writeStartElement(QStringLiteral("rPr"));
                writer.writeEmptyElement(QStringLiteral("sz"));
                writer.writeAttribute(QStringLiteral("val"), QStringLiteral("9"));
                writer.writeEmptyElement(QStringLiteral("color"));
                writer.writeAttribute(QStringLiteral("indexed"), QStringLiteral("81"));
                writer.writeEmptyElement(QStringLiteral("rFont"));
                writer.writeAttribute(QStringLiteral("val"), QStringLiteral("Tahoma"));
                writer.writeEmptyElement(QStringLiteral("family"));
                writer.writeAttribute(QStringLiteral("val"), QStringLiteral("2"));
                writer.writeEndElement();//rPr
                //////////////////////////////////////////////////////////////////////////
                writer.writeStartElement(QStringLiteral("t"));
                writer.writeAttribute(QStringLiteral("xml:space"), QStringLiteral("preserve"));
                writer.writeCharacters(*comLinIter);
                writer.writeEndElement();//t
                writer.writeEndElement();//r
            }
            writer.writeEndElement();//text
            writer.writeEndElement();//comment
        }
    }
    writer.writeEndElement();//commentList
    writer.writeEndElement();//comments
}

bool SpreadSheetComment::loadFromXmlFile(QIODevice *device)
{
    return true;
}
SpreadSheetComment::SpreadSheetComment(CreateFlag flag)
    : AbstractOOXmlFile(new SpreadSheetCommentPrivate(this, flag))
{}

bool SpreadSheetComment::hasComment(int row, int col) const
{
    Q_D(const SpreadSheetComment);
    return d->getComment(row, col);
}

bool SpreadSheetComment::hasComment(const CellReference& rowCol) const
{
    if (rowCol.isValid()) return hasComment(rowCol.row(), rowCol.column());
    return false;
}

bool SpreadSheetComment::hasRow(int row) const
{
    Q_D(const SpreadSheetComment);
    return d->hasRow(row);
}

bool SpreadSheetComment::isEmpty() const
{
    Q_D(const SpreadSheetComment);
    return d->isEmpty();
}

SpreadSheetComment::~SpreadSheetComment()
{}

QByteArray SpreadSheetComment::saveShapeToXmlData() const
{
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    saveShapeToXmlFile(&buffer);
    return data;
}
QT_END_NAMESPACE_XLSX

