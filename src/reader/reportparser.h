#ifndef REPORTPARSER_H
#define REPORTPARSER_H

// Qt5
#include <QDialog>
#include "ui_reportparser.h"

class ParserReport;

class ReportParser : public QDialog
{
    Q_OBJECT

public:
    ReportParser(QWidget *parent, int id, QString name);
    ~ReportParser();
private slots:
    void apply();
signals:
    void parserReport(ParserReport *pr); // Deleted after this

private:
    int parserid;
    Ui::ReportParserClass ui;
};

#endif // REPORTPARSER_H
