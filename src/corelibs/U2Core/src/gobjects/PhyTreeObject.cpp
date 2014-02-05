/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2014 UniPro <ugene@unipro.ru>
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

#include "PhyTreeObject.h"

namespace U2 {

GObject* PhyTreeObject::clone(const U2DbiRef&, U2OpStatus&) const {
    PhyTreeObject* cln = new PhyTreeObject(tree, getGObjectName(), getGHintsMap());
    cln->setIndexInfo(getIndexInfo());
    return cln;
}

PhyTreeObject* PhyTreeObject::clone() const {
    PhyTreeObject* cln = new PhyTreeObject(tree, getGObjectName(), getGHintsMap());
    cln->setIndexInfo(getIndexInfo());
    return cln;
}

void PhyTreeObject::setTree(const PhyTree& _tree) {
    tree = _tree; 
    emit si_phyTreeChanged();
}

void PhyTreeObject::rerootPhyTree(PhyNode* node) {
    PhyTreeUtils::rerootPhyTree(tree, node);
    emit si_phyTreeChanged();
}

bool PhyTreeObject::treesAreAlike( const PhyTree& tree1, const PhyTree& tree2 )
{
    
    QList<const PhyNode*> track1 =  tree1->collectNodes();
    QList<const PhyNode*> track2 =  tree2->collectNodes();
    if (track1.count() != track2.count()) {
        return false;
    }

    foreach (const PhyNode* n1, track1) {
        if (n1->getName().isEmpty()) {
            continue;
        } 
        foreach (const PhyNode* n2, track2) {
            if (n2->getName() != n1->getName()) {
                continue;
            }
            if (n1->getNumberOfBranches() != n2->getNumberOfBranches()) {
                return false;
            }
        }


    }

    return true;
}

const PhyNode* PhyTreeObject::findPhyNodeByName( const QString& name )
{
    QList<const PhyNode*> nodes = tree.constData()->collectNodes();
    foreach (const PhyNode* node, nodes) {
        if (node->getName() == name) {
            return node;
        }
    }
    return NULL;
}

}//namespace


