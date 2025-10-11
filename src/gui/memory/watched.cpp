/***************************************************************************
 *   Copyright (C) 2023 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include "watched.h"
#include "watcher.h"

Watched::Watched()
{
    m_display = nullptr;
    m_watcher = nullptr;
}
Watched::~Watched() {}

void Watched::createWatcher()
{
    if( !m_watcher ) m_watcher = new Watcher( nullptr, this );
}
