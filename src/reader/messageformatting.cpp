#include "messageformatting.h"

MessageFormatting::MessageFormatting() {
}

MessageFormatting::~MessageFormatting() {
}

QString MessageFormatting::sanitize(QString &txt) {
    QString s = stripHtml(txt);
    s.replace('\n', "");
    s.replace(QRegExp("\\s+"), " ");
    return s;
}

QString MessageFormatting::stripHtml(QString &txt) {
    txt.replace(QRegExp("<.*>"), "");
    txt.replace("&amp;", "&");
    txt.replace("&quot;", "\"");
    txt.replace("&lt;", "<");
    txt.replace("&gt;", ">");
    txt.replace("&nbsp;", " ");
    return txt;
}
