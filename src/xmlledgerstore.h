//
// Created by wsnzg6 on 2026/11/4.
// Copyright(c) 2026 ZGTeam233.
//

#pragma once

#include "domain.h"
#include <QString>

class XmlLedgerStore
{
public:
    static bool load(const QString& filePath, LedgerData& out, QString* error = nullptr);
    static bool save(const QString& filePath, const LedgerData& data, QString* error = nullptr);
};
