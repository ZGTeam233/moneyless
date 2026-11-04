//
// Created by wsnzg6 on 2026/11/4.
// Copyright(c) 2026 ZGTeam233.
//

#include "xmlledgerstore.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

static void setError(QString* error, const QString& message)
{
    if (error)
        *error = message;
}

bool XmlLedgerStore::load(const QString& filePath, LedgerData& out, QString* error)
{
    QFile file(filePath);

    if (!file.exists()) {
        out = LedgerData{};
        return true;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        setError(error, QStringLiteral("无法打开文件：%1").arg(filePath));
        return false;
    }

    QXmlStreamReader xml(&file);
    LedgerData data;

    while (!xml.atEnd()) {
        const auto token = xml.readNext();

        if (token != QXmlStreamReader::StartElement)
            continue;

        const auto name = xml.name();
        const auto attrs = xml.attributes();

        if (name == u"ledger") {
            const QString version = attrs.value(QStringLiteral("version")).toString();
            if (version != QStringLiteral("1")) {
                setError(error, QStringLiteral("不支持的账本版本：%1").arg(version));
                return false;
            }

            const QString currency = attrs.value(QStringLiteral("baseCurrency")).toString();
            if (!currency.isEmpty())
                data.baseCurrency = currency;
        } else if (name == u"account") {
            Account a;
            a.id = QUuid(attrs.value(QStringLiteral("id")).toString());
            a.name = attrs.value(QStringLiteral("name")).toString();
            a.type = attrs.value(QStringLiteral("type")).toString();
            data.accounts.push_back(a);
            xml.skipCurrentElement();
        } else if (name == u"category") {
            Category c;
            c.id = QUuid(attrs.value(QStringLiteral("id")).toString());
            c.name = attrs.value(QStringLiteral("name")).toString();
            c.kind = attrs.value(QStringLiteral("kind")).toString();
            data.categories.push_back(c);
            xml.skipCurrentElement();
        } else if (name == u"transaction") {
            Transaction t;
            t.id = QUuid(attrs.value(QStringLiteral("id")).toString());
            t.date = QDate::fromString(attrs.value(QStringLiteral("date")).toString(), Qt::ISODate);
            t.type = txTypeFromString(attrs.value(QStringLiteral("type")).toString());

            bool ok = false;
            t.amountCents = attrs.value(QStringLiteral("amountCents")).toString().toLongLong(&ok);

            if (!ok || t.amountCents < 0 || !t.date.isValid()) {
                setError(error, QStringLiteral("交易记录格式错误"));
                return false;
            }

            t.accountId = QUuid(attrs.value(QStringLiteral("accountId")).toString());
            t.categoryId = QUuid(attrs.value(QStringLiteral("categoryId")).toString());
            t.note = attrs.value(QStringLiteral("note")).toString();

            data.transactions.push_back(t);
            xml.skipCurrentElement();
        }
    }

    if (xml.hasError()) {
        setError(error, QStringLiteral("XML 解析失败：%1").arg(xml.errorString()));
        return false;
    }

    out = std::move(data);
    return true;
}

bool XmlLedgerStore::save(const QString& filePath, const LedgerData& data, QString* error)
{
    const QFileInfo info(filePath);
    QDir dir(info.absolutePath());

    if (!dir.exists() && !QDir().mkpath(dir.absolutePath())) {
        setError(error, QStringLiteral("无法创建数据目录：%1").arg(dir.absolutePath()));
        return false;
    }

    QSaveFile file(filePath);

    if (!file.open(QIODevice::WriteOnly)) {
        setError(error, QStringLiteral("无法写入文件：%1").arg(filePath));
        return false;
    }

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();

    xml.writeStartElement(QStringLiteral("ledger"));
    xml.writeAttribute(QStringLiteral("version"), QStringLiteral("1"));
    xml.writeAttribute(QStringLiteral("baseCurrency"), data.baseCurrency);

    xml.writeStartElement(QStringLiteral("accounts"));
    for (const auto& a : data.accounts) {
        xml.writeEmptyElement(QStringLiteral("account"));
        xml.writeAttribute(QStringLiteral("id"), a.id.toString(QUuid::WithoutBraces));
        xml.writeAttribute(QStringLiteral("name"), a.name);
        xml.writeAttribute(QStringLiteral("type"), a.type);
    }
    xml.writeEndElement();

    xml.writeStartElement(QStringLiteral("categories"));
    for (const auto& c : data.categories) {
        xml.writeEmptyElement(QStringLiteral("category"));
        xml.writeAttribute(QStringLiteral("id"), c.id.toString(QUuid::WithoutBraces));
        xml.writeAttribute(QStringLiteral("name"), c.name);
        xml.writeAttribute(QStringLiteral("kind"), c.kind);
    }
    xml.writeEndElement();

    xml.writeStartElement(QStringLiteral("transactions"));
    for (const auto& t : data.transactions) {
        xml.writeEmptyElement(QStringLiteral("transaction"));
        xml.writeAttribute(QStringLiteral("id"), t.id.toString(QUuid::WithoutBraces));
        xml.writeAttribute(QStringLiteral("date"), t.date.toString(Qt::ISODate));
        xml.writeAttribute(QStringLiteral("type"), txTypeToString(t.type));
        xml.writeAttribute(QStringLiteral("amountCents"), QString::number(t.amountCents));
        xml.writeAttribute(QStringLiteral("accountId"), t.accountId.toString(QUuid::WithoutBraces));
        xml.writeAttribute(QStringLiteral("categoryId"), t.categoryId.toString(QUuid::WithoutBraces));
        xml.writeAttribute(QStringLiteral("note"), t.note);
    }
    xml.writeEndElement();

    xml.writeEndElement();
    xml.writeEndDocument();

    if (xml.hasError()) {
        file.cancelWriting();
        setError(error, QStringLiteral("XML 写入失败"));
        return false;
    }

    if (!file.commit()) {
        setError(error, QStringLiteral("提交保存失败：%1").arg(file.errorString()));
        return false;
    }

    return true;
}
