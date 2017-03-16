/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
 * http://ugene.net
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

#include <U2Core/U2SafePoints.h>

#include "GenericSqlHelpers.h"
#include "GenericSqlModificationAction.h"
#include "dbi/GenericSqlDbi.h"
#include "dbi/GenericSqlModDbi.h"
#include "dbi/GenericSqlObjectDbi.h"

namespace U2 {

GenericSqlModificationAction::GenericSqlModificationAction(GenericSqlDbi* _dbi, const U2DataId& _masterObjId) :
    dbi(_dbi),
    masterObjId(_masterObjId),
    trackMod(NoTrack)
{
    objIds.insert(masterObjId);
}

U2TrackModType GenericSqlModificationAction::prepare(U2OpStatus& os) {
    CHECK_OP(os, NoTrack);
    GenericSqlTransaction t(dbi->getDbRef(), os);
    Q_UNUSED(t);

    trackMod = dbi->getObjectDbi()->getTrackModType(masterObjId, os);
    if (os.hasError()) {
        trackMod = NoTrack;
        FAIL("Failed to get trackMod", NoTrack);
    }

    if (TrackOnUpdate == trackMod) {
        qint64 masterObjVersionToTrack = dbi->getObjectDbi()->getObjectVersion(masterObjId, os);
        CHECK_OP(os, trackMod);

        // If a user mod step has already been created for this action
        // then it can not be deleted. The version must be incremented.
        // Obsolete duplicate step must be deleted
        if (dbi->getSqlModDbi()->isUserStepStarted(masterObjId)) {
            dbi->getSqlModDbi()->removeDuplicateUserStep(masterObjId, masterObjVersionToTrack, os);

            // Increment the object version
            masterObjVersionToTrack++;
        }

        // A user pressed "Undo" (maybe several times), did another action => there is no more "Redo" history
        dbi->getSqlModDbi()->removeModsWithGreaterVersion(masterObjId, masterObjVersionToTrack, os);
        if (os.hasError()) {
            dbi->getSqlModDbi()->cleanUpAllStepsOnError();
            return trackMod;
        }
    }

    return trackMod;
}

void GenericSqlModificationAction::addModification(const U2DataId& objId, qint64 modType, const QByteArray& modDetails, U2OpStatus& os) {
    CHECK_OP(os, );

    objIds.insert(objId);

    if (TrackOnUpdate == trackMod) {
        SAFE_POINT(!modDetails.isEmpty(), "Empty modification details", );

        qint64 objVersion = dbi->getObjectDbi()->getObjectVersion(objId, os);
        CHECK_OP(os, );

        if ((objId == masterObjId) && (dbi->getSqlModDbi()->isUserStepStarted(masterObjId))) {
            objVersion++;
        }

        U2SingleModStep singleModStep;
        singleModStep.objectId = objId;
        singleModStep.version = objVersion;
        singleModStep.modType = modType;
        singleModStep.details = modDetails;

        singleSteps.append(singleModStep);
    }
}

void GenericSqlModificationAction::complete(U2OpStatus& os) {
    // TODO: rewrite it with another U2UseCommonMultiModStep
    CHECK_OP(os, );
    GenericSqlTransaction t(dbi->getDbRef(), os);
    Q_UNUSED(t);

    // Save modification tracks, if required
    if (TrackOnUpdate == trackMod) {
        if (0 == singleSteps.size()) {
            // do nothing
        }
        else if (1 == singleSteps.size()) {
            dbi->getSqlModDbi()->createModStep(masterObjId, singleSteps.first(), os);
            CHECK_OP(os, );
        }
        else {
            GenericSqlUseCommonMultiModStep multi(dbi, masterObjId, os);
            CHECK_OP(os, );
            Q_UNUSED(multi);

            foreach (U2SingleModStep singleStep, singleSteps) {
                dbi->getSqlModDbi()->createModStep(masterObjId, singleStep, os);
                CHECK_OP(os, );
            }
        }
    }

    // Increment versions of all objects
    foreach (const U2DataId& objId, objIds) {
        GenericSqlObjectDbi::incrementVersion(objId, dbi->getDbRef(), os);
        CHECK_OP(os, );
    }
}

U2TrackModType GenericSqlModificationAction::getTrackModType() const {
    return trackMod;
}

}   // namespace U2
