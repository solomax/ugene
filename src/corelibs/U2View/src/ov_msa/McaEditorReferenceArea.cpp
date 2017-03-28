/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
 * http://ugene.unipro.ru
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "McaEditorReferenceArea.h"
#include "McaEditor.h"
#include "McaEditorSequenceArea.h"

#include <U2Core/DNASequenceSelection.h>
#include <U2View/ADVSequenceObjectContext.h> // SANGER_TODO: rename
#include <U2Core/U2SafePoints.h>

namespace U2 {

McaEditorReferenceArea::McaEditorReferenceArea(McaEditorWgt *p, SequenceObjectContext *ctx)
    : PanView(p, ctx, McaReferenceAreaRendererFactory(NULL != p ? p->getEditor() : NULL)),
      editor(NULL != p ? p->getEditor() : NULL),
      renderer(dynamic_cast<McaReferenceAreaRenderer *>(getRenderArea()->getRenderer()))
{
    SAFE_POINT(NULL != renderer, "Renderer is NULL", );

    setLocalToolbarVisible(false);
    settings->showMainRuler = false;

    scrollBar->hide();
    rowBar->hide();

    connect(p->getSequenceArea(), SIGNAL(si_visibleRangeChanged()), SLOT(sl_visibleRangeChanged()));
    connect(p->getSequenceArea(), SIGNAL(si_selectionChanged(MaEditorSelection,MaEditorSelection)),
            SLOT(sl_selectionChanged(MaEditorSelection,MaEditorSelection)));

    connect(p->getSequenceArea(), SIGNAL(si_clearReferenceSelection()),
            SLOT(sl_clearSelection()));

    connect(ctx->getSequenceSelection(),
        SIGNAL(si_selectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>&)),
        SLOT(sl_onSelectionChanged()));

    connect(this, SIGNAL(si_selectionChanged()),
            p->getSequenceArea(), SLOT(sl_referenceSelectionChanged()));
    connect(editor, SIGNAL(si_fontChanged(const QFont &)), SLOT(sl_fontChanged(const QFont &)));

    sl_fontChanged(editor->getFont());
}

void McaEditorReferenceArea::sl_visibleRangeChanged() {
    int start = editor->getUI()->getSequenceArea()->getFirstVisibleBase();
    int len = editor->getUI()->getSequenceArea()->getNumVisibleBases(false);

    U2Region visRange(start, len);
    setVisibleRange(visRange);
}

void McaEditorReferenceArea::sl_selectionChanged(const MaEditorSelection &current, const MaEditorSelection &) {
    U2Region selection(current.x(), current.width());
    setSelection(selection);
}

void McaEditorReferenceArea::sl_clearSelection() {
    ctx->getSequenceSelection()->clear();
}

void McaEditorReferenceArea::sl_fontChanged(const QFont &newFont) {
    renderer->setFont(newFont);
    setFixedHeight(renderer->getMinimumHeight());
}

void McaEditorReferenceArea::sl_onSelectionChanged() {
    emit si_selectionChanged();
}

} // namespace
