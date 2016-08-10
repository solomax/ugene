/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2016 UniPro <ugene@unipro.ru>
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

#include <U2Core/DNAChromatogram.h>
#include <U2Core/Log.h>
#include <U2Core/L10n.h>
#include <U2Core/U2OpStatus.h>

#include "ChromatogramUtils.h"

namespace U2 {

void ChromatogramUtils::append(DNAChromatogram chromatogram, const DNAChromatogram &appendedChromatogram) {
    chromatogram.traceLength += appendedChromatogram.traceLength;
    chromatogram.seqLength += appendedChromatogram.seqLength;
    chromatogram.baseCalls += appendedChromatogram.baseCalls;
    chromatogram.A += appendedChromatogram.A;
    chromatogram.C += appendedChromatogram.C;
    chromatogram.G += appendedChromatogram.G;
    chromatogram.T += appendedChromatogram.T;
    chromatogram.prob_A += appendedChromatogram.prob_A;
    chromatogram.prob_C += appendedChromatogram.prob_C;
    chromatogram.prob_G += appendedChromatogram.prob_G;
    chromatogram.prob_T += appendedChromatogram.prob_T;
    chromatogram.hasQV &= appendedChromatogram.hasQV;
}

void ChromatogramUtils::remove(U2OpStatus &os, DNAChromatogram &chromatogram, int startPos, int endPos) {
    if ((endPos <= startPos) || (startPos < 0) || (endPos > chromatogram.traceLength)) {
        coreLog.trace(L10N::internalError("incorrect parameters was passed to ChromatogramUtils::remove, "
            "startPos '%1', endPos '%2', chromatogram length '%3'").arg(startPos).arg(endPos).arg(chromatogram.traceLength));
        os.setError("Can't remove chars from a chromatogram");
        return;
    }

    const int regionLength = endPos - startPos;
    chromatogram.traceLength -= regionLength;
    chromatogram.seqLength -= regionLength;
    chromatogram.baseCalls.remove(startPos, regionLength);
    chromatogram.A.remove(startPos, regionLength);
    chromatogram.C.remove(startPos, regionLength);
    chromatogram.G.remove(startPos, regionLength);
    chromatogram.T.remove(startPos, regionLength);
    chromatogram.prob_A.remove(startPos, regionLength);
    chromatogram.prob_C.remove(startPos, regionLength);
    chromatogram.prob_G.remove(startPos, regionLength);
    chromatogram.prob_T.remove(startPos, regionLength);
}

bool ChromatogramUtils::areEqual(const DNAChromatogram &first, const DNAChromatogram &second) {
    return first.traceLength == second.traceLength &&
            first.seqLength == second.seqLength &&
            first.baseCalls == second.baseCalls &&
            first.A == second.A &&
            first.C == second.C &&
            first.G == second.G &&
            first.T == second.T &&
            first.prob_A == second.prob_A &&
            first.prob_C == second.prob_C &&
            first.prob_G == second.prob_G &&
            first.prob_T == second.prob_T &&
            first.hasQV == second.hasQV;
}

}   // namespace U2