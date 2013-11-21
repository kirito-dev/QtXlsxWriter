#include "private/xlsxstyles_p.h"
#include "private/xlsxxmlreader_p.h"
#include "xlsxformat.h"
#include "private/xlsxformat_p.h"
#include <QString>
#include <QtTest>

class StylesTest : public QObject
{
    Q_OBJECT

public:
    StylesTest();

private Q_SLOTS:
    void testEmptyStyle();
    void testAddFormat();
    void testAddFormat2();
    void testSolidFillBackgroundColor();

    void testWriteBorders();

    void testReadNumFmts();
    void testReadFonts();
    void testReadFills();
    void testReadBorders();
};

StylesTest::StylesTest()
{
}

void StylesTest::testEmptyStyle()
{
    QXlsx::Styles styles;
    QByteArray xmlData = styles.saveToXmlData();

    QVERIFY2(xmlData.contains("<cellXfs count=\"1\"><xf numFmtId=\"0\" fontId=\"0\" fillId=\"0\" borderId=\"0\" xfId=\"0\"/></cellXfs>"), "Must have one cell style");
}

void StylesTest::testAddFormat()
{
    QXlsx::Styles styles;

    for (int i=0; i<10; ++i) {
        QXlsx::Format format;
        format.setFontBold(true);
        styles.addFormat(format);
    }

    QByteArray xmlData = styles.saveToXmlData();
    QVERIFY2(xmlData.contains("<cellXfs count=\"2\">"), ""); //Note we have a default one
}

void StylesTest::testAddFormat2()
{
    QXlsx::Styles styles;

    QXlsx::Format format;
    format.setNumberFormat("h:mm:ss AM/PM"); //builtin 19
    styles.addFormat(format);

    QCOMPARE(format.numberFormatIndex(), 19);

    QXlsx::Format format2;
    format2.setNumberFormat("aaaaa h:mm:ss AM/PM"); //custom
    styles.addFormat(format2);

    QCOMPARE(format2.numberFormatIndex(), 176);
}

// For a solid fill, Excel reverses the role of foreground and background colours
void StylesTest::testSolidFillBackgroundColor()
{
    QXlsx::Styles styles;
    QXlsx::Format format;
    format.setPatternBackgroundColor(QColor(Qt::red));
    styles.addFormat(format);

    QByteArray xmlData = styles.saveToXmlData();

    QVERIFY(xmlData.contains("<patternFill patternType=\"solid\"><fgColor rgb=\"FFff0000\"/>"));
}

void StylesTest::testWriteBorders()
{
    QXlsx::Styles styles;
    QXlsx::Format format;
    format.setRightBorderStyle(QXlsx::Format::BorderThin);
    styles.addFormat(format);

    QByteArray xmlData = styles.saveToXmlData();

    QVERIFY(xmlData.contains("<border><right style=\"thin\">"));
}

void StylesTest::testReadFonts()
{
    QByteArray xmlData = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
            "<fonts count=\"3\">"
            "<font><sz val=\"11\"/><name val=\"Calibri\"/><family val=\"2\"/><scheme val=\"minor\"/></font>"
            "<font><sz val=\"15\"/><color rgb=\"FFff0000\"/><name val=\"Calibri\"/><family val=\"2\"/><scheme val=\"minor\"/></font>"
            "<font><b/><u val=\"double\"/><sz val=\"11\"/><name val=\"Calibri\"/><family val=\"2\"/><scheme val=\"minor\"/></font>"
            "</fonts>";
    QXlsx::Styles styles(true);
    QXlsx::XmlStreamReader reader(xmlData);
    reader.readNextStartElement();//So current node is fonts
    styles.readFonts(reader);

    QCOMPARE(styles.m_fontsList.size(), 3);
    QXlsx::Format font0 = styles.m_fontsList[0];
    QCOMPARE(font0.fontSize(), 11);
    QCOMPARE(font0.fontName(), QString("Calibri"));
}

void StylesTest::testReadFills()
{
    QByteArray xmlData = "<fills count=\"4\">"
            "<fill><patternFill patternType=\"none\"/></fill>"
            "<fill><patternFill patternType=\"gray125\"/></fill>"
            "<fill><patternFill patternType=\"lightUp\"/></fill>"
            "<fill><patternFill patternType=\"solid\"><fgColor rgb=\"FFa0a0a4\"/></patternFill></fill>"
            "</fills>";
    QXlsx::Styles styles(true);
    QXlsx::XmlStreamReader reader(xmlData);
    reader.readNextStartElement();//So current node is fills
    styles.readFills(reader);

    QCOMPARE(styles.m_fillsList.size(), 4);
    QCOMPARE(styles.m_fillsList[3].fillPattern(), QXlsx::Format::PatternSolid);
    QCOMPARE(styles.m_fillsList[3].patternBackgroundColor(), QColor(Qt::gray));//for solid pattern, bg vs. fg color!
}

void StylesTest::testReadBorders()
{
    QByteArray xmlData ="<borders count=\"2\">"
            "<border><left/><right/><top/><bottom/><diagonal/></border>"
            "<border><left style=\"dashDotDot\"><color auto=\"1\"/></left><right style=\"dashDotDot\"><color auto=\"1\"/></right><top style=\"dashDotDot\"><color auto=\"1\"/></top><bottom style=\"dashDotDot\"><color auto=\"1\"/></bottom><diagonal/></border>"
            "</borders>";

    QXlsx::Styles styles(true);
    QXlsx::XmlStreamReader reader(xmlData);
    reader.readNextStartElement();//So current node is borders
    styles.readBorders(reader);

    QCOMPARE(styles.m_bordersList.size(), 2);
}

void StylesTest::testReadNumFmts()
{
    QByteArray xmlData ="<numFmts count=\"2\">"
            "<numFmt numFmtId=\"164\" formatCode=\"yyyy-mm-ddThh:mm:ss\"/>"
            "<numFmt numFmtId=\"165\" formatCode=\"dd/mm/yyyy\"/>"
            "</numFmts>";

    QXlsx::Styles styles(true);
    QXlsx::XmlStreamReader reader(xmlData);
    reader.readNextStartElement();//So current node is numFmts
    styles.readNumFmts(reader);

    QCOMPARE(styles.m_customNumFmtIdMap.size(), 2);
    QVERIFY(styles.m_customNumFmtIdMap.contains(164));
    QCOMPARE(styles.m_customNumFmtIdMap[164]->formatString, QStringLiteral("yyyy-mm-ddThh:mm:ss"));
    QVERIFY(styles.m_customNumFmtIdMap.contains(165));
    QCOMPARE(styles.m_customNumFmtIdMap[165]->formatString, QStringLiteral("dd/mm/yyyy"));
}

QTEST_APPLESS_MAIN(StylesTest)

#include "tst_stylestest.moc"
